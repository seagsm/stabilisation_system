#ifndef BOARD_DRV_ADXL345_H
#define BOARD_DRV_ADXL345_H 1

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_i2c.h"


extern uint8_t accelCalibrating;

extern BOARD_FLOAT_3X_DATA accelRTBias;

extern BOARD_I32_3X_DATA accelSum100Hz;

extern BOARD_I32_3X_DATA accelSum200Hz;

extern BOARD_I32_3X_DATA accelSummedSamples100Hz;

extern BOARD_I32_3X_DATA accelSummedSamples200Hz;

extern BOARD_I16_3X_DATA rawAccel;


void readAccel(void);

void computeAccelRTBias(void);

void initAccel(void);

#endif