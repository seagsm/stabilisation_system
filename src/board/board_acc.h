#ifndef BOARD_ACC_H
#define BOARD_ACC_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_drv_adxl345.h"


BOARD_ERROR be_board_acc_init(void);
BOARD_ERROR be_board_acc_read(void);


#endif