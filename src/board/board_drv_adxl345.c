
#include "board_drv_adxl345.h"





static volatile uint8_t u8_accelCalibrating = 0U;

static BOARD_FLOAT_3X_DATA accelRTBias = { 0.0f, 0.0f, 0.0f };

BOARD_I32_3X_DATA accelSum100Hz = { 0, 0, 0 };

BOARD_I32_3X_DATA accelSum200Hz = { 0, 0, 0 };

BOARD_I32_3X_DATA accelSummedSamples100Hz;

BOARD_I32_3X_DATA accelSummedSamples200Hz;

static BOARD_I16_3X_DATA rawAccel;

static BOARD_FLOAT_3X_DATA accelScaleFactor={ 9.8065f / 256.0f, 9.8065f / 256.0f, 9.8065f / 256.0f};


/* Compute Accel Runtime Bias */
static void computeAccelRTBias(void)
{
    uint16_t samples;
    BOARD_FLOAT_3X_DATA accelSum = { 0.0f, 0.0f, 0.0f};

    u8_accelCalibrating = 1U;

    for (samples = 0U; samples < 2000U; samples++)
    {
        board_drv_adxl345_read();

        accelSum.fl_X += (float)rawAccel.i16_X;
        accelSum.fl_Y += (float)rawAccel.i16_Y;
        accelSum.fl_Z += (float)rawAccel.i16_Z;

        gv_board_sys_tick_delay(1U);/* 1miliSec */
    }

    accelRTBias.fl_X =  accelSum.fl_X / 2000.0f;
    accelRTBias.fl_Y =  accelSum.fl_Y / 2000.0f;
    accelRTBias.fl_Z = (accelSum.fl_Z / 2000.0f) - (9.8056f / (float)fabs((double)accelScaleFactor.fl_Z));

    u8_accelCalibrating = 0U;
}

/* Read Accelerometer. */
BOARD_ERROR  board_drv_adxl345_read(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    uint8_t u8_buffer[6];
    uint16_t u16_i;

                    /*board_i2c_read(ADXL345_ADDRESS, ADXL345_DATAX0, 6U, u8_buffer);*/
    be_result = board_i2c_read(ADXL345_ADDRESS, ADXL345_DATAX0, 6U, u8_buffer);
/*
    u16_i = (((uint16_t)u8_buffer[1]) << 8U) + ((uint16_t)u8_buffer[0]);
    rawAccel.i16_Y = (int16_t)u16_i;

    u16_i = (((uint16_t)u8_buffer[3]) << 8U) + ((uint16_t)u8_buffer[2]);
    rawAccel.i16_X = (int16_t)u16_i;

    u16_i = (((uint16_t)u8_buffer[5]) << 8U) + ((uint16_t)u8_buffer[4]);
    rawAccel.i16_Z = (int16_t)u16_i;
*/
    return (be_result);
}

/* Accel Initialization */
BOARD_ERROR  board_drv_adxl345_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result |= board_i2c_write(ADXL345_ADDRESS, ADXL345_POWER_CTL, MEASURE);

    gv_board_sys_tick_fast_delay(10U);

    be_result |= board_i2c_write(ADXL345_ADDRESS, ADXL345_DATA_FORMAT, FULL_RES | RANGE_4_G);

    gv_board_sys_tick_fast_delay(10U);

    be_result |= board_i2c_write(ADXL345_ADDRESS, ADXL345_BW_RATE, DATA_RATE_1600);

    gv_board_sys_tick_fast_delay(100U);

#if USE_ACCEL_RT_BIAS
    /* computeAccelRTBias(); */
#else
    accelRTBias.fl_X = 0.0f;
    accelRTBias.fl_Y = 0.0f;
    accelRTBias.fl_Z = 0.0f;
#endif

    return (be_result);
}

