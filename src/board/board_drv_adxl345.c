
#include "board_drv_adxl345.h"





static volatile uint8_t u8_accelCalibrating = 0U;

BOARD_FLOAT_3X_DATA accelRTBias = { 0.0f, 0.0f, 0.0f };

/* Function compute accel Runtime Bias */
static void computeAccelRTBias(void)
{

#if USE_ACCEL_RT_BIAS_CALIBRATION
    uint16_t samples;
    BOARD_FLOAT_3X_DATA accelSum = { 0.0f, 0.0f, 0.0f};
    BOARD_I16_3X_DATA   rawAccel;
    uint8_t             u8_buffer[6] = {0U};
    uint16_t            u16_i = 0U;


    u8_accelCalibrating = 1U;

/* Finaly accel calibration looks useless because later accel data used normalised only. */
/* Accel calibration is ON. */
    for (samples = 0U; samples < 2000U; samples++ )
    {
        board_drv_adxl345_read(u8_buffer);

        u16_i = (((uint16_t)  u8_buffer[1]) << 8U) + ((uint16_t)u8_buffer[0]);
        rawAccel.i16_X = (int16_t)u16_i;

        u16_i = (((uint16_t)  u8_buffer[3]) << 8U) + ((uint16_t)u8_buffer[2]);
        rawAccel.i16_Y = (int16_t)u16_i;

        u16_i = (((uint16_t)  u8_buffer[5]) << 8U) + ((uint16_t)u8_buffer[4]);
        rawAccel.i16_Z = (int16_t)u16_i;

        accelSum.fl_X += (float)rawAccel.i16_X;
        accelSum.fl_Y += (float)rawAccel.i16_Y;
        accelSum.fl_Z += (float)rawAccel.i16_Z;

        gv_board_sys_tick_delay(1U);/* 1miliSec */
    }
    /* This calibration is for Z placed on vertical position only. */
    accelRTBias.fl_X =  accelSum.fl_X / 2000.0f;
    accelRTBias.fl_Y =  accelSum.fl_Y / 2000.0f;
    accelRTBias.fl_Z = (accelSum.fl_Z / 2000.0f) - 256.0f; /* 256 is 9.8 m/c^2 */
    u8_accelCalibrating = 0U;
#else
    accelRTBias.fl_X = 0.0f;
    accelRTBias.fl_Y = 0.0f;
    accelRTBias.fl_Z = 0.0f;
#endif
}

/* Function read accelerometer. */
BOARD_ERROR  board_drv_adxl345_read(uint8_t* pu8_buffer )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = board_i2c_read(ADXL345_ADDRESS, ADXL345_DATAX0, 6U, pu8_buffer);

    return (be_result);
}

/* Function do accel initialization. */
BOARD_ERROR  board_drv_adxl345_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result |= board_i2c_write(ADXL345_ADDRESS, ADXL345_POWER_CTL, MEASURE);

    gv_board_sys_tick_fast_delay(10U);

    be_result |= board_i2c_write(ADXL345_ADDRESS, ADXL345_DATA_FORMAT, FULL_RES | RANGE_16_G);/* RANGE_8_G */

    gv_board_sys_tick_fast_delay(10U);

    be_result |= board_i2c_write(ADXL345_ADDRESS, ADXL345_BW_RATE, DATA_RATE_100);/* DATA_RATE_400 */

    gv_board_sys_tick_fast_delay(100U);

    computeAccelRTBias();

    return (be_result);
}

