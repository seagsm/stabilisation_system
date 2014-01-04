#ifndef BOARD_DMA_H
#define BOARD_DMA_H 1



#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_r_buff.h"

BOARD_ERROR be_board_dma1_init(void);

void DMA1_Channel4_IRQHandler(void);





#endif