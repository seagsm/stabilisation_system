#ifndef BOARD_GYRO_H
#define BOARD_GYRO_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_drv_l3g4200d.h"

extern BOARD_GYRO_DATA board_gyro_data;

BOARD_ERROR be_board_gyro_detect(void);
BOARD_ERROR be_board_gyro_init(void);
BOARD_ERROR be_board_gyro_read(BOARD_GYRO_DATA *p_board_gyro_data);











#endif