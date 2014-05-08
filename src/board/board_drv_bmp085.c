


#include "board_drv_bmp085.h"

static BMP85_STATE  bs_bmp85_state;

BOARD_ERROR board_drv_bmp085_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    gv_board_sys_tick_delay(10U);
/*
    i2c_BMP085_readCalibration();
    i2c_BMP085_UT_Start();

    gv_board_sys_tick_delay(5U);

    i2c_BMP085_UT_Read();


*/
    return (be_result);
}

BOARD_ERROR board_drv_bmp085_callibration_read(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    uint8_t u8_callibration_data[BMP085_REG_SIZEOF];
    gv_board_sys_tick_delay(10U);

    /* Read calibration data from calibration registers. */
    be_result = board_i2c_read( BMP085_ADDRESS, BMP085_REG_START_ADDRESS, BMP085_REG_SIZEOF, &u8_callibration_data[0]);

    /* Setup data to registers structure. */
    bs_bmp85_state.i16_ac1 = (( int16_t)u8_callibration_data[0]  * 256) + (( int16_t)u8_callibration_data[1]);
    bs_bmp85_state.i16_ac2 = (( int16_t)u8_callibration_data[2]  * 256) + (( int16_t)u8_callibration_data[3]);
    bs_bmp85_state.i16_ac3 = (( int16_t)u8_callibration_data[4]  * 256) + (( int16_t)u8_callibration_data[5]);
    bs_bmp85_state.u16_ac4 = ((uint16_t)u8_callibration_data[6]  << 8 ) + ((uint16_t)u8_callibration_data[7]);
    bs_bmp85_state.u16_ac5 = ((uint16_t)u8_callibration_data[8]  << 8 ) + ((uint16_t)u8_callibration_data[9]);
    bs_bmp85_state.u16_ac6 = ((uint16_t)u8_callibration_data[10] << 8 ) + ((uint16_t)u8_callibration_data[11]);
    bs_bmp85_state.i16_b1  = (( int16_t)u8_callibration_data[12] * 256) + (( int16_t)u8_callibration_data[13]);
    bs_bmp85_state.i16_b2  = (( int16_t)u8_callibration_data[14] * 256) + (( int16_t)u8_callibration_data[15]);
    bs_bmp85_state.i16_mb  = (( int16_t)u8_callibration_data[16] * 256) + (( int16_t)u8_callibration_data[17]);
    bs_bmp85_state.i16_mc  = (( int16_t)u8_callibration_data[18] * 256) + (( int16_t)u8_callibration_data[19]);
    bs_bmp85_state.i16_md  = (( int16_t)u8_callibration_data[20] * 256) + (( int16_t)u8_callibration_data[21]);

    return(be_result);
}
























int32_t i32_board_drv_bmp085_calculate_pressure(void)
{
    int32_t i32_pressure = 0;

    return(i32_pressure);
}


