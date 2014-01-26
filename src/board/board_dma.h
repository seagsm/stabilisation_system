#ifndef BOARD_DMA_H
#define BOARD_DMA_H 1



#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_r_buff.h"
#include "board_ppm.h"
#include "board_gyro.h"
#include "board_sys_tick.h"
#include "api_main_loop.h"

/* This is size of packet that will be send by DMA TX. */
#define DMA1_CH4_TX_SIZE 16U
/*
    This is maximun size of data packet that can be sent for one time.
    I think it is good idea to have same size like DMA TX round buffer.
*/
#define USART_TX_DATA_PACKET_SIZE   TX_USART1_SIZE
BOARD_ERROR be_board_dma1_init(void);

void DMA1_Channel4_IRQHandler(void);
static void sv_board_dma_send_packet(uint16_t u16_size_of_tx_data);
void board_dma_send_buff(void);

#endif