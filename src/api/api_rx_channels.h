#ifndef API_RX_CHANNELS_H
#define API_RX_CHANNELS_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "api_common.h"
#include "math.h"

#define RC_EXP_COEFF            65
#define RC_RC_RATE              70  /* 90 it is 90 degree */

int32_t api_rx_channels_approximation(int32_t i32_ch_value, int32_t i32_ch_zero);

#endif