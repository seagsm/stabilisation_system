/*
 * https://github.com/mahowik/MahoRotorF4-Discovery/blob/master/src/drv/drv_ms5611.c
 * http://www.daedalus.ei.tum.de/attachments/article/61/MS5611-01BA01.pdf
 */


#include "board_drv_ms5611.h"


static uint16_t ms5611_prom(uint8_t u8_coef_num);
static BOARD_ERROR ms5611_crc(uint16_t u16_prom[]);
static BOARD_ERROR ms5611_read_adc(uint32_t *pu32_value);
static BOARD_ERROR ms5611_start_ut(void);
static BOARD_ERROR ms5611_get_ut(void);
static BOARD_ERROR ms5611_start_up(void);
static BOARD_ERROR ms5611_get_up(void);
static void ms5611_calculate(int32_t *pressure, int32_t *temperature);

static uint32_t ms5611_ut;                  /* static result of temperature measurement */
static uint32_t ms5611_up;                  /* static result of pressure measurement */
static uint16_t ms5611_c[PROM_NB];          /* on-chip ROM */
static uint8_t ms5611_osr = CMD_ADC_4096;

static baro_t local_baro;

static MS5611_STATE_CONDITION local_msc_state;

static uint8_t u8_data[3] = {0U};


uint32_t u32_board_drv_ms5611_get_filtered_pressure(void)
{
    return(local_baro.u32_filtered_pressure);
}
void v_board_drv_ms5611_set_filtered_pressure(uint32_t u32_filtered_pressure)
{
    local_baro.u32_filtered_pressure = u32_filtered_pressure;
}

/* Function get correct temperature value from baro module. */
int32_t i32_board_drv_ms5611_get_temperature(void)
{
    return( local_baro.i32_temperature);
}

/* Function get correct pressure value from baro module. */
int32_t i32_board_drv_ms5611_get_pressure(void)
{
    return((int32_t) local_baro.i32_pressure);
}

BOARD_ERROR be_board_drv_ms5611_set_conversion_state(MS5611_STATE_CONDITION msc_state)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    local_msc_state = msc_state;
    return(be_result);
}

BOARD_ERROR be_board_drv_ms5611_get_conversion_state(MS5611_STATE_CONDITION *msc_state)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    *msc_state = local_msc_state;
    return(be_result);
}

BOARD_ERROR be_board_drv_ms5611_state_machine(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    baro_t *pb_baro;
    MS5611_STATE_CONDITION msc_state;
    /* uint32_t u32_value[2]; */


    be_result = board_drv_get_baro(&pb_baro);
    be_result = be_board_drv_ms5611_get_conversion_state(&msc_state);

    switch(msc_state)
    {   /* Send "start pressure conversion" command:*/
        case MS5611_START_PRESS_CONVERSION :
            /* Send "start time" point: */
            pb_baro->up_delay = gu64_read_system_time() + MS5611_CONVERSION_TIME;
            /* Set "pressure conversion" command value: */
            u8_data[0] = CMD_ADC_CONV + CMD_ADC_D1 + ms5611_osr;
            /* Write "pressure conversation" command: */
            be_result = be_board_i2c_write_start(u8_data, 1U, MS5611_ADDR);
            /* Change state to next state "MS5611_START_READ_UNCOMP_PRESS": */
            be_result = be_board_drv_ms5611_set_conversion_state(MS5611_START_READ_UNCOMP_PRESS);
            break;
        /* Start read pressure raw value, first of all we must write read command: */
        case MS5611_START_READ_UNCOMP_PRESS:
            if(pb_baro->up_delay <= gu64_read_system_time())
            {
                u8_data[0] = CMD_ADC_READ;
                /* Set read ADC command: */
                be_result = be_board_i2c_write_start(u8_data, 1U, MS5611_ADDR);
                be_result = be_board_drv_ms5611_set_conversion_state(MS5611_READ_UNCOMPENSATED_PRESS);
            }
            else
            {
                /* Set IMU data ready flag. Baro will be done at next frame. */
                api_i2c_data.u8_ready = 1U;
            }
            break;
        /* Read pressure value: */
        case MS5611_READ_UNCOMPENSATED_PRESS:
            /* Initialisation of read interrupt and DMA */
/*rd*/      be_result = be_board_i2c_read_start(u8_data, 3U, MS5611_ADDR);
            /* Change state to next state "MS5611_WAIT_FOR_PRESS_DATA_READY": */
            be_result = be_board_drv_ms5611_set_conversion_state(MS5611_WAIT_FOR_PRESS_DATA_READY);
            break;

        case MS5611_WAIT_FOR_PRESS_DATA_READY:
            /* Raw pressure */
            board_drv_get_raw_value(&ms5611_up);
            /* Set IMU data ready flag. Baro will be done at next frame. */
            api_i2c_data.u8_ready = 1U;
            be_result = be_board_drv_ms5611_set_conversion_state(MS5611_START_TEMP_CONVERSION);
            break;

        case MS5611_START_TEMP_CONVERSION:
            pb_baro->ut_delay = gu64_read_system_time() + MS5611_CONVERSION_TIME;
            /* Start pressure conversation: */
            u8_data[0] = CMD_ADC_CONV + CMD_ADC_D2 + ms5611_osr;
            be_result = be_board_i2c_write_start(u8_data, 1U, MS5611_ADDR);
            be_result = be_board_drv_ms5611_set_conversion_state(MS5611_START_READ_UNCOMP_TEMP);
            break;

        case MS5611_START_READ_UNCOMP_TEMP:
            if(pb_baro->ut_delay <= gu64_read_system_time())
            {
                u8_data[0] = CMD_ADC_READ;
                /* Set read ADC command: */
                be_result = be_board_i2c_write_start(u8_data, 1U, MS5611_ADDR);
                be_result = be_board_drv_ms5611_set_conversion_state(MS5611_READ_UNCOMPENSATED_TEMP);
            }
            else
            {
                /* Set IMU data ready flag. Baro will be done at next frame. */
                api_i2c_data.u8_ready = 1U;
            }
            break;

        case MS5611_READ_UNCOMPENSATED_TEMP:
            /* Initialisation of read interrupt and DMA */
/*rd*/      be_result = be_board_i2c_read_start(u8_data, 3U, MS5611_ADDR);
            be_result = be_board_drv_ms5611_set_conversion_state(MS5611_WAIT_FOR_TEMP_DATA_READY);
            break;

        case MS5611_WAIT_FOR_TEMP_DATA_READY:
            /* Raw temperature: */
            board_drv_get_raw_value(&ms5611_ut);
            /* Set IMU data ready flag. Baro will be done at next frame. */
            api_i2c_data.u8_ready = 1U;
            be_result = be_board_drv_ms5611_set_conversion_state(MS5611_CALCULATION);
            pb_baro->calculate(&(pb_baro->i32_pressure),&(pb_baro->i32_temperature));
            break;

        default:
            be_result = BOARD_ERR_STATE;
            break;
    }
    return(be_result);
}

static BOARD_ERROR board_drv_get_raw_value(uint32_t *pu32_variable)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint32_t u32_value[2];

    u32_value[0] = (uint32_t)u8_data[0];
    u32_value[0] = u32_value[0] << 16U;
    u32_value[1] = (uint32_t)u8_data[1];
    u32_value[1] = u32_value[1] << 8U;
    u32_value[0] = u32_value[0] | u32_value[1];
    u32_value[1] = (uint32_t)u8_data[2];
    u32_value[0] = u32_value[0] | u32_value[1];
    /* Raw temperature: */
    *pu32_variable = u32_value[0];
    return(be_result);
}

BOARD_ERROR board_drv_get_baro(baro_t **pb_baro)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
   *pb_baro = &local_baro;
    return(be_result);
}

BOARD_ERROR board_drv_ms5611Detect(baro_t *baro)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t u8_sig;
    uint8_t u8_i;
    uint16_t u16_value;

    gv_board_sys_tick_delay(10U); /* No idea how long the chip takes to power-up, but let's make it 10 mS */

    be_result = board_i2c_read( MS5611_ADDR, CMD_PROM_RD, 2U, &u8_sig);


    if (be_result == BOARD_ERR_OK)
    {
        /*ms5611_reset();*/
        /* read all coefficients */
        for (u8_i = 0U; u8_i < PROM_NB; u8_i++)
        {
            u16_value = ms5611_prom(u8_i);
            ms5611_c[u8_i] = u16_value;
        }
        /* check crc, bail out if wrong - we are probably talking to BMP085 w/o XCLR line! */
        if (ms5611_crc(ms5611_c) == 0U)
        {
            /* TODO prom + CRC */
            baro->ut_delay = 10000U;
            baro->up_delay = 10000U;
            baro->start_ut = ms5611_start_ut;
            baro->get_ut = ms5611_get_ut;
            baro->start_up = ms5611_start_up;
            baro->get_up = ms5611_get_up;
            baro->calculate = ms5611_calculate;
        }
    }
    return(be_result);
}

BOARD_ERROR board_drv_ms5611_reset(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    be_result = board_i2c_write_byte(MS5611_ADDR, CMD_RESET);
    gv_board_sys_tick_delay(2800U);
    return (be_result);
}

static uint16_t ms5611_prom(uint8_t u8_coef_num)
{
    uint16_t u16_value = 0U;
    uint8_t u8_rxbuf[2] = { 0, 0 };

    board_i2c_read( MS5611_ADDR, CMD_PROM_RD + u8_coef_num * 2U, 2U, u8_rxbuf); /* send PROM READ command */
    u16_value = (uint16_t)(u8_rxbuf[0]) << 8 ;
    u16_value = u16_value | ((uint16_t)(u8_rxbuf[1]));

    return (u16_value);
}

static BOARD_ERROR ms5611_crc(uint16_t u16_prom[])
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    int32_t     i32_i;
    int32_t     i32_j;
    uint16_t    u16_value;
    uint32_t    u32_res = 0U;
    uint8_t     u8_zero = 1U;
    uint8_t     u8_crc;
    uint8_t     u8_value;

    u8_crc = (uint8_t)(u16_prom[7] & 0x000FU);
    u16_prom[7] &= 0xFF00U;

    /* if eeprom is all zeros, we're probably fucked - BUT this will return valid CRC lol */
    for (i32_i = 0; i32_i < 8; i32_i++)
    {
        if (u16_prom[i32_i] != 0U)
        {
            u8_zero = 0U;
        }
    }
    if (u8_zero == 0U)
    {
        for (i32_i = 0; i32_i < 16; i32_i++)
        {
            if ( ((uint32_t)i32_i & 1U) == 1U)
            {

                 /* u32_res ^= (uint32_t)((u16_prom[(uint32_t)i32_i >> 1U]) & 0x00FF); */
                u16_value = u16_prom[(uint32_t)i32_i >> 1U];
                u16_value =  u16_value & 0x00FFU;
                u32_res  ^= (uint32_t)u16_value;
            }
            else
            {
                u16_value = (u16_prom[(uint32_t)i32_i >> 1U] >> 8U);
                u32_res ^= (uint32_t)u16_value;
            }
            for (i32_j = 8; i32_j > 0; i32_j--)
            {
                if ( (u32_res & 0x8000U) != 0U )
                {
                    u32_res ^= 0x1800U;
                }
                u32_res <<= 1U;
            }
        }
        u16_prom[7] |= u8_crc;

        u8_value = ((uint8_t)(u32_res >> 12U) & 0xFU);

        if (u8_crc == u8_value)
        {
            be_result = BOARD_ERR_OK;
        }
        else
        {
            be_result = BOARD_ERR_ERROR;
        }
    }
    else
    {
        be_result = BOARD_ERR_ERROR;
    }
    return be_result;
}

static BOARD_ERROR ms5611_read_adc(uint32_t *pu32_value)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t u8_rxbuf[3];
    uint32_t u32_value[2];

    /* i2cRead(MS5611_ADDR, CMD_ADC_READ, 3, rxbuf);  read ADC */
    be_result = board_i2c_read( MS5611_ADDR, CMD_ADC_READ, 3U, u8_rxbuf);

    /* ((rxbuf[0] << 16U) | (rxbuf[1] << 8U) | rxbuf[2]); */

    u32_value[0] = (uint32_t)u8_rxbuf[0];
    u32_value[0] = u32_value[0] << 16U;
    u32_value[1] = (uint32_t)u8_rxbuf[1];
    u32_value[1] = u32_value[1] << 8U;
    u32_value[0] = u32_value[0] | u32_value[1];
    u32_value[1] = (uint32_t)u8_rxbuf[2];
    u32_value[0] = u32_value[0] | u32_value[1];

    *pu32_value  = u32_value[0];
    /* return (u32_value[0]); */
    return (be_result);
}

/* D2 (temperature) conversion start! */
static BOARD_ERROR ms5611_start_ut(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    be_result = board_i2c_write_byte(MS5611_ADDR, CMD_ADC_CONV + CMD_ADC_D2 + ms5611_osr);
    return (be_result);
}

static BOARD_ERROR ms5611_get_ut(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    be_result = ms5611_read_adc(&ms5611_ut);
    return(be_result);
}

/* D1 (pressure) conversion start! */
static BOARD_ERROR ms5611_start_up(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    be_result = board_i2c_write_byte(MS5611_ADDR, CMD_ADC_CONV + CMD_ADC_D1 + ms5611_osr);
    return(be_result);
}

static BOARD_ERROR ms5611_get_up(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    be_result = ms5611_read_adc(&ms5611_up);
    return(be_result);
}

static void ms5611_calculate(int32_t *pressure, int32_t *temperature)
{
    uint32_t u32_press;
    int64_t i64_temp;
    int64_t i64_delt;
    int64_t i64_dT;
    int64_t i64_off;
    int64_t i64_sens;
    uint64_t u64_tmp0;
    int64_t  i64_tmp1;

    /* i32_dT = (int64_t)ms5611_ut - (int64_t)((uint64_t)ms5611_c[5] * 256); */
    i64_dT = (int64_t)ms5611_c[5] * 256;
    i64_dT = (int64_t)ms5611_ut - i64_dT;

    /* i64_off = ((int64_t)ms5611_c[2] << 16) + (((int64_t)ms5611_c[4] * i32_dT) >> 7); */
    u64_tmp0 = (uint64_t)ms5611_c[2];
    u64_tmp0 = u64_tmp0 << 16;
    i64_tmp1 = ((int64_t)ms5611_c[4] * i64_dT) / 128;
    i64_off = (int64_t)u64_tmp0 + i64_tmp1 ;

    /* i64_sens = ((int64_t)ms5611_c[1] << 15) + (((int64_t)ms5611_c[3] * i32_dT) >> 8); */
    i64_sens = ((int64_t)ms5611_c[1] * 32768) + (((int64_t)ms5611_c[3] * i64_dT) / 256);

    /* i64_temp = 2000 + ((i32_dT * (int64_t)ms5611_c[6]) >> 23); */
    i64_temp = 2000 + ((i64_dT * (int64_t)ms5611_c[6]) / 8388608);

    if (i64_temp < 2000)
    {
        /* temperature lower than 20degC */
        i64_delt = i64_temp - 2000;
        i64_delt = 5 * i64_delt * i64_delt;
        /* i64_off -= i64_delt >> 1; */
        i64_off -= i64_delt / 2;
        /* i64_sens -= i64_delt >> 2; */
        i64_sens -= i64_delt / 4;

        if (i64_temp < -1500)
        {
            /* temperature lower than -15degC */
            i64_delt = i64_temp + 1500;
            i64_delt = i64_delt * i64_delt;
            i64_off  -= 7 * i64_delt;
            i64_sens -= (11 * i64_delt) / 2;
        }
    }

    /* u32_press = ((((int64_t)ms5611_up * sens ) >> 21) - i64_off) >> 15; */

    i64_tmp1 = (int64_t)ms5611_up * i64_sens;
    i64_tmp1 = i64_tmp1 / 2097152;
    i64_tmp1 = (i64_tmp1 - i64_off) / 32768;
    u32_press = (uint32_t)i64_tmp1;

    if (pressure)
    {
        *pressure = (int32_t)u32_press; /* return pressure in Pascals. */
    }
    if (temperature)
    {
        *temperature = (int32_t)i64_temp; /* return Temperature in 0.01 Celsium degree. */
    }
}