#ifndef BOARD_BARO_BMP085_H
#define BOARD_BARO_BMP085_H 1

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_i2c.h"
#include "board_drv_bmp085.h"

BOARD_ERROR be_board_baro_bmp085_init(void);
int16_t i16_board_baro_bmp085_get_temperature(void);
uint32_t u32_board_baro_bmp085_get_pressure(void);
int32_t  i32_board_baro_bmp085_get_altitude(void);

BOARD_ERROR be_board_baro_bmp085_set_state(BARO_STATE_CONDITION bsc_state);
BARO_STATE_CONDITION bsc_board_baro_bmp085_get_state(void);
void v_board_baro_bmp085_data_compensation(void);

uint32_t u32_board_baro_bmp085_get_filtered_pressure(void);
void v_board_baro_bmp085_set_filtered_pressure(uint32_t u32_filtered_pressure);

static BOARD_ERROR be_board_baro_bmp085_set_baro_dev_state(BOARD_DEV_STATE  bds_value);
BOARD_ERROR be_board_baro_bmp085_get_baro_dev_state(BOARD_DEV_STATE *bds_value);


#endif