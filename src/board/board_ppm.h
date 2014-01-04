#ifndef BOARD_PPM_H
#define BOARD_PPM_H 1



#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_NVIC.h"

#define PPM_TIM4_CH3    3U

BOARD_ERROR be_board_ppm_init(void);
void TIM4_IRQHandler(void);

static BOARD_ERROR be_TIMER4_CAPTURE_channel_init(
                                            uint16_t ch_number,
                                            uint16_t TIM_TIxExternalCLKSource,
                                            uint16_t TIM_ICPolarity,
                                            uint16_t TIM_ICPSC_divider,
                                            uint16_t ICFilter
                                          );




#endif