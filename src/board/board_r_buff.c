

#include "board_r_buff.h"


static uint8_t u8_board_r_buff_USART1_TX[TX_USART1_SIZE];
static uint8_t u8_board_r_buff_USART1_RX[RX_USART1_SIZE];
static BOARD_ROUND_BUFFER_STRUCTURE rb_USART1_TX;
static BOARD_ROUND_BUFFER_STRUCTURE rb_USART1_RX;

static uint8_t u8_board_r_buff_USART3_TX[TX_USART3_SIZE];
static uint8_t u8_board_r_buff_USART3_RX[RX_USART3_SIZE];
static BOARD_ROUND_BUFFER_STRUCTURE rb_USART3_TX;
static BOARD_ROUND_BUFFER_STRUCTURE rb_USART3_RX;



/* This function initialize USARTx TX and RX buffer. */
BOARD_ERROR be_board_r_buff_USARTx_init(USART_TypeDef*  USARTx)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    if (USARTx == USART1)
    {
        be_result = be_board_r_buff_structure_init(&rb_USART1_TX, TX_USART1_SIZE);
        be_result = be_board_r_buff_structure_init(&rb_USART1_RX, RX_USART1_SIZE);
    }
    else if(USARTx == USART2)
    {
        be_result |= BOARD_ERR_ERROR;
    }
    else if(USARTx == USART3)
    {
        be_result = be_board_r_buff_structure_init(&rb_USART3_TX, TX_USART3_SIZE);
        be_result = be_board_r_buff_structure_init(&rb_USART3_RX, RX_USART3_SIZE);
    }
    else
    {
        be_result |= BOARD_ERR_ERROR;
    }
    return(be_result);
}

/* This function initialize buffer structure. */
static BOARD_ERROR be_board_r_buff_structure_init(BOARD_ROUND_BUFFER_STRUCTURE *prb_round_buffer, uint16_t u16_size_of_buffer)
{
	BOARD_ERROR be_result = BOARD_ERR_OK;
	prb_round_buffer->gu16_r_buffer_head = 0U;
	prb_round_buffer->gu16_r_buffer_tail = 0U;
	prb_round_buffer->gu16_r_buffer_size = 0U;
	prb_round_buffer->gu16_r_sizeof_buffer = u16_size_of_buffer;

    return(be_result);
}

/*
    This public function put one byte to USARTx TX round buffer. If buffer is full this function return error
    Input parameters are USART name and Tx byte.
*/
BOARD_ERROR be_board_r_buff_USARTx_TX_Put_byte( USART_TypeDef*  USARTx, uint8_t u8_byte)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    if (USARTx == USART1)
    {
        be_result |= be_board_r_buff_USARTx_Put_byte_to_buffer_TX(u8_board_r_buff_USART1_TX, &rb_USART1_TX, u8_byte);
    }
    else if(USARTx == USART2)
    {
        be_result |= BOARD_ERR_ERROR;
    }
    else if(USARTx == USART3)
    {
         be_result |= be_board_r_buff_USARTx_Put_byte_to_buffer_TX(u8_board_r_buff_USART3_TX, &rb_USART3_TX, u8_byte);
    }
    else
    {
        be_result |= BOARD_ERR_ERROR;
    }
    return(be_result);
}

/*
    This private function put one byte to USARTx TX round buffer. If buffer is full this function return error
    Input parameters are USART name, pointer to USARTX TX round buffer and Tx byte.
*/
static BOARD_ERROR be_board_r_buff_USARTx_Put_byte_to_buffer_TX(uint8_t r_buff[], BOARD_ROUND_BUFFER_STRUCTURE *prb_TX_r_buffer, uint8_t u8_byte)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Check if buffer is full.*/
    if(prb_TX_r_buffer->gu16_r_buffer_size < prb_TX_r_buffer->gu16_r_sizeof_buffer)
    {
        /* Copy input byte to buffer place pointed by buffer head index. */
        r_buff[prb_TX_r_buffer->gu16_r_buffer_head] = u8_byte;
        /* Increase buffer head index. */
        prb_TX_r_buffer->gu16_r_buffer_head++;
        /* Round head index. */
        if(prb_TX_r_buffer->gu16_r_buffer_head >= prb_TX_r_buffer->gu16_r_sizeof_buffer)
        {
            prb_TX_r_buffer->gu16_r_buffer_head = 0U;
        }
        /* Add data size counter. */
        prb_TX_r_buffer->gu16_r_buffer_size++;
    }
    else
    {   /* Set buffer full error. */
        be_result = BOARD_ERR_FULL;
    }
    return(be_result);
}

/* Should be rereplased by function above! This function put one byte to USART1 TX round buffer. If buffer is full this function return error */
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
        *u8_byte = u8_board_r_buff_USART1_TX[rb_USART1_TX.gu16_r_buffer_tail];
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


/* This function put one byte to USART1 RX round buffer. If buffer is full this function return error */
BOARD_ERROR be_board_r_buff_USART1_RX_Put_byte(uint8_t u8_byte)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    if(rb_USART1_RX.gu16_r_buffer_size < rb_USART1_RX.gu16_r_sizeof_buffer)
    {
        u8_board_r_buff_USART1_RX[rb_USART1_RX.gu16_r_buffer_head] = u8_byte;
        rb_USART1_RX.gu16_r_buffer_head++;
        if(rb_USART1_RX.gu16_r_buffer_head >= rb_USART1_RX.gu16_r_sizeof_buffer)
        {
            rb_USART1_RX.gu16_r_buffer_head = 0U;
        }
        rb_USART1_RX.gu16_r_buffer_size++;
    }
    else
    {
        be_result = BOARD_ERR_FULL;
    }
    return(be_result);
}

/* This function put one byte to USART1 RX round buffer. Tail eat function. */
BOARD_ERROR be_board_r_buff_tail_eat_USART1_RX_Put_byte(uint8_t u8_byte)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Save byte to buffer. */
    u8_board_r_buff_USART1_RX[rb_USART1_RX.gu16_r_buffer_head] = u8_byte;
    /* Shift head index to next buffer address. */
    rb_USART1_RX.gu16_r_buffer_head++;
    /* Round buffer address. */
    if(rb_USART1_RX.gu16_r_buffer_head >= rb_USART1_RX.gu16_r_sizeof_buffer)
    {
        rb_USART1_RX.gu16_r_buffer_head = 0U;
    }
    /* If size is maximum size, stopp add size value and set tail index same like head index. (Eat tail.) */
    if(rb_USART1_RX.gu16_r_buffer_size < rb_USART1_RX.gu16_r_sizeof_buffer)
    {
        rb_USART1_RX.gu16_r_buffer_size++;
    }
    else
    {
        rb_USART1_RX.gu16_r_buffer_tail = rb_USART1_RX.gu16_r_buffer_head;
        be_result = BOARD_ERR_OVERLOAD;
    }
    return(be_result);
}

/* This function get one byte from USART1 RX round buffer. If buffer is empty this function return error */
BOARD_ERROR be_board_r_buff_USART1_RX_Get_byte(uint8_t *u8_byte)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    if(rb_USART1_RX.gu16_r_buffer_size > 0U)
    {
        *u8_byte = u8_board_r_buff_USART1_RX[rb_USART1_RX.gu16_r_buffer_tail];
        rb_USART1_RX.gu16_r_buffer_tail++;
        if(rb_USART1_RX.gu16_r_buffer_tail >= rb_USART1_RX.gu16_r_sizeof_buffer)
        {
            rb_USART1_RX.gu16_r_buffer_tail = 0U;
        }
        rb_USART1_RX.gu16_r_buffer_size--;
    }
    else
    {
        be_result = BOARD_ERR_EMPTY;
    }
    return(be_result);
}

/* Functions get and set rx buffer structure parameters */
uint16_t u16_board_r_buff_USART1_RX_tail_get_buffer(void)
{
    return(rb_USART1_RX.gu16_r_buffer_tail);
}

void v_board_r_buff_USART1_RX_tail_buffer_set(uint16_t u16_tail)
{
    rb_USART1_RX.gu16_r_buffer_tail = u16_tail;
}

uint16_t u16_board_r_buff_USART1_RX_size_buffer_get(void)
{
    return(rb_USART1_RX.gu16_r_buffer_size);
}

void v_board_r_buff_USART1_RX_size_buffer_set(uint16_t u16_size)
{
    rb_USART1_RX.gu16_r_buffer_size = u16_size;
}












