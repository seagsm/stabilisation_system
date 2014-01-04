
/**/
#include "board_dma.h"

static uint8_t u8_board_dma_buff_DMA1_CH4_TX[DMA1_CH4_TX_SIZE];

/* This function should initialise usart dma. */
BOARD_ERROR be_board_dma1_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

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

    /* UART -> DMA enable. */
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    /* End of Tx interrupt. */
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);


    /*********************************************/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  /*  __enable_irq(); */
    /*********************************************/
    NVIC_SetPriority(DMA1_Channel4_IRQn, 0x00U);
   /* NVIC_EnableIRQ(DMA1_Channel4_IRQn); */

    /* TODO: it is example of code. */

    /* Start DMA transmitting. */
    DMA1_Channel4->CCR |= DMA_CCR1_EN;




    /*Test for round buffer. */
    {
        gv_board_dma_send_packet();
    }


    /*********************************/

    return(be_result);
}

static void gv_board_dma_send_packet(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    uint8_t test[100];

    uint16_t u16_byte_counter;

    test[0] = 'h';
    test[1] = 'e';
    test[2] = 'l';
    test[3] = 'l';
    test[4] = 'o';
    test[5] = ' ';
    test[6] = 'w';
    test[7] = 'o';
    test[8] = 'r';
    test[9] = 'd';
    test[10] = 0x0AU;
    test[11] = 0x0DU;
    test[12] = 0x00U;

    u16_byte_counter = 0U;

    /* Disable DMA interrupt to avoid problem with dual access to round buffer. */
    NVIC_DisableIRQ(DMA1_Channel4_IRQn);

    /* Copy data to USART1 TX round buffer. */
    while(test[u16_byte_counter] != 0U)
    {
        be_result = be_board_r_buff_USART1_TX_Put_byte(test[u16_byte_counter]);
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

    u16_counter = 0U;
    /* Copy from UASRT1 round buffer to DMA1_CH4 buffer. */
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
    {   /* Restart DMA chanel for new transaction. */
        DMA1_Channel4->CCR  &= (uint16_t)(~DMA_CCR1_EN);
        DMA1_Channel4->CNDTR = u16_counter;
        DMA1_Channel4->CCR  |= DMA_CCR1_EN;

        /* clear flag "end_of_TX through DMA channel". */
        DMA1->IFCR |= DMA_ISR_TCIF4;
    }
    else
    {
        /* If buffer empty, do not clear interrupt flag, just disable interrupt.
         * Function that will try to send something enable interrupt again.
         */
        NVIC_DisableIRQ(DMA1_Channel4_IRQn);
    }
}