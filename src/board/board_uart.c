

#include "board_uart.h"


/*
    UART initialisation. UART can be used in two way. Fist of them is using interrupt,
    second is using DMA+interrupt.
*/


/* Initialisation part. */
static USART_TypeDef* COM_USART[COMn]       = {BOARD_COM1,              BOARD_COM2,              BOARD_COM3};
static GPIO_TypeDef* COM_TX_PORT[COMn]      = {BOARD_COM1_TX_GPIO_PORT, BOARD_COM2_TX_GPIO_PORT, BOARD_COM3_TX_GPIO_PORT};
static GPIO_TypeDef* COM_RX_PORT[COMn]      = {BOARD_COM1_RX_GPIO_PORT, BOARD_COM2_RX_GPIO_PORT, BOARD_COM3_RX_GPIO_PORT};

static const uint32_t COM_USART_CLK[COMn]   = {BOARD_COM1_CLK,          BOARD_COM2_CLK,          BOARD_COM3_CLK};
static const uint32_t COM_TX_PORT_CLK[COMn] = {BOARD_COM1_TX_GPIO_CLK,  BOARD_COM2_TX_GPIO_CLK,  BOARD_COM3_TX_GPIO_CLK};
static const uint32_t COM_RX_PORT_CLK[COMn] = {BOARD_COM1_RX_GPIO_CLK,  BOARD_COM2_RX_GPIO_CLK,  BOARD_COM3_RX_GPIO_CLK};
static const uint16_t COM_TX_PIN[COMn]      = {BOARD_COM1_TX_PIN,       BOARD_COM2_TX_PIN,       BOARD_COM3_TX_PIN};
static const uint16_t COM_RX_PIN[COMn]      = {BOARD_COM1_RX_PIN,       BOARD_COM2_RX_PIN,       BOARD_COM3_RX_PIN};

/* This function should initialise all uart devices. */
BOARD_ERROR be_board_uart_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Initialisation of UART1, communication interface. */
    be_result |= be_board_uart_uart1_init();
    /* Initialisation of UART3, GPS interface. */
    /* be_result |= be_board_uart_uart3_init();*/
    return(be_result);
}

/* This function do initialisation of UART1 module. */
static BOARD_ERROR be_board_uart_uart1_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* UART variable structure. */
    USART_InitTypeDef usart_init_uart;
    /* Nested vector interrupt controller structure. */
    /* NVIC_InitTypeDef NVIC_InitStructure;*/

    /* Setup uart module parameters. */
    usart_init_uart.USART_BaudRate   = COM1_BAUD_RATE;
    usart_init_uart.USART_WordLength = USART_WordLength_8b;
    usart_init_uart.USART_StopBits   = USART_StopBits_1;
    usart_init_uart.USART_Parity     = USART_Parity_No;
    usart_init_uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init_uart.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;

    /* Initialise and enable UART module. */
    be_result = be_board_uart_module_init(COM1, &usart_init_uart);
    /*TODO: somethere here should be added INTERRUPT and/or DMA initialisation. */

    return(be_result);
}



/*
  This function configurate communication port.
  COM:  Specifies the COM port to be configured.
        This parameter can be one of following parameters:
        COM1, COM2, COM3.
  USART_InitStruct: pointer to a USART_InitTypeDef structure that
                    contains the configuration information for the
                    specified USART peripheral.
 */
static BOARD_ERROR be_board_uart_module_init(COM_TypeDef com_com, USART_InitTypeDef* p_usart_init_struct)
{
  BOARD_ERROR be_result = BOARD_ERR_OK;
  GPIO_InitTypeDef gpio_init_structure;

  /* GPIO module clock turn ON. */
  RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[com_com] | COM_RX_PORT_CLK[com_com] | RCC_APB2Periph_AFIO, ENABLE);

  /* UART module clock turn ON. */
  if (com_com == COM1)
  {
    /* RCC_APB1Periph_USART1 is on RCC_APB2PeriphClockCmd. */
    RCC_APB2PeriphClockCmd(COM_USART_CLK[com_com], ENABLE);
  }
  else
  {
    /* RCC_APB1Periph_USART2, RCC_APB1Periph_USART3 is on RCC_APB1PeriphClockCmd. */
    RCC_APB1PeriphClockCmd(COM_USART_CLK[com_com], ENABLE);
  }

  /* Configure USART Tx as alternate function push-pull. */
  gpio_init_structure.GPIO_Mode  = GPIO_Mode_AF_PP;
  gpio_init_structure.GPIO_Pin   = COM_TX_PIN[com_com];
  gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COM_TX_PORT[com_com], &gpio_init_structure);

  /* Configure USART Rx as input floating. */
  gpio_init_structure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  gpio_init_structure.GPIO_Pin   = COM_RX_PIN[com_com];
  GPIO_Init(COM_RX_PORT[com_com], &gpio_init_structure);

  /* USART configuration. */
  USART_Init(COM_USART[com_com], p_usart_init_struct);

  /* Enable USART. */
  USART_Cmd(COM_USART[com_com], ENABLE);

  return (be_result);
}
