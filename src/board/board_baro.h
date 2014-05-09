#ifndef BOARD_BARO_H
#define BOARD_BARO_H 1

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_i2c.h"
#include "board_drv_bmp085.h"

BOARD_ERROR be_board_baro_init(void);
uint32_t u32_board_baro_get_pressure(void);
int32_t  i32_board_baro_get_altitude(void);


#endif