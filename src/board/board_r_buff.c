

#include "board_r_buff.h"


static uint8_t u8_board_r_buff_USART1_TX[TX_USART1_SIZE];
static uint8_t u8_board_r_buff_USART1_RX[RX_USART1_SIZE];
static BOARD_ROUND_BUFFER rb_USART1_TX;
static BOARD_ROUND_BUFFER rb_USART1_RX;



/* This function initialize USART1 TX buffer. */
BOARD_ERROR be_board_r_buff_USART1_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_board_r_buff_structure_init(&rb_USART1_TX, TX_USART1_SIZE);
    be_result = be_board_r_buff_structure_init(&rb_USART1_RX, RX_USART1_SIZE);
    return(be_result);
}

/* This function initialize buffer structure. */
static BOARD_ERROR be_board_r_buff_structure_init(BOARD_ROUND_BUFFER *prb_round_buffer, uint16_t u16_size_of_buffer)
{
	BOARD_ERROR be_result = BOARD_ERR_OK;
	prb_round_buffer->gu16_r_buffer_head = 0U;
	prb_round_buffer->gu16_r_buffer_tail = 0U;
	prb_round_buffer->gu16_r_buffer_size = 0U;
	prb_round_buffer->gu16_r_sizeof_buffer = u16_size_of_buffer;

    return(be_result);
}

/* This function put one byte to USART1 TX round buffer. If buffer is full this function return error */
BOARD_ERROR be_board_r_buff_USART1_TX_Put_byte(uint8_t u8_byte)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    if(rb_USART1_TX.gu16_r_buffer_size < rb_USART1_TX.gu16_r_sizeof_buffer)
    {
        u8_board_r_buff_USART1_TX[rb_USART1_TX.gu16_r_buffer_head] = u8_byte;
        rb_USART1_TX.gu16_r_buffer_head++;
        if(rb_USART1_TX.gu16_r_buffer_head >= rb_USART1_TX.gu16_r_sizeof_buffer)
        {
            rb_USART1_TX.gu16_r_buffer_head = 0U;
        }
        rb_USART1_TX.gu16_r_buffer_size++;
    }
    else
    {
        be_result = BOARD_ERR_FULL;
    }
    return(be_result);
}

/* This function get one byte from USART1 TX round buffer. If buffer is empty this function return error */
BOARD_ERROR be_board_r_buff_USART1_TX_Get_byte(uint8_t *u8_byte)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    if(rb_USART1_TX.gu16_r_buffer_size > 0U)
    {
        *u8_byte = u8_board_r_buff_USART1_TX[rb_USART1_TX.gu16_r_buffer_head];
        rb_USART1_TX.gu16_r_buffer_tail++;
        if(rb_USART1_TX.gu16_r_buffer_tail >= rb_USART1_TX.gu16_r_sizeof_buffer)
        {
            rb_USART1_TX.gu16_r_buffer_tail = 0U;
        }
        rb_USART1_TX.gu16_r_buffer_size--;
    }
    else
    {
        be_result = BOARD_ERR_EMPTY;
    }
    return(be_result);
}



