/*
 * https://github.com/mahowik/MahoRotorF4-Discovery/blob/master/src/drv/drv_ms5611.c
 * http://www.daedalus.ei.tum.de/attachments/article/61/MS5611-01BA01.pdf
 */


#include "board_drv_ms5611.h"


static uint16_t ms5611_prom(int8_t coef_num);
static int8_t ms5611_crc(uint16_t *prom);
static uint32_t ms5611_read_adc(void);
static void ms5611_start_ut(void);
static void ms5611_get_ut(void);
static void ms5611_start_up(void);
static void ms5611_get_up(void);
static void ms5611_calculate(int32_t *pressure, int32_t *temperature);

static uint32_t ms5611_ut;                  /* static result of temperature measurement */
static uint32_t ms5611_up;                  /* static result of pressure measurement */
static uint16_t ms5611_c[PROM_NB];          /* on-chip ROM */
static uint8_t ms5611_osr = CMD_ADC_4096;


BOARD_ERROR ms5611Detect(baro_t *baro)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t u8_sig;
    uint8_t u8_i;

    gv_board_sys_tick_delay(10U); /* No idea how long the chip takes to power-up, but let's make it 10 mS */

    /* BMP085 is disabled. If we have a MS5611, it will reply. if no reply, means either */
    /* we have BMP085 or no baro at all. */
    be_result = board_i2c_read( MS5611_ADDR, CMD_PROM_RD, 1U, &u8_sig);

    if (be_result == BOARD_ERR_OK)
    {
        ms5611_reset();
        /* read all coefficients */
        for (u8_i = 0U; u8_i < PROM_NB; u8_i++)
        {
            ms5611_c[u8_i] = ms5611_prom(u8_i);
        }
        /* check crc, bail out if wrong - we are probably talking to BMP085 w/o XCLR line! */
        if (ms5611_crc(ms5611_c) == 0)
        {
            /* TODO prom + CRC */
            baro->ut_delay = 10000;
            baro->up_delay = 10000;
            baro->start_ut = ms5611_start_ut;
            baro->get_ut = ms5611_get_ut;
            baro->start_up = ms5611_start_up;
            baro->get_up = ms5611_get_up;
            baro->calculate = ms5611_calculate;
        }
    }
    return(be_result);
}

BOARD_ERROR ms5611_reset(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    be_result = board_i2c_write(MS5611_ADDR, CMD_RESET, 1);
    gv_board_sys_tick_delay(2800);
    return (be_result);
}

static uint16_t ms5611_prom(int8_t i8_coef_num)
{
    uint8_t u8_rxbuf[2] = { 0, 0 };
    /* i2cRead(MS5611_ADDR, CMD_PROM_RD + i8_coef_num * 2, 2, u8_rxbuf); /* send PROM READ command */
    be_result = board_i2c_read( MS5611_ADDR, CMD_PROM_RD + i8_coef_num * 2U, 2U, u8_rxbuf);
    return ((u8_rxbuf[0] << 8 )| u8_rxbuf[1]);
}

static BOARD_ERROR ms5611_crc(uint16_t *pu16_prom)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    int32_t     i32_i;
    int32_t     i32_j;
    uint32_t    u32_res = 0;
    uint8_t     u8_zero = 1;
    uint8_t     u8_crc;

    u8_crc = pu16_prom[7] & 0xFU;
    pu16_prom[7] &= 0xFF00U;

    /* if eeprom is all zeros, we're probably fucked - BUT this will return valid CRC lol */
    for (i32_i = 0; i32_i < 8; i32_i++)
    {
        if (pu16_prom[i32_i] != 0)
        {
            u8_zero = 0;
        }
    }
    if (u8_zero == 0)
    {
        for (i32_i = 0; i32_i < 16; i32_i++)
        {
            if (i32_i & 1)
            {
                u32_res ^= ((pu16_prom[i32_i >> 1]) & 0x00FF);
            }
            else
            {
                u32_res ^= (pu16_prom[i32_i >> 1] >> 8);
            }
            for (i32_j = 8; i32_j > 0; i32_j--)
            {
                if (res & 0x8000)
                {
                    u32_res ^= 0x1800;
                }
                u32_res <<= 1;
            }
        }
        pu16_prom[7] |= u8_crc;
        if (u8_crc == ((u32_res >> 12) & 0xF))
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


static uint32_t ms5611_read_adc(void)
{
    uint8_t u8_rxbuf[3];
    /* i2cRead(MS5611_ADDR, CMD_ADC_READ, 3, rxbuf); /* read ADC */
    be_result = board_i2c_read( MS5611_ADDR, CMD_ADC_READ, 3U, u8_rxbuf);
    return ((rxbuf[0] << 16U) | (rxbuf[1] << 8U) | rxbuf[2]);
}

static void ms5611_start_ut(void)
{
    /* i2cWrite(MS5611_ADDR, CMD_ADC_CONV + CMD_ADC_D2 + ms5611_osr, 1); /* D2 (temperature) conversion start! */
    be_result = board_i2c_write(MS5611_ADDR, CMD_ADC_CONV + CMD_ADC_D2 + ms5611_osr, 1);
}

static void ms5611_get_ut(void)
{
    ms5611_ut = ms5611_read_adc();
}

static void ms5611_start_up(void)
{
    /* i2cWrite(MS5611_ADDR, CMD_ADC_CONV + CMD_ADC_D1 + ms5611_osr, 1); /* D1 (pressure) conversion start! */
    be_result = board_i2c_write(MS5611_ADDR, CMD_ADC_CONV + CMD_ADC_D1 + ms5611_osr, 1);
}


static void ms5611_get_up(void)
{
    ms5611_up = ms5611_read_adc();
}


static void ms5611_calculate(int32_t *pressure, int32_t *temperature)
{
    uint32_t press;
    int64_t temp;
    int64_t delt;
    int32_t dT = (int64_t)ms5611_ut - ((uint64_t)ms5611_c[5] * 256);
    int64_t off = ((int64_t)ms5611_c[2] << 16) + (((int64_t)ms5611_c[4] * dT) >> 7);
    int64_t sens = ((int64_t)ms5611_c[1] << 15) + (((int64_t)ms5611_c[3] * dT) >> 8);
    temp = 2000 + ((dT * (int64_t)ms5611_c[6]) >> 23);

    if (temp < 2000)
    { /* temperature lower than 20degC */
        delt = temp - 2000;
        delt = 5 * delt * delt;
        off -= delt >> 1;
        sens -= delt >> 2;
        if (temp < -1500)
        { /* temperature lower than -15degC */
            delt = temp + 1500;
            delt = delt * delt;
            off  -= 7 * delt;
            sens -= (11 * delt) >> 1;
        }
    }

    press = ((((int64_t)ms5611_up * sens ) >> 21) - off) >> 15;

    if (pressure)
    {
        *pressure = press;
    }
    if (temperature)
    {
        *temperature = temp;
    }
}