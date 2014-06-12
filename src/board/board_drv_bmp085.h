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

/*
    This enum reflect algorithm of reading data from BARO sensor.
Current state :
    START_TEMP_CONVERSION               -> Function start writing a "start TEMP conversion" command to sensor.
                                           Setting next state - START_READING_UNCOMPENSATED_TEMP.
                                           And setting deadline time for WAITING 5mS.
                                           Cause DMA WRITE end interrupt.

        When deadline come:
    START_READING_UNCOMPENSATED_TEMP    -> Function start writing a read address of data to sensor.
                                           Setting next state - READ_UNCOMPENSATED_TEMP.
                                           Cause DMA WRITE end interrupt.

    READ_UNCOMPENSATED_TEMP             -> Function start reading a raw temperature data from sensor.
                                           Setting next state - WAIT_FOR_TEMP_DATA_READY.
                                           Cause DMA READ end interrupt.

    WAIT_FOR_TEMP_DATA_READY            -> Function copy raw temperature data to u16_temperature.
                                           Setting next state - START_PRESS_CONVERSION.

    START_PRESS_CONVERSION              -> Function start writing a "start PRESS conversion" command to sensor.
                                           Setting next state -     START_READING_UNCOMPENSATED_PRESS.
                                           And setting deadline time for WAITING 26mS.
                                           Cause DMA WRITE end interrupt.

        When deadline come:
    START_READING_UNCOMPENSATED_PRESS   -> Function start writing a read address of data to sensor.
                                           Setting next state - READ_UNCOMPENSATED_PRESS.
                                           Cause DMA WRITE end interrupt.


    READ_UNCOMPENSATED_PRESS            -> Function start reading a raw PRESSURE data from sensor.
                                           Setting next state - WAIT_FOR_PRESS_DATA_READY.
                                           Cause DMA READ end interrupt.

    WAIT_FOR_PRESS_DATA_READY           -> Function copy raw temperature data to u32_pressure.
                                           Setting next state - CALCULATION .

    CALCULATION                          End of script. In this state data ready for calculation.
*/

typedef enum
{
    START_TEMP_CONVERSION               = 0U, /* Write start temperature command to control register. */
    START_READING_UNCOMPENSATED_TEMP    = 1U, /* Write read address. */
    READ_UNCOMPENSATED_TEMP             = 2U, /* Read data from setted address. */
    WAIT_FOR_TEMP_DATA_READY            = 3U, /* Wait for temperature data read done. */
    START_PRESS_CONVERSION              = 4U, /* Write start pressure command to control register. */
    START_READING_UNCOMPENSATED_PRESS   = 5U, /* Write read address. */
    READ_UNCOMPENSATED_PRESS            = 6U, /* Write read address. */
    WAIT_FOR_PRESS_DATA_READY           = 7U, /* Wait for pressure data read done. */
    CALCULATION                         = 8U
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
  uint32_t              u32_filtered_pressure;
  
  /* System part. */
  BMP85_STATE_CONDITION bsc_state;
  uint64_t              u64_deadline;

} BMP85_STATE_STRUCTURE;

/* extern BMP85_STATE_STRUCTURE  bss_bmp85_state; */ 




BOARD_ERROR be_board_drv_bmp085_init(void);
uint32_t    u32_board_drv_bmp085_get_pressure(void);
int16_t     i16_board_drv_bmp085_get_temperature(void);



/* Temperature. */
static BOARD_ERROR be_board_drv_bmp085_raw_temperature_start_read(void);
static BOARD_ERROR be_board_drv_bmp085_read_raw_temperature(void);
/* Pressure. */
static BOARD_ERROR be_board_drv_bmp085_raw_pressure_start_read(void);
static BOARD_ERROR be_board_drv_bmp085_read_raw_pressure(void);

/* Calculation. */
BOARD_ERROR be_board_drv_bmp085_real_data_calculation(void);
static BOARD_ERROR be_board_drv_bmp085_callibration_read(void);

/* State machine. */
BOARD_ERROR be_board_drv_bmp085_state_machine(void);

/* System, get, set, condition. */
BOARD_ERROR be_board_drv_bmp085_init(void);
uint32_t    u32_board_drv_bmp085_get_pressure(void);
int16_t     i16_board_drv_bmp085_get_temperature(void);
void v_board_drv_bmp085_set_state(BMP85_STATE_CONDITION b85sc_state);
BMP85_STATE_CONDITION b85sc_board_drv_bmp085_get_state(void);

uint32_t u32_board_drv_bmp085_get_filtered_pressure(void);
void v_board_drv_bmp085_set_filtered_pressure(uint32_t u32_filtered_pressure);




#endif