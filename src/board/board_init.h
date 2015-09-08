#ifndef BOARD_INIT_H
#define BOARD_INIT_H 1



#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_state.h"
#include "board_gpio.h"
#include "board_uart.h"
#include "board_adc.h"
#include "board_i2c.h"
#include "board_ppm.h"
#include "board_pwm.h"
#include "board_sys_tick.h"
#include "board_gyro.h"
#include "board_acc.h"
#include "board_mag.h"
#include "board_baro.h"
#include "api_i2c_acquisition.h"
#include "api_main_loop.h"

#define BOARD_INIT_TOTAL_STEPS 12U

BOARD_ERROR be_board_init_main_init(void);




#endif