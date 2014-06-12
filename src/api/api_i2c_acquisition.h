#ifndef API_I2C_ACQUISITION_H
#define API_I2C_ACQUISITION_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_i2c.h"
#include "board_baro.h"
#include "board_drv_hmc5883.h"
#include "board_drv_l3g4200d.h"
#include "board_drv_adxl345.h"
#include "board_drv_bmp085.h"


/* data_acquisition chain. */

 /*
	0 - Gyro.
	1 - Accelerometer.
	2 - Magnetometer.
        3 - Barometer.
 */

#define MAX_DEV_NUM 4U
#define SENSOR_WRITE_SIZEOF 1U
#define SENSOR_READ_SIZEOF  6U

#define GYRO_DEVICE_ADDRESS             L3G4200D_ADDRESS
#define GYRO_READ_REGISTER              L3G4200D_GYRO_OUT
#define GYRO_WRITE_SIZE                 SENSOR_WRITE_SIZEOF
#define GYRO_READ_SIZE                  SENSOR_READ_SIZEOF

#define ACCELEROMETER_DEVICE_ADDRESS    ADXL345_ADDRESS
#define ACCELEROMETER_READ_REGISTER     ADXL345_DATAX0
#define ACCELEROMETER_WRITE_SIZE        SENSOR_WRITE_SIZEOF
#define ACCELEROMETER_READ_SIZE         SENSOR_READ_SIZEOF

#define MAGNETOMETER_DEVICE_ADDRESS     HMC5883_ADDRESS
#define MAGNETOMETER_READ_REGISTER      HMC5883_DATA_X_MSB_REG
#define MAGNETOMETER_WRITE_SIZE         SENSOR_WRITE_SIZEOF
#define MAGNETOMETER_READ_SIZE          SENSOR_READ_SIZEOF


typedef struct
{
  uint8_t data[SENSOR_READ_SIZEOF];
} BOARD_6BYTE_ARRAY;

typedef struct
{
  BOARD_6BYTE_ARRAY array[MAX_DEV_NUM];
  uint8_t u8_ready;
  uint8_t u8_device;
} BOARD_I2C_DATA;

extern BOARD_I2C_DATA api_i2c_data;

/* User function.*/
BOARD_ERROR be_api_i2c_acquisition_init(void);
BOARD_ERROR be_api_i2c_acquisition_start(void);

/* Service function. */
BOARD_ERROR be_api_i2c_read_process_start(void);
BOARD_ERROR be_api_i2c_write_process_start(void);











#endif