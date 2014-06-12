#ifndef BOARD_BARO_H
#define BOARD_BARO_H 1

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_i2c.h"
#include "board_drv_bmp085.h"

typedef enum
{
    START_CONVERSION            = 0U,
    CONVERSION_IN_PROGRESS      = 1U,
    CONVERSION_DONE             = 2U,
    UNDEFINED_STATE             = 3U
} BARO_STATE_CONDITION;

BOARD_ERROR be_board_baro_init(void);
int16_t i16_board_baro_get_temperature(void);
uint32_t u32_board_baro_get_pressure(void);
int32_t  i32_board_baro_get_altitude(void);

BOARD_ERROR be_board_baro_set_state(BARO_STATE_CONDITION bsc_state);
BARO_STATE_CONDITION bsc_board_baro_get_state(void);
void v_board_baro_data_compensation(void);

uint32_t u32_board_baro_get_filtered_pressure(void);
void u32_board_baro_set_filtered_pressure(uint32_t u32_filtered_pressure);

#endif