
#ifndef BOARD_DRV_HMC5883_H
#define BOARD_DRV_HMC5883_H 1

#include "stm32f10x.h"
#include "board_sys_tick.h"
#include "board_i2c.h"
#include "board_system_type.h"


/* 7 bit address. */
/*#define HMC5883_ADDRESS 0x1E*/
/* Real 8 bit address. */
#define HMC5883_ADDRESS 0x3DU

#define HMC5883_CONFIG_REG_A    0x00U
#define HMC5883_CONFIG_REG_B    0x01U
#define HMC5883_MODE_REG        0x02U
#define HMC5883_DATA_X_MSB_REG  0x03U
#define HMC5883_STATUS_REG      0x09U


/* #define SENSOR_CONFIG 0x18U  */   /* 1 Sample average, 75 Hz */
/* #define SENSOR_CONFIG 0x38U  */   /* 2 Sample average, 75 Hz */
/* #define SENSOR_CONFIG 0x58U  */   /* 4 Sample average, 75 Hz */
#define SENSOR_CONFIG 0x78U          /* 8 Sample average, 75 Hz */

#define NORMAL_MEASUREMENT_CONFIGURATION 0x00U
#define POSITIVE_BIAS_CONFIGURATION      0x01U


/* #define SENSOR_GAIN 0x00U  */ /* +/- 0.88 Ga */
#define SENSOR_GAIN 0x20U        /* +/- 1.3  Ga (default) */
/* #define SENSOR_GAIN 0x40U  */ /* +/- 1.9  Ga */
/* #define SENSOR_GAIN 0x60U  */ /* +/- 2.5  Ga */
/* #define SENSOR_GAIN 0x80U  */ /* +/- 4.0  Ga */
/* #define SENSOR_GAIN 0xA0U  */ /* +/- 4.7  Ga */
/* #define SENSOR_GAIN 0xC0U  */ /* +/- 5.6  Ga */
/* #define SENSOR_GAIN 0xE0U  */ /* +/- 8.1  Ga */



#define OP_MODE_CONTINUOUS 0x00U /* Continuous conversion */
#define OP_MODE_SINGLE     0x01U /* Single converison */

#define STATUS_RDY         0x01U /* Data Ready */


extern BOARD_FLOAT_3X_DATA magScaleFactor;
extern BOARD_U16_3X_DATA rawMag;
extern int16_t magSum[3];
extern uint8_t newMagData;


BOARD_ERROR be_board_drv_hmc5883_init(void);
BOARD_ERROR be_board_drv_hmc5883_read(void);

#endif