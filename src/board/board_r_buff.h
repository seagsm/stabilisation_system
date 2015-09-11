#ifndef BOARD_R_BUFF_H
#define BOARD_R_BUFF_H 1

#include "stm32f10x.h"
#include "board_system_type.h"

#define TX_USART1_SIZE  256U
#define RX_USART1_SIZE  128U

#define TX_USART3_SIZE  128U
#define RX_USART3_SIZE  512U

typedef struct
{
	uint16_t	gu16_r_buffer_head;	/* Index of head of round buffer */
	uint16_t 	gu16_r_buffer_tail; 	/* Index of tail of round buffer */
	uint16_t 	gu16_r_buffer_size; 	/* Size of data stored in round buffer*/
	uint16_t 	gu16_r_sizeof_buffer;	/* Size of buffer. */
}BOARD_ROUND_BUFFER_STRUCTURE;

static BOARD_ERROR be_board_r_buff_structure_init(BOARD_ROUND_BUFFER_STRUCTURE *prb_round_buffer, uint16_t u16_size_of_buffer);

BOARD_ERROR be_board_r_buff_USARTx_init(USART_TypeDef*  USARTx);

BOARD_ERROR be_board_r_buff_USART1_TX_Put_byte(uint8_t u8_byte);/* Should be replased by function below */
BOARD_ERROR be_board_r_buff_USART1_TX_Get_byte(uint8_t *u8_byte);/* Should be replased by function below */

BOARD_ERROR be_board_r_buff_USARTx_TX_PUT_byte( USART_TypeDef*  USARTx, uint8_t u8_byte);
static BOARD_ERROR be_board_r_buff_USARTx_PUT_byte_to_buffer_TX(uint8_t r_buff[], BOARD_ROUND_BUFFER_STRUCTURE *prb_TX_r_buffer, uint8_t u8_byte);
BOARD_ERROR be_board_r_buff_USARTx_TX_GET_byte( USART_TypeDef*  USARTx, uint8_t *u8_byte);
static BOARD_ERROR be_board_r_buff_USARTx_TX_buffer_GET_byte(uint8_t r_buff[], BOARD_ROUND_BUFFER_STRUCTURE *prb_TX_r_buffer, uint8_t *u8_byte);

BOARD_ERROR be_board_r_buff_USART1_RX_Put_byte(uint8_t u8_byte);

BOARD_ERROR be_board_r_buff_USARTx_RX_PUT_byte( USART_TypeDef*  USARTx, uint8_t u8_byte);
BOARD_ERROR be_board_r_buff_USARTx_tail_eat_RX_PUT_byte( USART_TypeDef*  USARTx, uint8_t u8_byte);
static BOARD_ERROR be_board_r_buff_USARTx_PUT_to_RX(uint8_t r_buff[], BOARD_ROUND_BUFFER_STRUCTURE *prb_RX_r_buffer, uint8_t u8_byte);
static BOARD_ERROR be_board_r_buff_tail_eat_USARTx_PUT_to_RX(uint8_t r_buff[], BOARD_ROUND_BUFFER_STRUCTURE *prb_RX_r_buffer, uint8_t u8_byte);

BOARD_ERROR be_board_r_buff_USARTx_RX_GET_byte( USART_TypeDef*  USARTx, uint8_t *u8_byte);
static BOARD_ERROR be_board_r_buff_USARTx_RX_buffer_GET_byte(uint8_t u8_r_buff[], BOARD_ROUND_BUFFER_STRUCTURE *prb_RX_r_buffer, uint8_t *u8_byte);

BOARD_ERROR be_board_r_buff_USART1_RX_Get_byte(uint8_t *u8_byte);
BOARD_ERROR be_board_r_buff_tail_eat_USART1_RX_Put_byte(uint8_t u8_byte);

uint16_t u16_board_r_buff_USART1_RX_tail_get_buffer(void);
void       v_board_r_buff_USART1_RX_tail_buffer_set(uint16_t u16_tail);
uint16_t u16_board_r_buff_USART1_RX_size_buffer_get(void);
void       v_board_r_buff_USART1_RX_size_buffer_set(uint16_t u16_size);

BOARD_ERROR be_board_r_USARTx_RX_get_tail_buffer( USART_TypeDef*  USARTx, uint16_t *u16_value);
BOARD_ERROR be_board_r_USARTx_RX_set_tail_buffer( USART_TypeDef*  USARTx, uint16_t u16_tail);
BOARD_ERROR be_board_r_USARTx_RX_get_size_buffer( USART_TypeDef*  USARTx, uint16_t *u16_size);
BOARD_ERROR be_board_r_USARTx_RX_set_size_buffer( USART_TypeDef*  USARTx, uint16_t u16_size);

#endif
