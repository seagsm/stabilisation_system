#include "board_gpio.h"

/* This function should set direction and mode for all pin working like GPIO. */
BOARD_ERROR be_board_gpio_init(void)
{
  BOARD_ERROR be_result = BOARD_ERR_OK;

  /* PB1 connected to Blue LED on board. This pin has not external board connection. */
  be_result = be_board_pin_init( GPIOB, GPIO_Pin_1, GPIO_Speed_10MHz,GPIO_Mode_Out_PP);
  return(be_result);
}

/*
  This function is setting GPIO pin on required mode and direction.
  GPIO_Mode_AIN             — analog input;
  GPIO_Mode_IN_FLOATING     — digital input, Z-state;
  GPIO_Mode_IPD             — digital input, pull-down;
  GPIO_Mode_IPU             — digital input, pull-up;
  GPIO_Mode_Out_OD          — output open drain;
  GPIO_Mode_Out_PP          — output push-pull;
  GPIO_Mode_AF_OD           - alternate function open-drain;
  GPIO_Mode_AF_PP           - alternate function push-pull;
*/
static BOARD_ERROR be_board_pin_init(
                                GPIO_TypeDef* gpio_board_port,
                                uint16_t u16_port_pin,
                                GPIOSpeed_TypeDef gpio_speed_pin_speed,
                                GPIOMode_TypeDef gpio_mode_pin_mode
                              )
{
  BOARD_ERROR be_result = BOARD_ERR_OK;
  /* Create structure for initialisation. */
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable PORT Periph clock. */
  if(gpio_board_port == GPIOA)
  {
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE);
  }
  else if(gpio_board_port == GPIOB)
  {
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);
  }
  else if(gpio_board_port == GPIOC)
  {
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE);
  }
  else
  {
    be_result = BOARD_ERR_ERROR;
  }
  /* Configurate the GPIO pin structure. */
  GPIO_InitStructure.GPIO_Pin   = u16_port_pin;
  GPIO_InitStructure.GPIO_Speed = gpio_speed_pin_speed;
  GPIO_InitStructure.GPIO_Mode  = gpio_mode_pin_mode;
  /* Call GPIO init function. */
  GPIO_Init( gpio_board_port, &GPIO_InitStructure);
  return(be_result);
}
