#ifndef BOARD_DRV_MS5611_H
#define BOARD_DRV_MS5611_H 1

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_i2c.h"



/* MS5611, Standard address 0xEE, but it is address of BMP085. */

#define MS5611_ADDR                          0xECU  /* 111011Cx , C = 0 */


/*  Autodetect: turn off BMP085 while initializing ms5611 and check PROM crc to confirm device */
#define BMP085_OFF                  digitalLo(BARO_GPIO, BARO_PIN);
#define BMP085_ON                   digitalHi(BARO_GPIO, BARO_PIN);


#define CMD_RESET               0x1EU /* ADC reset command       */
#define CMD_ADC_READ            0x00U /* ADC read command        */
#define CMD_ADC_CONV            0x40U /* ADC conversion command  */
#define CMD_ADC_D1              0x00U /* ADC D1 conversion       */
#define CMD_ADC_D2              0x10U /* ADC D2 conversion       */
#define CMD_ADC_256             0x00U /* ADC OSR=256             */
#define CMD_ADC_512             0x02U /* ADC OSR=512             */
#define CMD_ADC_1024            0x04U /* ADC OSR=1024            */
#define CMD_ADC_2048            0x06U /* ADC OSR=2048            */
#define CMD_ADC_4096            0x08U /* ADC OSR=4096            */
#define CMD_PROM_RD             0xA0U /* Prom read command       */
#define PROM_NB                 8U


#define MS5611_CONVERSION_TIME  10U

typedef BOARD_ERROR (* baroOpFuncBrdErrPtr)(void);
typedef void (* baroOpFuncPtr)(void);                                           /* baro start operation */
typedef void (* baroCalculateFuncPtr)(int32_t *pressure, int32_t *temperature); /* baro calculation (filled params are */

typedef struct Baro_t
{
    uint64_t ut_delay;
    uint64_t up_delay;
    baroOpFuncBrdErrPtr start_ut;
    baroOpFuncBrdErrPtr get_ut;
    baroOpFuncBrdErrPtr start_up;
    baroOpFuncBrdErrPtr get_up;
    baroCalculateFuncPtr calculate;
    int32_t i32_pressure;
    int32_t i32_temperature;
    uint32_t u32_filtered_pressure;
} baro_t;

typedef enum
{
    MS5611_START_TEMP_CONVERSION               = 0U, /* Write start temperature command to control register. */
    MS5611_START_READ_UNCOMP_TEMP              = 1U, /* Write read address. */
    MS5611_READ_UNCOMPENSATED_TEMP             = 2U, /* Read data from setted address. */
    MS5611_WAIT_FOR_TEMP_DATA_READY            = 3U, /* Wait for temperature data read done. */
    MS5611_START_PRESS_CONVERSION              = 4U, /* Write start pressure command to control register. */
    MS5611_START_READ_UNCOMP_PRESS             = 5U, /* Write read address. */
    MS5611_READ_UNCOMPENSATED_PRESS            = 6U, /* Write read address. */
    MS5611_WAIT_FOR_PRESS_DATA_READY           = 7U, /* Wait for pressure data read done. */
    MS5611_CALCULATION                         = 8U
} MS5611_STATE_CONDITION;


static BOARD_ERROR board_drv_get_raw_value(uint32_t *pu32_variable);

BOARD_ERROR board_drv_get_baro(baro_t **pb_baro);
BOARD_ERROR board_drv_ms5611Detect(baro_t *baro);
BOARD_ERROR board_drv_ms5611_reset(void);


BOARD_ERROR be_board_drv_ms5611_set_conversion_state(MS5611_STATE_CONDITION msc_state);
BOARD_ERROR be_board_drv_ms5611_get_conversion_state(MS5611_STATE_CONDITION *msc_state);
BOARD_ERROR be_board_drv_ms5611_state_machine(void);

int32_t i32_board_drv_ms5611_get_temperature(void);
int32_t i32_board_drv_ms5611_get_pressure(void);

uint32_t u32_board_drv_ms5611_get_filtered_pressure(void);
void       v_board_drv_ms5611_set_filtered_pressure(uint32_t u32_filtered_pressure);


#endif