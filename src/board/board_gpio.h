#ifndef BOARD_GPIO_H
#define BOARD_GPIO_H 1



#include "stm32f10x.h"
#include "board_system_type.h"

BOARD_ERROR be_board_gpio_init(void);
BOARD_ERROR be_board_pin_init(
                                GPIO_TypeDef* gpio_board_port,
                                uint16_t u16_port_pin,
                                GPIOSpeed_TypeDef gpio_speed_pin_speed,
                                GPIOMode_TypeDef gpio_mode_pin_mode
                              );


#endif