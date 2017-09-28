#ifndef BOARD_BARO_MS5611_H
#define BOARD_BARO_MS5611_H 1

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_i2c.h"
#include "board_drv_ms5611.h"

#if 0
typedef enum
{
    START_CONVERSION            = 0U,
    CONVERSION_IN_PROGRESS      = 1U,
    CONVERSION_DONE             = 2U,
    UNDEFINED_STATE             = 3U
} BARO_STATE_CONDITION;
#endif

BOARD_ERROR be_board_baro_ms5611_init(void);
int16_t i16_board_baro_ms5611_get_temperature(void);
uint32_t u32_board_baro_ms5611_get_pressure(void);
int32_t  i32_board_baro_ms5611_get_altitude(void);

BOARD_ERROR be_board_baro_ms5611_set_conversion_state(BARO_STATE_CONDITION bsc_state);
BOARD_ERROR be_board_baro_ms5611_get_conversion_state(BARO_STATE_CONDITION *pbsc_state);


void v_board_baro_ms5611_data_compensation(void);

uint32_t u32_board_baro_ms5611_get_filtered_pressure(void);
void u32_board_baro_ms5611_set_filtered_pressure(uint32_t u32_filtered_pressure);

static BOARD_ERROR be_board_baro_ms5611_set_baro_dev_state(BOARD_DEV_STATE bds_value);
BOARD_ERROR be_board_baro_ms5611_get_baro_dev_state(BOARD_DEV_STATE *bds_value);


BOARD_ERROR be_board_baro_ms5611_get_state(BARO_STATE_CONDITION *pbsc_state);



#endif