
/**/
#include "board_dma.h"

static uint8_t u8_board_dma_buff_DMA1_CH4_TX[DMA1_CH4_TX_SIZE];

static uint8_t u8_tx_data_packet[USART_TX_DATA_PACKET_SIZE];

/* This function should initialise usart dma. */
BOARD_ERROR be_board_dma1_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    NVIC_InitTypeDef  NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    /* DMA module clk ON. */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* DMA reset. */
    DMA_DeInit(DMA1_Channel4);

    /* Fill DMA init structure before initialisation. */
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t)(&USART1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)u8_board_dma_buff_DMA1_CH4_TX;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize            = 1U;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_Low;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;

    /* Initialisation of DMA UART TX. */
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    /* UART1 TX  <-  DMA enable. */
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

    /* Setup DMA TX end of transfer interrupt. */
    NVIC_InitStructure.NVIC_IRQChannel = (unsigned char)DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_Channel4_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_Channel4_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);

    /* Start DMA transmitting. */
    DMA_Cmd(DMA1_Channel4, ENABLE);

    return(be_result);
}




/* Function copy tx data packet to DMA TX round buffer. */
static void sv_board_dma_send_packet(uint16_t u16_size_of_tx_data)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint16_t u16_byte_counter;
    /* Disable DMA interrupt to avoid problem with dual access to round buffer. */
    NVIC_DisableIRQ(DMA1_Channel4_IRQn);

    /* Copy data to USART1 TX round buffer. */
    u16_byte_counter = 0U;
    while(u16_byte_counter < u16_size_of_tx_data)
    {
        be_result = be_board_r_buff_USART1_TX_Put_byte(u8_tx_data_packet[u16_byte_counter]);
        if(be_result == BOARD_ERR_FULL)
        {
            break;
        }
        u16_byte_counter++;
    }

    /* Enable DMA interrupt to start DMA reinitialisation and transaction. */
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}

void DMA1_Channel4_IRQHandler(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t u8_byte;
    uint16_t u16_counter;

    /* Copy from UASRT1 user round buffer to DMA1_CH4 buffer. */
    u16_counter = 0U;
    while((be_result == BOARD_ERR_OK)&&(u16_counter < DMA1_CH4_TX_SIZE))
    {
        be_result = be_board_r_buff_USART1_TX_Get_byte(&u8_byte);
        if(be_result == BOARD_ERR_OK)
        {
            u8_board_dma_buff_DMA1_CH4_TX[u16_counter] = u8_byte;
            u16_counter++;
        }
    }
    if(u16_counter > 0U)
    {
        /* Restart DMA chanel for new transaction. */
        DMA_Cmd(DMA1_Channel4, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Channel4, u16_counter);
        DMA_Cmd(DMA1_Channel4, ENABLE);

        /* clear flag "end_of_TX through DMA channel". */
        DMA_ClearFlag(DMA_ISR_TCIF4);
    }
    else
    {
        /* If buffer empty, do not clear interrupt flag, just disable interrupt.
         * Function that will try to send something enable interrupt again.
         */
        NVIC_DisableIRQ(DMA1_Channel4_IRQn);
    }
}

/* Function send data packet to ground station(PC).
   It placed here to avoid MICRA restriction to operate with pointers to array.
*/
void board_dma_send_buff(void)
{
    uint16_t u16_i = 0U;
#if 1
    u8_tx_data_packet[u16_i] = (uint8_t)(bc_channel_value_structure.u16_channel_1_value & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bc_channel_value_structure.u16_channel_1_value>>8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)(bc_channel_value_structure.u16_channel_2_value & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bc_channel_value_structure.u16_channel_2_value>>8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)(bc_channel_value_structure.u16_channel_3_value & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bc_channel_value_structure.u16_channel_3_value>>8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)(bc_channel_value_structure.u16_channel_4_value & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bc_channel_value_structure.u16_channel_4_value>>8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)(bc_channel_value_structure.u16_channel_5_value & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bc_channel_value_structure.u16_channel_5_value>>8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)(bc_channel_value_structure.u16_channel_6_value & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bc_channel_value_structure.u16_channel_6_value>>8) & 0xFFU);
    u16_i++;

/* Sensors. */
    u8_tx_data_packet[u16_i] = (uint8_t)(bu163d_api_main_loop_gyro_raw_data.u16_X & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bu163d_api_main_loop_gyro_raw_data.u16_X >> 8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)(bu163d_api_main_loop_gyro_raw_data.u16_Y & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bu163d_api_main_loop_gyro_raw_data.u16_Y >> 8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)(bu163d_api_main_loop_gyro_raw_data.u16_Z & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bu163d_api_main_loop_gyro_raw_data.u16_Z >> 8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)(bu163d_api_main_loop_acce_raw_data.u16_X & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bu163d_api_main_loop_acce_raw_data.u16_X >> 8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)(bu163d_api_main_loop_acce_raw_data.u16_Y & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bu163d_api_main_loop_acce_raw_data.u16_Y >> 8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)(bu163d_api_main_loop_acce_raw_data.u16_Z & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bu163d_api_main_loop_acce_raw_data.u16_Z >> 8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)(bu163d_api_main_loop_magn_raw_data.u16_X & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bu163d_api_main_loop_magn_raw_data.u16_X >> 8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)(bu163d_api_main_loop_magn_raw_data.u16_Y & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bu163d_api_main_loop_magn_raw_data.u16_Y >> 8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)(bu163d_api_main_loop_magn_raw_data.u16_Z & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = (uint8_t)((bu163d_api_main_loop_magn_raw_data.u16_Z >> 8) & 0xFFU);
    u16_i++;
    u8_tx_data_packet[u16_i] = 0x0AU;
    u16_i++;
    u8_tx_data_packet[u16_i] = 0x0DU;
    u16_i++;
#endif
    sv_board_dma_send_packet(u16_i);

}
















