#ifndef BOARD_UART_H
#define BOARD_UART_H 1



#include "stm32f10x.h"
#include "board_system_type.h"


/**
 * @brief Definition for COM port1, connected to USART1
 */
#define BOARD_COM1                        USART1
#define BOARD_COM1_CLK                    RCC_APB2Periph_USART1
#define BOARD_COM1_TX_PIN                 GPIO_Pin_9
#define BOARD_COM1_TX_GPIO_PORT           GPIOA
#define BOARD_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define BOARD_COM1_RX_PIN                 GPIO_Pin_10
#define BOARD_COM1_RX_GPIO_PORT           GPIOA
#define BOARD_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define BOARD_COM1_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port2, connected to USART2
 */
#define BOARD_COM2                        USART2
#define BOARD_COM2_CLK                    RCC_APB1Periph_USART2
#define BOARD_COM2_TX_PIN                 GPIO_Pin_2
#define BOARD_COM2_TX_GPIO_PORT           GPIOA
#define BOARD_COM2_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define BOARD_COM2_RX_PIN                 GPIO_Pin_3
#define BOARD_COM2_RX_GPIO_PORT           GPIOA
#define BOARD_COM2_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define BOARD_COM2_IRQn                   USART2_IRQn

/**
 * @brief Definition for COM port3, connected to USART3
 */
#define BOARD_COM3                        USART3
#define BOARD_COM3_CLK                    RCC_APB1Periph_USART3
#define BOARD_COM3_TX_PIN                 GPIO_Pin_10
#define BOARD_COM3_TX_GPIO_PORT           GPIOB
#define BOARD_COM3_TX_GPIO_CLK            RCC_APB2Periph_GPIOB
#define BOARD_COM3_RX_PIN                 GPIO_Pin_11
#define BOARD_COM3_RX_GPIO_PORT           GPIOB
#define BOARD_COM3_RX_GPIO_CLK            RCC_APB2Periph_GPIOB
#define BOARD_COM3_IRQn                   USART3_IRQn

/* Amount of enabled ports. */
#define COMn                             3U
 typedef enum
{
  COM1 = 0U,
  COM2 = 1U,
  COM3 = 2U
} COM_TypeDef;

/* Before use printf a suitable com port should be initialised. */
#define TERMINAL_OUTPUT COM1
#define COM1_BAUD_RATE  115200U
#define GPS_SERIAL_PORT COM3
#define COM3_BAUD_RATE  9600U




BOARD_ERROR be_board_uart_init(void);
BOARD_ERROR be_board_uart_module_init(COM_TypeDef com_com, USART_InitTypeDef* p_usart_init_struct);
BOARD_ERROR be_board_uart_uart1_init(void);


#endif