


#include "board_drv_bmp085.h"

static BMP85_STATE_STRUCTURE  bss_bmp85_state;

BOARD_ERROR be_board_drv_bmp085_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    gv_board_sys_tick_delay(10U);


    be_result = be_board_drv_bmp085_callibration_read();


/*
    i2c_BMP085_UT_Start();

    gv_board_sys_tick_delay(5U);

    i2c_BMP085_UT_Read();


*/
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

return(be_result);
}

/* Function send command to sensor to start temperature measurement. */
BOARD_ERROR be_board_drv_bmp085_raw_temperature_start_read(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Write to control register " start of temperature convertion" command. */
    be_result = board_i2c_write(BMP085_ADDRESS, BMP085_CTRL_REG, BMP085_START_TEMPERATURE_MEASURE);

    /* Save to sensor structure estimated time of the end of temperature conversion. */
    bss_bmp85_state.u64_deadline = gu64_read_system_time() + BMP085_TEMP_CONVERSION_TIME;

    /* Next step should be reading temperature value from baro sensor. */
    bss_bmp85_state.bsc_state = READ_UNCOMPENSATED_TEMP;

    return(be_result);
}

/* Function read result of temperature conversion from sensor. */
BOARD_ERROR be_board_drv_bmp085_read_raw_temperature(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Read 2 bytes of raw temperature from ADC. Should be called 5ms later after start temperature conversion. */
    be_result = board_i2c_read( BMP085_ADDRESS, BMP085_ADC_OUT_MSB_REG, 2U, bss_bmp85_state.u8_raw_temperature);

    /* Convert value to uint16. Can be used pointers conversion */
    bss_bmp85_state.u16_raw_temperature = ((uint16_t)bss_bmp85_state.u8_raw_temperature[0] << 8) + (( uint16_t)bss_bmp85_state.u8_raw_temperature[1]);

    /* Next step it to read raw pressure value. */
    bss_bmp85_state.bsc_state = START_PRESS_CONVERSION;

    return(be_result);
}

/* Function send command to sensor to start pressure measurement. */
BOARD_ERROR be_board_drv_bmp085_raw_pressure_start_read(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Write to control register " start of temperature convertion" command. */
    be_result = board_i2c_write(BMP085_ADDRESS, BMP085_CTRL_REG, BMP085_START_TEMPERATURE_MEASURE + (BMP085_OVERSAMPLING << 6));

    /* Save to sensor structure estimated time of the end of pressure conversion. */
    bss_bmp85_state.u64_deadline = gu64_read_system_time() + BMP085_TEMP_CONVERSION_TIME;

    /* Next step should be reading pressure value from baro sensor. */
    bss_bmp85_state.bsc_state = READ_UNCOMPENSATED_PRESS;

    return(be_result);
}

/* Function read result of pressure conversion from sensor. */
BOARD_ERROR be_board_drv_bmp085_read_raw_pressure(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint32_t u32_tmp[3];

    /* Read 3 bytes from sensor. */
    be_result = board_i2c_read( BMP085_ADDRESS, BMP085_ADC_OUT_MSB_REG, 3U, bss_bmp85_state.u8_raw_pressure);

    u32_tmp[0] = (uint32_t)bss_bmp85_state.u8_raw_pressure[0];
    u32_tmp[1] = (uint32_t)bss_bmp85_state.u8_raw_pressure[1];
    u32_tmp[2] = (uint32_t)bss_bmp85_state.u8_raw_pressure[2];

    /* Convert byte array to uint32_t. */
    bss_bmp85_state.u32_raw_pressure = (u32_tmp[0] << 16) + (u32_tmp[1] << 8) + u32_tmp[2];

    /* Next step should be calculated real temperature and real pressure. */
    bss_bmp85_state.bsc_state = CALCULATION;

    return(be_result);
}


BOARD_ERROR be_board_drv_bmp085_real_data_calculation(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    int32_t i32_x1, i32_x2, i32_x3, i32_b3, i32_b6, i32_tmp ;
    uint32_t u32_b4, u32_b7 , u32_tmp;

    if(bss_bmp85_state.bsc_state == CALCULATION)
    {
        /* Calculation of the real temperature. */
        i32_x1 = (( (int32_t) bss_bmp85_state.u16_raw_temperature  - (int32_t) bss_bmp85_state.u16_ac6) * (int32_t) bss_bmp85_state.u16_ac5)/32768;
        i32_x2 = ((int32_t) bss_bmp85_state.i16_mc * 2048) / (i32_x1 + bss_bmp85_state.i16_md);
        bss_bmp85_state.i32_b5 = i32_x1 + i32_x2;

        /* Save temperature in 0.1°C */
        bss_bmp85_state.i16_real_temperature = (int16_t)(bss_bmp85_state.i32_b5 + 8) / 16;

        /* Calculatin of the real pressure. */

        /* calculate B6 */
        i32_b6 = bss_bmp85_state.i32_b5 - 4000;

        /* calculate B3 */
        i32_x1 = (i32_b6 * i32_b6)/4096;
        i32_x1 *= bss_bmp85_state.i16_b2;
        i32_x1 = i32_x1 / 2048;
        i32_x2 = (bss_bmp85_state.i16_ac2 * i32_b6);
        i32_x2 = i32_x2 / 2048;
        i32_x3 = i32_x1 + i32_x2;
        i32_b3 = (((((int32_t)bss_bmp85_state.i16_ac1) * 4 + i32_x3) * (int32_t)BMP085_OVERSAMPLING_POW) + 2) / 4;

        /* calculate B4 */
        i32_x1 = (bss_bmp85_state.i16_ac3 * i32_b6) / 8192;
        i32_x2 = (bss_bmp85_state.i16_b1 * ((i32_b6 * i32_b6) / 4096) ) / 65636;
        i32_x3 = ((i32_x1 + i32_x2) + 2) / 4;
        i32_x3 = i32_x3 + 32768;
        u32_b4 = ((uint32_t)bss_bmp85_state.u16_ac4 * (uint32_t)i32_x3) / 32768U;

        /* calculate B7 */
        /*     u32_b7 = ((uint32_t)((int32_t)bss_bmp85_state.u32_raw_pressure - i32_b3) * (50000 / BMP085_OVERSAMPLING_POW)); */
        i32_tmp = (int32_t)bss_bmp85_state.u32_raw_pressure;
        i32_tmp = i32_tmp - i32_b3;
        u32_b7 = ((uint32_t)i32_tmp) * (50000U / BMP085_OVERSAMPLING_POW);

        if (u32_b7 < 0x80000000)
        {
            bss_bmp85_state.u32_real_pressure = (u32_b7 << 1) / u32_b4;
        }
        else
        {
            bss_bmp85_state.u32_real_pressure = (u32_b7 / u32_b4) << 1;
        }
        u32_tmp = bss_bmp85_state.u32_real_pressure / 256U;
        i32_x1 = (int32_t)u32_tmp;
        i32_x1 *= i32_x1;
        i32_x1 = (i32_x1 * SMD500_PARAM_MG) / 65536;
        i32_x2 = ((int32_t)bss_bmp85_state.u32_real_pressure * SMD500_PARAM_MH) / 65536;
        i32_tmp = (i32_x1 + i32_x2 + (int32_t)SMD500_PARAM_MI) / 16;

        /* Real_pressure in Pa*/
        bss_bmp85_state.u32_real_pressure += (uint32_t)i32_tmp;
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


