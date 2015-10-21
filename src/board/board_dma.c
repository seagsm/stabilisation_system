
/**/
#include "board_dma.h"

static uint8_t u8_board_dma_buff_DMA1_CH2_TX[DMA1_CH2_TX_SIZE];
static uint8_t u8_board_dma_buff_DMA1_CH3_RX[DMA1_CH3_RX_SIZE];

static uint8_t u8_board_dma_buff_DMA1_CH4_TX[DMA1_CH4_TX_SIZE];
static uint8_t u8_board_dma_buff_DMA1_CH5_RX[DMA1_CH5_RX_SIZE];

static uint16_t u16_DMA1_CH3_interrupt_counter = 0U;
static uint16_t u16_DMA1_CH3_head_index = 0U;
static uint16_t u16_DMA1_CH3_tail_index = 0U;
static uint16_t u16_DMA1_CH3_intr_index = 0U;

static uint16_t u16_DMA1_CH5_interrupt_counter = 0U;
static uint16_t u16_DMA1_CH5_head_index = 0U;
static uint16_t u16_DMA1_CH5_tail_index = 0U;
static uint16_t u16_DMA1_CH5_intr_index = 0U;


uint8_t u8_tx_UART1_data_packet[USART1_TX_DATA_PACKET_SIZE];
uint8_t u8_tx_UART3_data_packet[USART3_TX_DATA_PACKET_SIZE];



BOARD_ERROR be_board_dma1_ch2_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    NVIC_InitTypeDef  NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    /* DMA module clk ON. */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* DMA reset. */
    DMA_DeInit(DMA1_Channel2);

    /* Fill DMA init structure before initialisation. */
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t)(&USART3->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)u8_board_dma_buff_DMA1_CH2_TX;
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
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);

    /* UART3 TX  <-  DMA enable. */
    USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);

    /* Setup DMA TX end of transfer interrupt. */
    NVIC_InitStructure.NVIC_IRQChannel                      = (unsigned char)DMA1_Channel2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = DMA1_Channel2_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = DMA1_Channel2_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

    /* Start DMA transmitting. */
    DMA_Cmd(DMA1_Channel2, ENABLE);
    return(be_result);
}

BOARD_ERROR be_board_dma1_ch3_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    NVIC_InitTypeDef  NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    /* DMA module clk ON. */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* DMA reset. */
    DMA_DeInit(DMA1_Channel3);

    /* Fill DMA init structure before initialisation. */
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t)(&USART3->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)u8_board_dma_buff_DMA1_CH3_RX;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize            = DMA1_CH3_RX_SIZE;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_Low;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;

    /* Initialisation of DMA UART TX. */
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

    /* UART3 RX  ->  DMA enable. */
    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);

    /* Setup DMA TX end of transfer interrupt. */
    NVIC_InitStructure.NVIC_IRQChannel = (unsigned char)DMA1_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_Channel3_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_Channel3_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);

    /* Start DMA transmitting. */
    DMA_Cmd(DMA1_Channel3, ENABLE);
    return(be_result);
}

/* This function should initialise usart TX dma channel 4. */
BOARD_ERROR be_board_dma1_ch4_init(void)
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

/* This function should initialise usart RX dma channel 5. */
BOARD_ERROR be_board_dma1_ch5_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    NVIC_InitTypeDef  NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    /* DMA module clk ON. */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* DMA reset. */
    DMA_DeInit(DMA1_Channel5);

    /* Fill DMA init structure before initialisation. */
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t)(&USART1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)u8_board_dma_buff_DMA1_CH5_RX;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize            = DMA1_CH5_RX_SIZE;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_Low;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;

    /* Initialisation of DMA UART TX. */
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    /* UART1 RX  ->  DMA enable. */
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

    /* Setup DMA TX end of transfer interrupt. */
    NVIC_InitStructure.NVIC_IRQChannel = (unsigned char)DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_Channel5_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_Channel5_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

    /* Start DMA transmitting. */
    DMA_Cmd(DMA1_Channel5, ENABLE);

    return(be_result);
}

/* Function get value from packet buffer for UART. */
BOARD_ERROR be_board_dma_get_UARTx_packet(BOARD_UART UARTx, uint8_t *u8_value, uint16_t u16_index)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    switch(UARTx)
    {
        case BOARD_UART1:
            if(u16_index < USART1_TX_DATA_PACKET_SIZE)
            {
                *u8_value = u8_tx_UART1_data_packet[u16_index];
            }
            else
            {
                be_result = BOARD_ERR_RANGE;
            }
            break;

        case BOARD_UART2:
            be_result = BOARD_ERR_RANGE;
            break;

        case BOARD_UART3:
            if(u16_index < USART3_TX_DATA_PACKET_SIZE)
            {
                *u8_value = u8_tx_UART3_data_packet[u16_index];
            }
            else
            {
                be_result = BOARD_ERR_RANGE;
            }
            break;
        default:
            be_result = BOARD_ERR_RANGE;
            break;
    }
    return(be_result);
}


/* Function set value from packet buffer for UART. */
BOARD_ERROR be_board_dma_set_UARTx_packet(BOARD_UART UARTx, uint8_t u8_value, uint16_t u16_index)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    switch(UARTx)
    {
        case BOARD_UART1:
            if(u16_index < USART1_TX_DATA_PACKET_SIZE)
            {
                u8_tx_UART1_data_packet[u16_index] = u8_value;
            }
            else
            {
                be_result = BOARD_ERR_RANGE;
            }
            break;

        case BOARD_UART2:
            be_result = BOARD_ERR_RANGE;
            break;

        case BOARD_UART3:
            if(u16_index < USART3_TX_DATA_PACKET_SIZE)
            {
                u8_tx_UART3_data_packet[u16_index] = u8_value;
            }
            else
            {
                be_result = BOARD_ERR_RANGE;
            }
            break;
        default:
            be_result = BOARD_ERR_RANGE;
            break;
    }
    return(be_result);
}


/* Function copy tx data packet to DMA TX round buffer. */
void sv_board_dma_ch2_send_packet(uint16_t u16_size_of_tx_data)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint16_t u16_byte_counter;

    /* Disable DMA interrupt to avoid problem with dual access to round buffer. */
    NVIC_DisableIRQ(DMA1_Channel2_IRQn);

    /* Copy data to USART3 TX round buffer. */
    u16_byte_counter = 0U;
    while(u16_byte_counter < u16_size_of_tx_data)
    {
        be_result = be_board_r_buff_USARTx_TX_PUT_byte(USART3, u8_tx_UART3_data_packet[u16_byte_counter]);

        if(be_result == BOARD_ERR_FULL)
        {
            break;
        }
        u16_byte_counter++;
    }

    /* Enable DMA interrupt to start DMA reinitialisation and transaction. */
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
}



/* Function copy tx data packet to DMA TX round buffer. */
void v_board_dma_send_packet(uint16_t u16_size_of_tx_data)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint16_t u16_byte_counter;
    uint8_t   u8_value;
    /* Disable DMA interrupt to avoid problem with dual access to round buffer. */
    NVIC_DisableIRQ(DMA1_Channel4_IRQn);

    /* Copy data to USART1 TX round buffer. */
    u16_byte_counter = 0U;
    while(u16_byte_counter < u16_size_of_tx_data)
    {
        /* be_result = be_board_r_buff_USART1_TX_Put_byte(u8_tx_UART1_data_packet[u16_byte_counter]); */
        be_board_dma_get_UARTx_packet(BOARD_UART1, &u8_value, u16_byte_counter);
        be_result = be_board_r_buff_USART1_TX_Put_byte(u8_value);
        if(be_result == BOARD_ERR_FULL)
        {
            break;
        }
        u16_byte_counter++;
    }

    /* Enable DMA interrupt to start DMA reinitialisation and transaction. */
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}

void DMA1_Channel2_IRQHandler(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t u8_byte;
    uint16_t u16_counter;

    /* Copy from UASRT1 user round buffer to DMA1_CH4 buffer. */
    u16_counter = 0U;
    while((be_result == BOARD_ERR_OK)&&(u16_counter < DMA1_CH2_TX_SIZE))
    {
        /* be_result = be_board_r_buff_USART1_TX_Get_byte(&u8_byte); */
        be_result = be_board_r_buff_USARTx_TX_GET_byte( USART3, &u8_byte);
        if(be_result == BOARD_ERR_OK)
        {
            u8_board_dma_buff_DMA1_CH2_TX[u16_counter] = u8_byte;
            u16_counter++;
        }
    }
    if(u16_counter > 0U)
    {
        /* Restart DMA chanel for new transaction. */
        DMA_Cmd(DMA1_Channel2, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Channel2, u16_counter);
        DMA_Cmd(DMA1_Channel2, ENABLE);

        /* clear flag "end_of_TX through DMA channel". */
        DMA_ClearFlag(DMA_ISR_TCIF2);
    }
    else
    {
        /* If buffer empty, do not clear interrupt flag, just disable interrupt.
         * Function that will try to send something enable interrupt again.
         */
        NVIC_DisableIRQ(DMA1_Channel2_IRQn);
    }
}

void DMA1_Channel3_IRQHandler(void)
{
    /* Add circle DMA buffer overflow counter. */
    u16_DMA1_CH3_interrupt_counter++;
    /* Reset DMA transfer complete interrupt.*/
    DMA1->IFCR |= DMA_ISR_TCIF3;
}

/* Function return amount of received bytes by UART3 -> DMA1_CH3. */
static uint16_t u16_board_dma_DMA1_CH3_byte_received(void)
{
    uint16_t u16_byte_received = 0U;

    u16_DMA1_CH3_intr_index = u16_DMA1_CH3_interrupt_counter;
    u16_DMA1_CH3_head_index = DMA1_CH3_RX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);
    u16_byte_received = ( u16_DMA1_CH3_intr_index * DMA1_CH3_RX_SIZE) + u16_DMA1_CH3_head_index - u16_DMA1_CH3_tail_index;

    return u16_byte_received;
}

/* Function copy received data from DMA1_CH3 buffer to UART1 received buffer. It should be called uninterrupted as possible.*/
BOARD_ERROR be_board_dma_DMA1_CH3_buffer_copy_to_UART3_buffer(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint16_t u16_byte_counter = 0U;
    uint8_t u8_byte;

    u16_byte_counter = u16_board_dma_DMA1_CH3_byte_received();
    if(u16_byte_counter > 0U )
    {
        /* If read more that buffer size, we losted some bytes, but we can read just last buffer size amount of bytes. */
        if(u16_byte_counter > DMA1_CH3_RX_SIZE )
        {
            u16_byte_counter = DMA1_CH3_RX_SIZE;

            /* Probably we loosed some data, so, set tail index to head index and read all byte from buffer. */
            u16_DMA1_CH3_tail_index = u16_DMA1_CH3_head_index;

            /* Read byte by byte. */ /* Code should be optimised. (same part of code)*/
            while (u16_byte_counter > 0U)
            {
                u8_byte = u8_board_dma_buff_DMA1_CH3_RX[u16_DMA1_CH3_tail_index];

                /* be_result = be_board_r_buff_tail_eat_USART1_RX_Put_byte(u8_byte);*/
                be_result = be_board_r_buff_USARTx_tail_eat_RX_PUT_byte(USART3, u8_byte);
                u16_byte_counter--;
                u16_DMA1_CH3_tail_index++;
                if(u16_DMA1_CH3_tail_index >= DMA1_CH3_RX_SIZE)
                {
                    u16_DMA1_CH3_tail_index = 0U;
                }
            }
        }
        else
        {
            /* Read byte by byte. */
            while (u16_byte_counter > 0U)
            {
                u8_byte = u8_board_dma_buff_DMA1_CH3_RX[u16_DMA1_CH3_tail_index];

                /* be_result = be_board_r_buff_tail_eat_USART1_RX_Put_byte(u8_byte); */
                be_result = be_board_r_buff_USARTx_tail_eat_RX_PUT_byte(USART3, u8_byte);
                u16_byte_counter--;
                u16_DMA1_CH3_tail_index++;
                if(u16_DMA1_CH3_tail_index >= DMA1_CH3_RX_SIZE)
                {
                    u16_DMA1_CH3_tail_index = 0U;
                }
            }
        }
        /* Reset interrupt counter, related with size of received bytes. */
        u16_DMA1_CH3_interrupt_counter = u16_DMA1_CH3_interrupt_counter - u16_DMA1_CH3_intr_index;
    }
    return(be_result);
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

void DMA1_Channel5_IRQHandler(void)
{
    /* Add circle DMA buffer overflow counter. */
    u16_DMA1_CH5_interrupt_counter++;
    /* Reset DMA transfer complete interrupt.*/
    DMA1->IFCR |= DMA_ISR_TCIF5;
}

/* Function return amount of received bytes by UART1 -> DMA1_CH5. */
static uint16_t u16_board_dma_DMA1_CH5_byte_received(void)
{
    uint16_t u16_byte_received = 0U;

    u16_DMA1_CH5_intr_index = u16_DMA1_CH5_interrupt_counter;
    u16_DMA1_CH5_head_index = DMA1_CH5_RX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
    u16_byte_received = ( u16_DMA1_CH5_intr_index * DMA1_CH5_RX_SIZE) + u16_DMA1_CH5_head_index - u16_DMA1_CH5_tail_index;
    return u16_byte_received;
}


/* Function copy received data from DMA1_CH5 buffer to UART1 received buffer. It should be called uninterrupted as possible.*/
BOARD_ERROR be_board_dma_DMA1_CH5_buffer_copy_to_UART1_buffer(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint16_t u16_byte_counter = 0U;
    uint8_t u8_byte;

    u16_byte_counter = u16_board_dma_DMA1_CH5_byte_received();
    if(u16_byte_counter > 0U )
    {
        /* If read more that buffer size, we losted some bytes, but we can read just last buffer size amount of bytes. */
        if(u16_byte_counter > DMA1_CH5_RX_SIZE )
        {
            u16_byte_counter = DMA1_CH5_RX_SIZE;

            /* Probably we loosed some data, so, set tail index to head index and read all byte from buffer. */
            u16_DMA1_CH5_tail_index = u16_DMA1_CH5_head_index;

            /* Read byte by byte. */ /* Code should be optimised. (same part of code)*/
            while (u16_byte_counter > 0U)
            {
                u8_byte = u8_board_dma_buff_DMA1_CH5_RX[u16_DMA1_CH5_tail_index];
                be_result = be_board_r_buff_tail_eat_USART1_RX_Put_byte(u8_byte);
                u16_byte_counter--;
                u16_DMA1_CH5_tail_index++;
                if(u16_DMA1_CH5_tail_index >= DMA1_CH5_RX_SIZE)
                {
                    u16_DMA1_CH5_tail_index = 0U;
                }
            }
        }
        else
        {
            /* Read byte by byte. */
            while (u16_byte_counter > 0U)
            {
                u8_byte = u8_board_dma_buff_DMA1_CH5_RX[u16_DMA1_CH5_tail_index];
                be_result = be_board_r_buff_tail_eat_USART1_RX_Put_byte(u8_byte);
                u16_byte_counter--;
                u16_DMA1_CH5_tail_index++;
                if(u16_DMA1_CH5_tail_index >= DMA1_CH5_RX_SIZE)
                {
                    u16_DMA1_CH5_tail_index = 0U;
                }
            }
        }
        /* Reset interrupt counter, related with size of received bytes. */
        u16_DMA1_CH5_interrupt_counter = u16_DMA1_CH5_interrupt_counter - u16_DMA1_CH5_intr_index;
    }
    return(be_result);
}

