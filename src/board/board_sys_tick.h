#ifndef BOARD_SYS_TICK_H
#define BOARD_SYS_TICK_H 1


#include "stm32f10x.h"
#include "board_NVIC.h"


void gv_board_sys_tick_init(void);
void SysTick_Handler(void);
void gv_board_sys_tick_delay(uint64_t u64_delay_time);
void gv_board_sys_tick_fast_delay( uint32_t u32_val);
static uint64_t gu64_read_system_time(void);


#endif