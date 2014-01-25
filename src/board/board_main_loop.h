#ifndef BOARD_MAIN_LOOP_H
#define BOARD_MAIN_LOOP_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_NVIC.h"



BOARD_ERROR be_board_main_loop_init(uint16_t u16_period_of_interupt);
BOARD_ERROR be_board_main_loop_start(void);
static BOARD_ERROR be_board_main_loop_TIMER1_interupt_period_init(uint16_t u16_period_of_interupt);
void TIM1_UP_IRQHandler(void);


#endif