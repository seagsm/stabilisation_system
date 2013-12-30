#ifndef BOARD_INIT_H
#define BOARD_INIT_H 1



#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_state.h"
#include "board_gpio.h"
#include "board_uart.h"


#define BOARD_INIT_TOTAL_STEPS 3U

BOARD_ERROR board_init_main_init(void);




#endif