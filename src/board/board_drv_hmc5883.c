

#include "board_drv_hmc5883.h"

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



float magScaleFactor[3];

int16_t magSum[3] = { 0, 0, 0 };

uint8_t newMagData = 0U;

int16_t rawMag[3];

void readMag(void)
{

    uint8_t u8_buffer[6];

    board_i2c_read(HMC5883_ADDRESS, HMC5883_DATA_X_MSB_REG, 6U, u8_buffer);

    /*  TODO: Should be solved.
    rawMag[XAXIS].bytes[1] = I2C2_Buffer_Rx[0];
    rawMag[XAXIS].bytes[0] = I2C2_Buffer_Rx[1];
    rawMag[ZAXIS].bytes[1] = I2C2_Buffer_Rx[2];
    rawMag[ZAXIS].bytes[0] = I2C2_Buffer_Rx[3];
    rawMag[YAXIS].bytes[1] = I2C2_Buffer_Rx[4];
    rawMag[YAXIS].bytes[0] = I2C2_Buffer_Rx[5];
*/
}

void initMag(void)
{
    /*
       uint8_t I2C2_Buffer_Rx[1] = { 0 };

       i2cWrite( HMC5883_ADDRESS, HMC5883_CONFIG_REG_B, SENSOR_GAIN );
       delay(20);

       i2cWrite( HMC5883_ADDRESS, HMC5883_CONFIG_REG_A, SENSOR_CONFIG | POSITIVE_BIAS_CONFIGURATION );
       delay(20);

       i2cWrite( HMC5883_ADDRESS, HMC5883_MODE_REG, OP_MODE_SINGLE );

       while ( (I2C2_Buffer_Rx[0] & STATUS_RDY) == 0x00 )
       i2cRead ( HMC5883_ADDRESS, HMC5883_STATUS_REG, 1, I2C2_Buffer_Rx );

       readMag();
     */
    /*TODO: should be solved. */
/*    magScaleFactor[XAXIS] = 1.0f;*/       /* (1.16F * 1090.0F) / (float)rawMag[XAXIS].value; */
/*    magScaleFactor[YAXIS] = 1.0f;*/       /* (1.16F * 1090.0F) / (float)rawMag[YAXIS].value; */
/*    magScaleFactor[ZAXIS] = 1.0f;*/       /* (1.08F * 1090.0F) / (float)rawMag[ZAXIS].value; */

    board_i2c_write(HMC5883_ADDRESS, HMC5883_CONFIG_REG_A, SENSOR_CONFIG | NORMAL_MEASUREMENT_CONFIGURATION);
    gv_board_sys_tick_fast_delay(20U);

    board_i2c_write(HMC5883_ADDRESS, HMC5883_MODE_REG, OP_MODE_CONTINUOUS);
    gv_board_sys_tick_fast_delay(20U);

    readMag();

    gv_board_sys_tick_fast_delay(20U);
}
