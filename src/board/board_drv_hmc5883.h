
#ifndef BOARD_DRV_HMC5883_H
#define BOARD_DRV_HMC5883_H 1

#include "stm32f10x.h"
#include "board_sys_tick.h"
#include "board_i2c.h"
#include "board_system_type.h"



extern float magScaleFactor[3];
extern int16_t magSum[3];
extern uint8_t newMagData;
extern int16_t rawMag[3];


void readMag(void);

void initMag(void);



#endif