


#include "board_drv_bmp085.h"

    BMP85_STATE_STRUCTURE  bss_bmp85_state;

BOARD_ERROR be_board_drv_bmp085_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    gv_board_sys_tick_delay(10U);


    be_result = be_board_drv_bmp085_callibration_read();

    return (be_result);
}

/*
    If this array has size 8 bytes and less, it can be defined like local array  inside function.
    If I do size of that array more that 8 byte, it make troubles with DMA.
    If array defined like global, it work good. I did not found reason yet.
    I just defined it like global.
*/
static  uint8_t u8_callibration_data[BMP085_PROM_DATA_LEN] = {0U};

static BOARD_ERROR be_board_drv_bmp085_callibration_read(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    uint16_t u16_counter = 10U;

    gv_board_sys_tick_delay(10U);

    /* Read calibration data from calibration registers. */
    be_result = board_i2c_read( BMP085_ADDRESS, BMP085_PROM_START_ADDR, BMP085_PROM_DATA_LEN, u8_callibration_data);

    bss_bmp85_state.i16_ac1 = (( int16_t)u8_callibration_data[0]  * 256) + (( int16_t)u8_callibration_data[1]);
    bss_bmp85_state.i16_ac2 = (( int16_t)u8_callibration_data[2]  * 256) + (( int16_t)u8_callibration_data[3]);
    bss_bmp85_state.i16_ac3 = (( int16_t)u8_callibration_data[4]  * 256) + (( int16_t)u8_callibration_data[5]);
    bss_bmp85_state.u16_ac4 = ((uint16_t)u8_callibration_data[6]  << 8 ) + ((uint16_t)u8_callibration_data[7]);
    bss_bmp85_state.u16_ac5 = ((uint16_t)u8_callibration_data[8]  << 8 ) + ((uint16_t)u8_callibration_data[9]);
    bss_bmp85_state.u16_ac6 = ((uint16_t)u8_callibration_data[10] << 8 ) + ((uint16_t)u8_callibration_data[11]);
    bss_bmp85_state.i16_b1  = (( int16_t)u8_callibration_data[12] * 256) + (( int16_t)u8_callibration_data[13]);
    bss_bmp85_state.i16_b2  = (( int16_t)u8_callibration_data[14] * 256) + (( int16_t)u8_callibration_data[15]);
    bss_bmp85_state.i16_mb  = (( int16_t)u8_callibration_data[16] * 256) + (( int16_t)u8_callibration_data[17]);
    bss_bmp85_state.i16_mc  = (( int16_t)u8_callibration_data[18] * 256) + (( int16_t)u8_callibration_data[19]);
    bss_bmp85_state.i16_md  = (( int16_t)u8_callibration_data[20] * 256) + (( int16_t)u8_callibration_data[21]);

#if 1
    /* For test reading only. */
    while(u16_counter > 0U)
    {
        be_board_drv_bmp085_raw_temperature_start_read(ONE_TIME);
        gv_board_sys_tick_delay(5U);
        be_board_drv_bmp085_read_raw_temperature(ONE_TIME);
        be_board_drv_bmp085_raw_pressure_start_read(ONE_TIME);
        gv_board_sys_tick_delay(28U);
        be_board_drv_bmp085_read_raw_pressure(ONE_TIME);

        be_board_drv_bmp085_real_data_calculation();
        {
            float float_altitude;
            uint32_t u32_pressure;

            u32_pressure =  bss_bmp85_state.u32_real_pressure;

            /* altitude in centimeters*/
            float_altitude = (1.0f - (float)pow((float)u32_pressure/101325.0, 0.190295)) * 4433000.0f;

        }

        u16_counter--;
    }
#endif


return(be_result);
}

/* Function send command to sensor to start temperature measurement. */
BOARD_ERROR be_board_drv_bmp085_raw_temperature_start_read(BMP85_I2C_ACCESS_CONDITION  b85ac_mode)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    uint8_t u8_write_buffer[2U]= { 0U, 0U};

    if(b85ac_mode == ONE_TIME)
    {
        /* Write to control register " start of temperature convertion" command in ONE_TIME mode. */
        be_result = board_i2c_write(BMP085_ADDRESS, BMP085_CTRL_REG, BMP085_START_TEMPERATURE_MEASURE);
    }
    else
    {
        /* Write to control register " start of temperature convertion" command in AUTO mode. */
        u8_write_buffer[0U] = BMP085_CTRL_REG;                  /* Address of device register. */
        u8_write_buffer[1U] = BMP085_START_TEMPERATURE_MEASURE; /* Data for writing to device register. */
        be_result = be_board_i2c_write_start(u8_write_buffer, 2U, BMP085_ADDRESS);
    }

    /* Save to sensor structure estimated time of the end of temperature conversion. */
    bss_bmp85_state.u64_deadline = gu64_read_system_time() + BMP085_TEMP_CONVERSION_TIME;

    /* Next step should be reading temperature value from baro sensor. */
    bss_bmp85_state.bsc_state = WR_READ_UNCOMPENSATED_TEMP;
    return(be_result);
}







/* Function send start address for reading. */
BOARD_ERROR be_board_drv_bmp085_write_data_address(BMP85_STATE_CONDITION  b85sc_state)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    uint8_t u8_data = BMP085_ADC_OUT_MSB_REG;

    be_result = be_board_i2c_write_start(&u8_data, 1U, BMP085_ADDRESS);
     /* Next step should be reading temperature value from baro sensor. */
    bss_bmp85_state.bsc_state = b85sc_state;
    return(be_result);
}

/* Function read data from baro sensor. */
BOARD_ERROR be_board_drv_bmp085_read_data_from_address(uint8_t* pu8_buffer,uint16_t u16_number_byte_to_read, BMP85_STATE_CONDITION  b85sc_state)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_board_i2c_read_start(pu8_buffer, u16_number_byte_to_read, BMP085_ADDRESS);
     /* Next step should be reading temperature value from baro sensor. */
    bss_bmp85_state.bsc_state = b85sc_state;
    return(be_result);
}








/* Function read result of temperature conversion from sensor. */
static BOARD_ERROR be_board_drv_bmp085_read_raw_temperature(BMP85_I2C_ACCESS_CONDITION  b85ac_mode)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    if(b85ac_mode == ONE_TIME)
    {
        /* Read 2 bytes of raw temperature from ADC. Should be called 5ms later after start temperature conversion. */
        be_result = board_i2c_read( BMP085_ADDRESS, BMP085_ADC_OUT_MSB_REG, 2U, bss_bmp85_state.u8_raw_temperature);
    }
    else
    {

            /* Call read-init function. */
       /* be_result = be_board_i2c_read_start(pu8_buffer, u16_num_byte_to_read, u8_device_address);*/

    }


    /* Convert value to uint16. Can be used pointers conversion */
    bss_bmp85_state.u16_raw_temperature = ((uint16_t)bss_bmp85_state.u8_raw_temperature[0] << 8) + (( uint16_t)bss_bmp85_state.u8_raw_temperature[1]);

    /* Next step it to read raw pressure value. */
    bss_bmp85_state.bsc_state = START_PRESS_CONVERSION;

    return(be_result);
}

/* Function send command to sensor to start pressure measurement. */
BOARD_ERROR be_board_drv_bmp085_raw_pressure_start_read(BMP85_I2C_ACCESS_CONDITION  b85ac_mode)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t u8_write_buffer[2U]= { 0U, 0U};

    if(b85ac_mode == ONE_TIME)
    {
        /* Write to control register " start of pressure convertion" command. */
        be_result = board_i2c_write(BMP085_ADDRESS, BMP085_CTRL_REG, BMP085_START_PRESSURE_MEASURE + (BMP085_OVERSAMPLING << 6));
    }
    else
    {
        /* Write to control register " start of temperature convertion" command in AUTO mode. */
        u8_write_buffer[0U] = BMP085_CTRL_REG;                  /* Address of device register. */
        u8_write_buffer[1U] = BMP085_START_PRESSURE_MEASURE + (BMP085_OVERSAMPLING << 6); /* Data for writing to device register. */
        be_result = be_board_i2c_write_start(u8_write_buffer, 2U, BMP085_ADDRESS);
    }
    /* Save to sensor structure estimated time of the end of pressure conversion. */
    bss_bmp85_state.u64_deadline = gu64_read_system_time() + BMP085_PRESS_CONVERSION_TIME;

    /* Next step should be reading pressure value from baro sensor. */
    bss_bmp85_state.bsc_state = WR_READ_UNCOMPENSATED_PRESS;

    return(be_result);
}

/* Function read result of pressure conversion from sensor. */
static BOARD_ERROR be_board_drv_bmp085_read_raw_pressure(BMP85_I2C_ACCESS_CONDITION  b85ac_mode)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint32_t u32_tmp[3];

    if(b85ac_mode == ONE_TIME)
    {
        /* Read 3 bytes from sensor. */
        be_result = board_i2c_read( BMP085_ADDRESS, BMP085_ADC_OUT_MSB_REG, 3U, bss_bmp85_state.u8_raw_pressure);
    }


    u32_tmp[0] = (uint32_t)bss_bmp85_state.u8_raw_pressure[0];
    u32_tmp[1] = (uint32_t)bss_bmp85_state.u8_raw_pressure[1];
    u32_tmp[2] = (uint32_t)bss_bmp85_state.u8_raw_pressure[2];

    /* Convert byte array to uint32_t. */
    bss_bmp85_state.u32_raw_pressure = ((u32_tmp[0] << 16) | (u32_tmp[1] << 8) | u32_tmp[2]) ;
    bss_bmp85_state.u32_raw_pressure = bss_bmp85_state.u32_raw_pressure >> (8 - (int8_t)BMP085_OVERSAMPLING);


    /* Next step should be calculated real temperature and real pressure. */
    bss_bmp85_state.bsc_state = CALCULATION;

    return(be_result);
}


BOARD_ERROR be_board_drv_bmp085_real_data_calculation(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    int32_t i32_x1, i32_x2, i32_x3, i32_b3, i32_b6, i32_tmp ;
    uint32_t u32_b4, u32_b7 , u32_tmp, u32_pr_tmp;

    if(bss_bmp85_state.bsc_state == CALCULATION)
    {
        /* Calculation of the real temperature. */
        i32_x1 = (( (int32_t) bss_bmp85_state.u16_raw_temperature  - (int32_t) bss_bmp85_state.u16_ac6) * (int32_t) bss_bmp85_state.u16_ac5)/32768;
        i32_x2 = ((int32_t) bss_bmp85_state.i16_mc * 2048) / (i32_x1 + bss_bmp85_state.i16_md);
        bss_bmp85_state.i32_b5 = i32_x1 + i32_x2;

        /* Save temperature in 0.1�C */
        bss_bmp85_state.i16_real_temperature = (int16_t)(bss_bmp85_state.i32_b5 + 8) / 16;

        /* Calculatin of the real pressure. */

        /* calculate B6 */
        i32_b6 = bss_bmp85_state.i32_b5 - 4000;
/*OK*/
        /* calculate B3 */
        i32_x1 = (i32_b6 * i32_b6)/4096;
        i32_x1 = bss_bmp85_state.i16_b2 * i32_x1;
        i32_x1 = i32_x1 / 2048;
/*OK*/
        i32_x2 = (bss_bmp85_state.i16_ac2 * i32_b6);
        i32_x2 = i32_x2 / 2048;
/*OK*/
        i32_x3 = i32_x1 + i32_x2;
/*OK*/
        i32_tmp = bss_bmp85_state.i16_ac1;
        i32_tmp = (i32_tmp * 4) + i32_x3;
        i32_tmp = i32_tmp * (int32_t)BMP085_OVERSAMPLING_POW;
        i32_b3  = (i32_tmp + 2) / 4;
/*OK*/
        /* calculate B4 */
        i32_x1 = (bss_bmp85_state.i16_ac3 * i32_b6) / 8192;
/*OK*/
        i32_x2 = (bss_bmp85_state.i16_b1 * ((i32_b6 * i32_b6) / 4096) ) / 65636;
/*OK*/
        i32_x3 = ((i32_x1 + i32_x2) + 2) / 4;
/*OK*/
        i32_x3 = i32_x3 + 32768;

        u32_b4 = ((uint32_t)bss_bmp85_state.u16_ac4 * (uint32_t)i32_x3) / 32768U;
/*OK*/
        /* calculate B7 */
        /*     u32_b7 = ((uint32_t)((int32_t)bss_bmp85_state.u32_raw_pressure - i32_b3) * (50000 / BMP085_OVERSAMPLING_POW)); */
        i32_tmp = (int32_t)bss_bmp85_state.u32_raw_pressure;
        i32_tmp = i32_tmp - i32_b3;
        u32_b7 = ((uint32_t)i32_tmp) * (50000U / BMP085_OVERSAMPLING_POW);
/*OK*/
        if (u32_b7 < 0x80000000)
        {
            /* bss_bmp85_state.u32_real_pressure = (u32_b7 << 1) / u32_b4; */
            u32_pr_tmp = (u32_b7 << 1) / u32_b4;
        }
        else
        {
            /* bss_bmp85_state.u32_real_pressure = (u32_b7 / u32_b4) << 1; */
            u32_pr_tmp = (u32_b7 / u32_b4) << 1;

        }
/*OK*/
        /* u32_tmp = bss_bmp85_state.u32_real_pressure / 256U; */
        u32_tmp = u32_pr_tmp / 256U;

        i32_x1 = (int32_t)u32_tmp;
        i32_x1 = i32_x1 * i32_x1;
        i32_x1 = (i32_x1 * SMD500_PARAM_MG) / 65536; /* SMD500_PARAM_MG = 3038. */
/*OK*/
        /* i32_x2 = ((int32_t)bss_bmp85_state.u32_real_pressure * SMD500_PARAM_MH) / 65536; */
        i32_x2 = ((int32_t)u32_pr_tmp * SMD500_PARAM_MH) / 65536;    /* SMD500_PARAM_MH -7357 */
        i32_tmp = (i32_x1 + i32_x2 + (int32_t)SMD500_PARAM_MI) / 16; /* SMD500_PARAM_MI  3791 */

        /* Real_pressure in Pa*/
        /* bss_bmp85_state.u32_real_pressure += (uint32_t)i32_tmp; */
        bss_bmp85_state.u32_real_pressure = u32_pr_tmp + (uint32_t)i32_tmp;
/*OK*/
    }
    else
    {
        be_result = BOARD_ERR_STATE;
    }
    return(be_result);
}


uint32_t u32_board_drv_bmp085_get_pressure(void)
{
    return(bss_bmp85_state.u32_real_pressure);
}

BMP85_STATE_CONDITION b85sc_board_drv_bmp085_get_state(void)
{
    return(bss_bmp85_state.bsc_state);
}

void v_board_drv_bmp085_set_state(BMP85_STATE_CONDITION b85sc_state)
{
    bss_bmp85_state.bsc_state = b85sc_state;
}







