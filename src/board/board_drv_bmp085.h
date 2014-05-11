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
    START_TEMP_CONVERSION       = 0U,
    READ_UNCOMPENSATED_TEMP     = 1U,
    START_PRESS_CONVERSION      = 2U,
    READ_UNCOMPENSATED_PRESS    = 3U,
    CALCULATION                 = 4U,
    COMPUTE_ALTITUDE            = 5U
} BMP85_STATE_CONDITION;


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
/*
  union {uint16_t val; uint8_t raw[2]; } ut; //uncompensated T
  union {uint32_t val; uint8_t raw[4]; } up; //uncompensated P
*/
  int16_t               i16_real_temperature;
  uint16_t              u16_raw_temperature;
  uint8_t               u8_raw_temperature[2];

  uint32_t               u32_real_pressure;
  uint32_t              u32_raw_pressure;
  uint8_t               u8_raw_pressure[4];

  BMP85_STATE_CONDITION bsc_state;
  uint64_t              u64_deadline;
} BMP85_STATE_STRUCTURE;






BOARD_ERROR be_board_drv_bmp085_init(void);
uint32_t   u32_board_drv_bmp085_get_pressure(void);
static BOARD_ERROR be_board_drv_bmp085_callibration_read(void);
BOARD_ERROR be_board_drv_bmp085_raw_temperature_start_read(void);
BOARD_ERROR be_board_drv_bmp085_read_raw_temperature(void);
BOARD_ERROR be_board_drv_bmp085_raw_pressure_start_read(void);
BOARD_ERROR be_board_drv_bmp085_read_raw_pressure(void);
BOARD_ERROR be_board_drv_bmp085_real_data_calculation(void);








#endif