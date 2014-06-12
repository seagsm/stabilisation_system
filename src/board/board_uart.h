#ifndef BOARD_UART_H
#define BOARD_UART_H 1



#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_dma.h"


/**
 * Definition for USART1.
 */
#define BOARD_USART1_TX_GPIO_PORT       GPIOA
#define BOARD_USART1_TX_PIN             GPIO_Pin_9
#define BOARD_USART1_RX_GPIO_PORT       GPIOA
#define BOARD_USART1_RX_PIN             GPIO_Pin_10
#define BOARD_USART1_IRQn               USART1_IRQn

/**
 * Definition for USART3
 */
#define BOARD_USART3_TX_GPIO_PORT       GPIOB
#define BOARD_USART3_TX_PIN             GPIO_Pin_10
#define BOARD_USART3_RX_GPIO_PORT       GPIOB
#define BOARD_USART3_RX_PIN             GPIO_Pin_11
#define BOARD_USART3_IRQn               USART3_IRQn


/* Before use printf a suitable com port should be initialised. */
#define TERMINAL_OUTPUT                 USART1
#define USART1_BAUD_RATE                115200U

#define GPS_SERIAL_PORT                 USART3
#define USART3_BAUD_RATE                9600U




BOARD_ERROR be_board_uart_init(void);
static BOARD_ERROR be_board_uart_uart1_init(void);
static BOARD_ERROR be_board_uart_uart3_init(void);
static BOARD_ERROR be_board_uart_module_init(
                                                USART_TypeDef*  USARTx,
                                                USART_InitTypeDef* p_usart_init_struct,
                                                GPIO_TypeDef* pg_gpio_port_tx,
                                                uint16_t      u16_gpio_pin_tx,
                                                GPIO_TypeDef* pg_gpio_port_rx,
                                                uint16_t      u16_gpio_pin_rx
                                             );



#endif