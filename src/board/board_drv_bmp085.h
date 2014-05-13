#ifndef BOARD_DRV_BMP085_H
#define BOARD_DRV_BMP085_H 1

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_i2c.h"

#define BMP085_ADDRESS                          0xEEU
#define I_AM_BMP085                             0x55U

/* Register definition */
#define BMP085_PROM_START_ADDR                  0xAAU
#define BMP085_PROM_DATA_LEN                    22U

#define BMP085_CHIP_ID_REG                      0xD0U
#define BMP085_VERSION_REG                      0xD1U

#define BMP085_CTRL_REG                         0xF4U
#define BMP085_ADC_OUT_MSB_REG                  0xF6U
#define BMP085_ADC_OUT_LSB_REG                  0xF7U

#define BMP085_SOFT_RESET_REG                   0xE0U

#define BMP085_START_TEMPERATURE_MEASURE        0x2EU       /* temperature measurent */
#define BMP085_START_PRESSURE_MEASURE           0x34U       /* pressure measurement */

/* Sensor mode. */
#define BMP085_OVERSAMPLING                     0x03U
#define BMP085_OVERSAMPLING_POW                 (0x0001U << BMP085_OVERSAMPLING)
#define BMP085_TEMP_CONVERSION_TIME             5U        /* TO be spec'd by GL or SB*/
#define BMP085_PRESS_CONVERSION_TIME            28U

/* Calibration parameters. */
#define SMD500_PARAM_MG                         3038
#define SMD500_PARAM_MH                        -7357
#define SMD500_PARAM_MI                         3791

typedef enum
{
    START_TEMP_CONVERSION       = 0U, /* Write start temperature command to control register. */
    WR_READ_UNCOMPENSATED_TEMP  = 1U, /* Write read address. */
    RD_READ_UNCOMPENSATED_TEMP  = 2U, /* Read data from setted address. */
    START_PRESS_CONVERSION      = 3U, /* Write start pressure command to control register. */
    WR_READ_UNCOMPENSATED_PRESS = 4U, /* Write read address. */
    RD_READ_UNCOMPENSATED_PRESS = 5U, /* Write read address. */
    WAIT_FOR_DATA_READY         = 6U,
    CALCULATION                 = 7U
} BMP85_STATE_CONDITION;

typedef enum
{
    ONE_TIME    = 0U,
    AUTO        = 1U
} BMP85_I2C_ACCESS_CONDITION;




typedef struct
{
  /* sensor registers from the BOSCH BMP085 datasheet */
  int16_t               i16_ac1;
  int16_t               i16_ac2;
  int16_t               i16_ac3;
  uint16_t              u16_ac4;
  uint16_t              u16_ac5;
  uint16_t              u16_ac6;
  int16_t               i16_b1;
  int16_t               i16_b2;
  int32_t               i32_b5;
  int16_t               i16_mb;
  int16_t               i16_mc;
  int16_t               i16_md;
  /* Temperature part. */
  int16_t               i16_real_temperature;
  uint16_t              u16_raw_temperature;
  uint8_t               u8_raw_temperature[2];
  /* Pressure part */
  uint32_t              u32_real_pressure;
  uint32_t              u32_raw_pressure;
  uint8_t               u8_raw_pressure[4];

  /* System part. */
  BMP85_STATE_CONDITION bsc_state;
  uint64_t              u64_deadline;

} BMP85_STATE_STRUCTURE;

extern BMP85_STATE_STRUCTURE  bss_bmp85_state;




BOARD_ERROR be_board_drv_bmp085_init(void);
uint32_t   u32_board_drv_bmp085_get_pressure(void);
static BOARD_ERROR be_board_drv_bmp085_callibration_read(void);
BOARD_ERROR be_board_drv_bmp085_raw_temperature_start_read(BMP85_I2C_ACCESS_CONDITION  b85ac_mode);
static BOARD_ERROR be_board_drv_bmp085_read_raw_temperature(BMP85_I2C_ACCESS_CONDITION  b85ac_mode);
BOARD_ERROR be_board_drv_bmp085_raw_pressure_start_read(BMP85_I2C_ACCESS_CONDITION  b85ac_mode);
static BOARD_ERROR be_board_drv_bmp085_read_raw_pressure(BMP85_I2C_ACCESS_CONDITION  b85ac_mode);
BOARD_ERROR be_board_drv_bmp085_real_data_calculation(void);
void v_board_drv_bmp085_set_state(BMP85_STATE_CONDITION b85sc_state);
BMP85_STATE_CONDITION b85sc_board_drv_bmp085_get_state(void);
BOARD_ERROR be_board_drv_bmp085_write_data_address(BMP85_STATE_CONDITION  b85sc_state);
BOARD_ERROR be_board_drv_bmp085_read_data_from_address(uint8_t* pu8_buffer,uint16_t u16_number_byte_to_read, BMP85_STATE_CONDITION  b85sc_state);



#endif