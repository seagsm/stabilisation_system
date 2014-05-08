#ifndef BOARD_DRV_BMP085_H
#define BOARD_DRV_BMP085_H 1

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_i2c.h"


BOARD_ERROR board_drv_bmp085_init(void);
int32_t i32_board_drv_bmp085_calculate_pressure(void);


#endif