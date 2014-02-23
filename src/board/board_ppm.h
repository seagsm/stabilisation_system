#ifndef BOARD_PPM_H
#define BOARD_PPM_H 1



#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_NVIC.h"

#define PPM_TIM4_CH3    3U

#define BOARD_PPM_ZERO_VALUE 1500
#define BOARD_PPM_MIN_VALUE  1000
#define BOARD_PPM_MAX_VALUE  2000
extern BOARD_CHANNEL_VALUE bc_channel_value_structure;
BOARD_ERROR be_board_ppm_init(void);
void TIM4_IRQHandler(void);

static BOARD_ERROR be_TIMER4_CAPTURE_channel_init(
                                            uint16_t u16_ch_number,
                                            uint16_t TIM_TIxExternalCLKSource,
                                            uint16_t TIM_ICPolarity,
                                            uint16_t TIM_ICPSC_divider,
                                            uint16_t ICFilter
                                          );




#endif