
/**/
#include "board_dma.h"

static uint8_t u8_board_dma_buff_DMA1_CH4_TX[DMA1_CH4_TX_SIZE];
static uint8_t u8_board_dma_buff_DMA1_CH5_RX[DMA1_CH5_RX_SIZE];
static uint16_t u16_DMA1_CH5_interrupt_counter = 0U;
static uint16_t u16_DMA1_CH5_head_index = 0U;
static uint16_t u16_DMA1_CH5_tail_index = 0U;
static uint16_t u16_DMA1_CH5_intr_index = 0U;


uint8_t u8_tx_data_packet[USART_TX_DATA_PACKET_SIZE];
uint8_t u8_rx_data_packet[USART_TX_DATA_PACKET_SIZE];

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





#if 0
/* old gui tx function. */
/* Function send data packet to ground station(PC).
   It placed here to avoid MICRA restriction to operate with pointers to array.
*/
void board_dma_send_buff(void)
{
    uint16_t u16_CRC = 0U;
    uint16_t u16_i;

#if 1
/* HEAD of TX packet. */
    board_dma_add_head_of_tx_packet(&u16_i);/* 4 bytes. */
/* PPM channels. */
    board_dma_add_u16_to_packet(&u16_i, bc_channel_value_structure.u16_channel_1_value);/* 2 bytes. */
    board_dma_add_u16_to_packet(&u16_i, bc_channel_value_structure.u16_channel_2_value);/* 2 bytes. */
    board_dma_add_u16_to_packet(&u16_i, bc_channel_value_structure.u16_channel_3_value);/* 2 bytes. */
    board_dma_add_u16_to_packet(&u16_i, bc_channel_value_structure.u16_channel_4_value);/* 2 bytes. */
    board_dma_add_u16_to_packet(&u16_i, bc_channel_value_structure.u16_channel_5_value);/* 2 bytes. */
    board_dma_add_u16_to_packet(&u16_i, bc_channel_value_structure.u16_channel_6_value);/* 2 bytes. */
/* Sensors raw data. */
    board_dma_add_bi163x_to_packet(&u16_i, bi163d_api_data_prepr_gyro_raw_data);/* 6 bytes. */
    board_dma_add_bi163x_to_packet(&u16_i, bi163d_api_data_prepr_acce_raw_data);/* 6 bytes. */
    board_dma_add_bi163x_to_packet(&u16_i, bi163d_api_data_prepr_magn_raw_data);/* 6 bytes. */
/* Send sensors normalising data. */
    board_dma_add_b_float3d_to_packet(&u16_i, b_float3d_api_data_norm_gyro_data);
    board_dma_add_b_float3d_to_packet(&u16_i, b_float3d_api_data_norm_acce_data);
    board_dma_add_b_float3d_to_packet(&u16_i, b_float3d_api_data_norm_magn_data);
/* Send quaternion. */
    board_dma_add_float_to_packet(&u16_i, fl_api_body_angle_quaternion[0]);
    board_dma_add_float_to_packet(&u16_i, fl_api_body_angle_quaternion[1]);
    board_dma_add_float_to_packet(&u16_i, fl_api_body_angle_quaternion[2]);
    board_dma_add_float_to_packet(&u16_i, fl_api_body_angle_quaternion[3]);
/* Send body wind angles fl_api_body_angle_wind_angles*/
    board_dma_add_float_to_packet(&u16_i, fl_api_body_angle_wind_angles[0]);
    board_dma_add_float_to_packet(&u16_i, fl_api_body_angle_wind_angles[1]);
    board_dma_add_float_to_packet(&u16_i, fl_api_body_angle_wind_angles[2]);

/*  PDF output. */
    board_dma_add_i32_to_packet(&u16_i, pid_api_pid_data[0].i32_pid_output);
    board_dma_add_i32_to_packet(&u16_i, pid_api_pid_data[1].i32_pid_output);
    board_dma_add_i32_to_packet(&u16_i, pid_api_pid_data[2].i32_pid_output);

 /* System time. */
    board_dma_add_system_time_to_tx_packet(&u16_i);/* 8 bytes. */
/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u16_CRC = gu16_api_CRC16_alg(1U, u16_i);
/* CRC. */
    board_dma_add_u16_to_packet(&u16_i,u16_CRC);/* 2 bytes. */
#else
/* Test packet. */
    /* HEAD of TX packet. */
    board_dma_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
    {
        while(u16_i < (83U))
        {
            u8_tx_data_packet[u16_i] = (uint8_t)u16_i;
            u16_i++;
        }
    }
    /* System time. */
    board_dma_add_system_time_to_tx_packet(&u16_i);/* 8 bytes. */
    /* CRC calculation of all array from 0+4 (size of head) to current u16_i.*/
    u16_CRC = gu16_api_CRC16_alg(1U, u16_i);
    board_dma_add_u16_to_packet(&u16_i,u16_CRC);/* 2 bytes. */
#endif
    /* Send packet. */
    sv_board_dma_send_packet(u16_i);
}
/* End ols function. */
#endif

/* Function send WRITE_OK.*/
void board_dma_send_WRITE_OK(void)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_dma_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    u8_tx_data_packet[u16_i] = WRITE_OK; /* write_ok */
    u16_i++;/* index of next element */

/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    u8_tx_data_packet[u16_i] = u8_CRC; /* 1 bytes. */
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
    u8_tx_data_packet[0x01U] = u8_size; /* 1 bytes. */

    /* Send packet. */
    sv_board_dma_send_packet(u16_i);
}

/* Function send WRITE_OK.*/
void board_dma_send_ERROR(void)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_dma_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    u8_tx_data_packet[u16_i] = ERROR; /* write_ok */
    u16_i++;/* index of next element */

/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    u8_tx_data_packet[u16_i] = u8_CRC; /* 1 bytes. */
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
    u8_tx_data_packet[0x01U] = u8_size; /* 1 bytes. */

    /* Send packet. */
    sv_board_dma_send_packet(u16_i);
}

/* Function send answer float*/
void board_dma_send_answer_float(uint16_t u16_data_id, float float_data)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_dma_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    u8_tx_data_packet[u16_i] = ANSWER; /* answer */
    u16_i++;/* index of next element */
/* Add of ID of parameters. */
    board_dma_add_u16_to_packet(&u16_i, u16_data_id);

/* Add parameters value. */
    board_dma_add_float_to_packet(&u16_i, float_data);

/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    u8_tx_data_packet[u16_i] = u8_CRC; /* 1 bytes. */
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
    u8_tx_data_packet[0x01U] = u8_size; /* 1 bytes. */

    /* Send packet. */
    sv_board_dma_send_packet(u16_i);
}

void board_dma_send_answer_int32(uint16_t u16_data_id, int32_t i32_data)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_dma_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    u8_tx_data_packet[u16_i] = 0x02U; /* answer */
    u16_i++;/* index of next element */
/* Add of ID of parameters. */
    board_dma_add_u16_to_packet(&u16_i, u16_data_id);

/* Add parameters value. */
    board_dma_add_i32_to_packet(&u16_i, i32_data);
/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    u8_tx_data_packet[u16_i] = u8_CRC; /* 1 bytes. */
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
    u8_tx_data_packet[0x01U] = u8_size; /* 1 bytes. */

    /* Send packet. */
    sv_board_dma_send_packet(u16_i);
}

/* Function send u64 answer. */
void board_dma_send_answer_uint64(uint16_t u16_data_id, uint64_t u64_data)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_dma_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    u8_tx_data_packet[u16_i] = 0x02U; /* answer */
    u16_i++;/* index of next element */
/* Add of ID of parameters. */
    board_dma_add_u16_to_packet(&u16_i, u16_data_id);

/* Add parameters value. */
    board_dma_add_u64_to_packet(&u16_i, u64_data);
/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    u8_tx_data_packet[u16_i] = u8_CRC; /* 1 bytes. */
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
    u8_tx_data_packet[0x01U] = u8_size; /* 1 bytes. */

    /* Send packet. */
    sv_board_dma_send_packet(u16_i);
}

void board_dma_send_answer_quaternion(uint16_t u16_data_id, BOARD_QUAT  bq_data)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_dma_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    u8_tx_data_packet[u16_i] = 0x02U; /* answer */
    u16_i++;/* index of next element */
/* Add of ID of parameters. */
    board_dma_add_u16_to_packet(&u16_i, u16_data_id);

/* Add parameters value. */
    board_dma_add_float_to_packet(&u16_i, bq_data.fl_q0);
    board_dma_add_float_to_packet(&u16_i, bq_data.fl_q1);
    board_dma_add_float_to_packet(&u16_i, bq_data.fl_q2);
    board_dma_add_float_to_packet(&u16_i, bq_data.fl_q3);

/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    u8_tx_data_packet[u16_i] = u8_CRC; /* 1 bytes. */
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
    u8_tx_data_packet[0x01U] = u8_size; /* 1 bytes. */

    /* Send packet. */
    sv_board_dma_send_packet(u16_i);
}

void board_dma_send_answer_float_vector3d(uint16_t u16_data_id, BOARD_FLOAT_3X_DATA fv3d_data)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_dma_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    u8_tx_data_packet[u16_i] = 0x02U; /* answer */
    u16_i++;/* index of next element */
/* Add of ID of parameters. */
    board_dma_add_u16_to_packet(&u16_i, u16_data_id);

/* Add parameters value. */
    board_dma_add_float_to_packet(&u16_i, fv3d_data.fl_X);
    board_dma_add_float_to_packet(&u16_i, fv3d_data.fl_Y);
    board_dma_add_float_to_packet(&u16_i, fv3d_data.fl_Z);

/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    u8_tx_data_packet[u16_i] = u8_CRC; /* 1 bytes. */
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
    u8_tx_data_packet[0x01U] = u8_size; /* 1 bytes. */

    /* Send packet. */
    sv_board_dma_send_packet(u16_i);
}





/* This function add to u8_tx_data_packet array u16 value and increment index. */
static void board_dma_add_u16_to_packet(uint16_t *pu16_i, uint16_t u16_value)
{
    uint16_t u16_index;

    u16_index = *pu16_i;
    u8_tx_data_packet[u16_index] = (uint8_t)(  u16_value & 0xFFU);
    u16_index++;
    u8_tx_data_packet[u16_index] = (uint8_t)( (u16_value >> 8) & 0xFFU);
    u16_index++;

    *pu16_i = u16_index;
}

#if 0
/* This function add to u8_tx_data_packet array a BOARD_I16_3X_DATA value and increment index. */
static void board_dma_add_bi163x_to_packet(uint16_t *pu16_i, BOARD_I16_3X_DATA bi163x_value)
{
    board_dma_add_u16_to_packet(pu16_i, (uint16_t)bi163x_value.i16_X);
    board_dma_add_u16_to_packet(pu16_i, (uint16_t)bi163x_value.i16_Y);
    board_dma_add_u16_to_packet(pu16_i, (uint16_t)bi163x_value.i16_Z);
}

/* This function add to u8_tx_data_packet array a BOARD_FLOAT_3X_DATA value and increment index. */
static void board_dma_add_b_float3d_to_packet(uint16_t *pu16_i, BOARD_FLOAT_3X_DATA b_float3d_value)
{
    board_dma_add_float_to_packet(pu16_i, b_float3d_value.fl_X);
    board_dma_add_float_to_packet(pu16_i, b_float3d_value.fl_Y);
    board_dma_add_float_to_packet(pu16_i, b_float3d_value.fl_Z);
}
#endif

/* This function add to u8_tx_data_packet array a float value and increment index. */
static void board_dma_add_float_to_packet(uint16_t *pu16_i, float f_value)
{
    uint32_t u32_i;
    u32_i = *(uint32_t*)((void*)&f_value);
    board_dma_add_u16_to_packet(pu16_i, (uint16_t)u32_i);
    board_dma_add_u16_to_packet(pu16_i, (uint16_t)(u32_i >> 16));
}

/* This function add to u8_tx_data_packet array a float value and increment index. */
static void board_dma_add_i32_to_packet(uint16_t *pu16_i, int32_t i32_value)
{
    uint32_t u32_i;
    u32_i = *(uint32_t*)((void*)&i32_value);
    board_dma_add_u16_to_packet(pu16_i, (uint16_t)u32_i);
    board_dma_add_u16_to_packet(pu16_i, (uint16_t)(u32_i >> 16));
}


/* This function add HEAD to u8_tx_data_packet array and increment index. */
static void board_dma_add_head_of_tx_packet(uint16_t *pu16_i)
{
    uint16_t u16_index;

    /* First index of packet always is zero. */
    *pu16_i = 0U;
    u16_index = *pu16_i;
    /* Create head structure. 0x73 is header. */
    u8_tx_data_packet[u16_index] = 0x73U;
    u16_index++;
    *pu16_i = u16_index;
}

/* This function add u64 system time to tx packet. */
static void board_dma_add_u64_to_packet(uint16_t *pu16_i, uint64_t u64_data)
{
    uint16_t u16_index;
    u16_index = *pu16_i;

    /* Add system time to tx packet. */
    u8_tx_data_packet[u16_index] = (uint8_t)( u64_data&0xFFU);
    u16_index++;
    u8_tx_data_packet[u16_index] = (uint8_t)((u64_data >> 8)&0xFFU);
    u16_index++;
    u8_tx_data_packet[u16_index] = (uint8_t)((u64_data >> 16)&0xFFU);
    u16_index++;
    u8_tx_data_packet[u16_index] = (uint8_t)((u64_data >> 24)&0xFFU);
    u16_index++;
    u8_tx_data_packet[u16_index] = (uint8_t)((u64_data >> 32)&0xFFU);
    u16_index++;
    u8_tx_data_packet[u16_index] = (uint8_t)((u64_data >> 40)&0xFFU);
    u16_index++;
    u8_tx_data_packet[u16_index] = (uint8_t)((u64_data >> 48)&0xFFU);
    u16_index++;
    u8_tx_data_packet[u16_index] = (uint8_t)((u64_data >> 56)&0xFFU);
    u16_index++;
    *pu16_i = u16_index;
}
