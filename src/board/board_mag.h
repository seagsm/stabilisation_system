#ifndef BOARD_MAG_H
#define BOARD_MAG_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_drv_hmc5883.h"



BOARD_ERROR be_board_mag_init(void);
BOARD_ERROR be_board_mag_read(void);







#endif