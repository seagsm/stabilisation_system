

#include "board_uart.h"


/*
    UART initialisation. UART can be used in two way. Fist of them is using interrupt,
    second is using DMA+interrupt.
*/


/* This function should initialise all uart devices. */
BOARD_ERROR be_board_uart_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Initialisation of UART1, communication interface. */
    be_result |= be_board_uart_uart1_init();
    be_result |= be_board_r_buff_USART1_init();
    be_result |= be_board_dma1_ch4_init();
    be_result |= be_board_dma1_ch5_init();

    /* Initialisation of UART3, GPS interface. */
    be_result |= be_board_uart_uart3_init();

    return(be_result);
}

/* This function do initialisation of UART1 module. */
static BOARD_ERROR be_board_uart_uart1_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* UART variable structure. */
    USART_InitTypeDef usart_init_uart;

    /* Setup uart module parameters. */
    usart_init_uart.USART_BaudRate   = USART1_BAUD_RATE;
    usart_init_uart.USART_WordLength = USART_WordLength_8b;
    usart_init_uart.USART_StopBits   = USART_StopBits_1;
    usart_init_uart.USART_Parity     = USART_Parity_No;
    usart_init_uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init_uart.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;

    /* Initialise and enable UART module. */
    be_result = be_board_uart_module_init(
                                            USART1,
                                            &usart_init_uart,
                                            BOARD_USART1_TX_GPIO_PORT,
                                            BOARD_USART1_TX_PIN,
                                            BOARD_USART1_RX_GPIO_PORT,
                                            BOARD_USART1_RX_PIN
                                          );
    /*TODO: somethere here should be added INTERRUPT and/or DMA initialisation. */

    return(be_result);
}

/* This function do initialisation of UART3 module. */
static BOARD_ERROR be_board_uart_uart3_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* UART variable structure. */
    USART_InitTypeDef usart_init_uart;

    /* Setup uart module parameters. */
    usart_init_uart.USART_BaudRate   = USART3_BAUD_RATE;
    usart_init_uart.USART_WordLength = USART_WordLength_8b;
    usart_init_uart.USART_StopBits   = USART_StopBits_1;
    usart_init_uart.USART_Parity     = USART_Parity_No;
    usart_init_uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init_uart.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;

    /* Initialise and enable UART module. */
    be_result = be_board_uart_module_init(
                                            USART3,
                                            &usart_init_uart,
                                            BOARD_USART3_TX_GPIO_PORT,
                                            BOARD_USART3_TX_PIN,
                                            BOARD_USART3_RX_GPIO_PORT,
                                            BOARD_USART3_RX_PIN
                                          );
    /*TODO: somethere here should be added INTERRUPT and/or DMA initialisation. */

    return(be_result);
}



/*
    This function configurate communication port.

    USART:  Specifies the USART module to be configured.
            This parameter can be one of following parameters:
            USART1, USART2, USART3.

    USART_InitStruct: pointer to a USART_InitTypeDef structure that
                    contains the configuration information for the
                    specified USART peripheral.

    GPIO_TypeDef*:  This parameter depend on USART TX port and can be one of
                    following parameters: GPIOA or GPIOB.

    uint16_t:       GPIO pin number. Depend on USART TX pin.

    GPIO_TypeDef*   This parameter depend on USART RX port and can be one of
                    following parameters: GPIOA and GPIOB.

    uint16_t        GPIO pin number. Depend on USART RX pin.
 */
static BOARD_ERROR be_board_uart_module_init(
                                                USART_TypeDef*  USARTx,
                                                USART_InitTypeDef* p_usart_init_struct,
                                                GPIO_TypeDef* pg_gpio_port_tx,
                                                uint16_t      u16_gpio_pin_tx,
                                                GPIO_TypeDef* pg_gpio_port_rx,
                                                uint16_t      u16_gpio_pin_rx
                                             )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    GPIO_InitTypeDef gpio_init_structure;

    /* UART module clocks turn ON. */
    /* RCC_APB1Periph_USART1 is on RCC_APB2PeriphClockCmd. */
    /* RCC_APB1Periph_USART2 and RCC_APB1Periph_USART3 is on RCC_APB1PeriphClockCmd. */
    if (USARTx == USART1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    }
    else if(USARTx == USART2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    }
    else if(USARTx == USART3)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    }
    else
    {
        be_result |= BOARD_ERR_ERROR;
    }

    /* Configure USART Tx as alternate function push-pull. */
    gpio_init_structure.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio_init_structure.GPIO_Pin   = u16_gpio_pin_tx;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(pg_gpio_port_tx, &gpio_init_structure);

    /* Configure USART Rx as input floating. */
    gpio_init_structure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    gpio_init_structure.GPIO_Pin   = u16_gpio_pin_rx;
    GPIO_Init(pg_gpio_port_rx, &gpio_init_structure);

    /* USART configuration. */
    USART_Init(USARTx, p_usart_init_struct);

    /* Enable USART. */
    USART_Cmd(USARTx, ENABLE);

    return (be_result);
}

