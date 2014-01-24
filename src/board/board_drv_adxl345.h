#ifndef BOARD_DRV_ADXL345_H
#define BOARD_DRV_ADXL345_H 1

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_i2c.h"


/* Address */
#define USE_ACCEL_RT_BIAS   1
/* #define ADXL345_ADDRESS 0x53U*/
#define ADXL345_ADDRESS 0xA7U
/* Registers */

#define ADXL345_OFSX        0x1EU
#define ADXL345_OFSY        0x1FU
#define ADXL345_OFSZ        0x20U
#define ADXL345_BW_RATE     0x2CU
#define ADXL345_POWER_CTL   0x2DU
#define ADXL345_DATA_FORMAT 0x31U
#define ADXL345_DATAX0      0x32U

/* Bits */

#define MEASURE             0x08U

#define FULL_RES            0x08U
#define RANGE_4_G           0x01U

#define DATA_RATE_1600      0x0EU











/*
extern uint8_t accelCalibrating;

extern BOARD_FLOAT_3X_DATA accelRTBias;

extern BOARD_I32_3X_DATA accelSum100Hz;

extern BOARD_I32_3X_DATA accelSum200Hz;

extern BOARD_I32_3X_DATA accelSummedSamples100Hz;

extern BOARD_I32_3X_DATA accelSummedSamples200Hz;

extern BOARD_I16_3X_DATA rawAccel;
*/



BOARD_ERROR  board_drv_adxl345_read(void);

static void computeAccelRTBias(void);

BOARD_ERROR board_drv_adxl345_init(void);

#endif