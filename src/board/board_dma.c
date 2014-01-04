
/**/
#include "board_dma.h"

/*TODO: It is example of TX buffer. */
static char text[]="Hello World.";
static char text1[]="I like you.";
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
    DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)text;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize            = sizeof(text);
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


    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    __enable_irq();
    NVIC_SetPriority(DMA1_Channel4_IRQn, 0x00U);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    /* TODO: it is example of code. */
    /* Start DMA transmitting. */
    DMA1_Channel4->CCR |= DMA_CCR1_EN;

    /* Retransmitting, disable DMA, set new parameters, start DMA tx.*/
    DMA1_Channel4->CCR  &= (uint16_t)(~DMA_CCR1_EN);
    DMA1_Channel4->CMAR = (uint32_t) &text1[0];
    DMA1_Channel4->CNDTR = sizeof(text1);
    DMA1_Channel4->CCR  |= DMA_CCR1_EN;



    /*Test for round buffer. */
    {
        uint8_t u8_test_value = 0x00U;
        be_board_r_buff_USART1_init();
        while(u8_test_value < 0x67U)
        {
            be_board_r_buff_USART1_TX_Put_byte(u8_test_value);
            u8_test_value++;
        }


        while(be_result == BOARD_ERR_OK )
        {
          be_result  =  be_board_r_buff_USART1_TX_Get_byte(&u8_test_value);
        }


    }


    /*********************************/

    return(be_result);
}


void DMA1_Channel4_IRQHandler(void)
{
    /* clear flag "end_of_TX". */
    DMA1->IFCR |= DMA_ISR_TCIF4;
}