#include "api_cmd.h"

/* Linear buffer. */
static  uint8_t  u8_value_buffer[API_CMD_MAX_SIZE_OF_LINE_BUFFER];

void api_cmd_reading_packet(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint16_t u16_save_tail_index;
    uint16_t u16_save_size;
    uint8_t  u8_read_byte;
    uint8_t  u8_CRC = 0U;
    uint8_t  u8_flag = 0U;
    uint8_t  u8_i    = 0U;
    uint8_t  u8_packet_size = 0U;


    /* Try to found packet header. */
    while(be_result == BOARD_ERR_OK )
    {
        /* Save current buffer tail index pointed to naxt byte after header of packet. */
        u16_save_tail_index = u16_board_r_buff_USART1_RX_tail_get_buffer();

        /* Save current buffer size. */
        u16_save_size = u16_board_r_buff_USART1_RX_size_buffer_get();

        /* Read byte from UART1 Rx buffer. Remember, after reading size--, tail++. */
        be_result = be_board_r_buff_USART1_RX_Get_byte(&u8_read_byte);

        /* Check for packet header. */
        if(u8_read_byte == HEAD_OF_PACKET)
        {
            /* Flag header found. */
            u8_flag = 1U;
        }
    }

   	/* Start read packet data if buffer is not empty and pointed to head of packet. */
	while(  ( (u16_board_r_buff_USART1_RX_size_buffer_get() > 0U) || (u8_flag == 4U) )  && (u8_flag != 0U))
	{
		switch (u8_flag)
		{
            case 1: /* Read and check size of data in packet. */

                /* Read size of data in packet. */
                be_result = be_board_r_buff_USART1_RX_Get_byte(&u8_read_byte);
                /* First byte placed after header is size of packet. */
                u8_packet_size  = u8_read_byte;

                /* Check size value. */
                if(u8_packet_size >= API_CMD_MAX_SIZE_OF_LINE_BUFFER)
                {
                    /* It is wrong size. Exit from while. */
                    /* Buffer still pointed to SIZE byte. It can be head of  next packet. */

                    /* Disable state "header found". */
                    u8_flag = 0U;

                    /* Restore tail index in RX UART1 buffer. */
                    v_board_r_buff_USART1_RX_tail_buffer_set(u16_save_tail_index);

                    /* Restore size of data in RX UART1 buffer. */
                    v_board_r_buff_USART1_RX_size_buffer_set(u16_save_size);

                    /* Remove header from UART1 buffer. */
                    /* Read byte from UART1 RX buffer. Remember, after reading size--, tail++. */
                    be_result = be_board_r_buff_USART1_RX_Get_byte(&u8_read_byte);

                    be_result = BOARD_ERR_RANGE;
                }
                else
                {
                    /*Go to read data from packet. */
                    u8_flag = 2U;
                    /*Set line buffer index to start. */
                    u8_i = 0U;

                    if(u16_board_r_buff_USART1_RX_size_buffer_get() == 0U)
                    {
                        be_result = BOARD_ERR_EMPTY;
                        /* TODO: check if empty, what we should do??? */
                    }
			}
			break;

		case 2: /* Read data to line buffer.*/
			be_result = be_board_r_buff_USART1_RX_Get_byte(&u8_read_byte);

			u8_value_buffer[u8_i] = u8_read_byte;
			u8_i++;
            if(u8_i >= u8_packet_size)
            {
            	u8_flag = 3U;
            }

			if(u16_board_r_buff_USART1_RX_size_buffer_get() == 0U)
			{
				be_result = BOARD_ERR_EMPTY; /* lets wait for next parth of packet. */
			}
            break;

        case 3:
            /* Read CRC byte.*/
			be_result = be_board_r_buff_USART1_RX_Get_byte(&u8_read_byte);
			/* Here u8_i = u8_packet_size, so ,it pointed to CRC position. */
			u8_value_buffer[u8_i] = u8_read_byte; /* Save CRC */
            /* Calculating of CRC. */
            u8_CRC = su8_api_CMD_CRC8(0U, u8_packet_size);
            /* Check CRC.*/
            if(u8_value_buffer[u8_i] == u8_CRC)
            {
                be_result = BOARD_ERR_OK ;
                u8_flag = 4U;
            }
            else
            {
				/* It is wrong CRC. *//* Exit from while. *//* Buffer still pointed to SIZE byte. It can be head of  next packet. */
				u8_flag = 0U;

                /* Restore tail index in RX UART1 buffer. */
                v_board_r_buff_USART1_RX_tail_buffer_set(u16_save_tail_index);
                /* Restore size of data in RX UART1 buffer. */
                v_board_r_buff_USART1_RX_size_buffer_set(u16_save_size);
                /* Remove header from UART1 buffer. */
                /* Read byte from UART1 RX buffer. Remember, after reading size--, tail++. */
                be_result = be_board_r_buff_USART1_RX_Get_byte(&u8_read_byte);
				be_result = BOARD_ERR_RANGE;
            }
            break;
		case 4:
                /* Decoding data from packet */
                /* TODO: In line_buffer we have now packet information load. We should decode it here. */
            break;

		default:
			/* Just exit with error. */
			u8_flag = 0U;
			be_result = BOARD_ERR_RANGE;
			break;
		}
	}

}


/* This function calc CRC summ for linear buffer from board_dma.h */
static uint8_t su8_api_CMD_CRC8(uint8_t u8_start, uint8_t u8_length)
{
    uint8_t u8_crc = 0U;
    uint8_t u8_i = 0U;

    for (u8_i = u8_start; u8_i < u8_length; u8_i++)
    {
        u8_crc = u8_crc + u8_value_buffer[u8_i];
    }
    u8_crc = 0xFFU - u8_crc;
    return(u8_crc);
}









