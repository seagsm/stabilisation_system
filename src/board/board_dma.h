#ifndef BOARD_DMA_H
#define BOARD_DMA_H 1



#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_r_buff.h"
#include "stdio.h"

#define DMA1_CH4_TX_SIZE 16U

BOARD_ERROR be_board_dma1_init(void);

void DMA1_Channel4_IRQHandler(void);
static void gv_board_dma_send_packet(void);




#endif