#ifndef BOARD_SYSTEM_INIT_H
#define BOARD_SYSTEM_INIT_H 1



#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_ppm.h"
#include "board_state.h"
#include "board_sys_tick.h"
#include "board_pwm.h"

#define CHANNEL_1_UNLOCK_RUN    0x0470U
#define CHANNEL_2_UNLOCK_RUN    0x0470U
#define CHANNEL_3_UNLOCK_RUN    0x0470U
#define CHANNEL_4_UNLOCK_RUN    0x0730U

#define CHANNEL_1_UNLOCK_CALIBRATION    0x0470U
#define CHANNEL_2_UNLOCK_CALIBRATION    0x0710U
#define CHANNEL_3_UNLOCK_CALIBRATION    0x0710U
#define CHANNEL_4_UNLOCK_CALIBRATION    0x0710U



typedef enum
{
    BOARD_SYSTEM_INIT_LOCKED                = 0U,
    BOARD_SYSTEM_INIT_UNLOCK_RUN            = 1U,
    BOARD_SYSTEM_INIT_UNLOCK_CALIBRATION    = 2U,
}   BOARD_SYSTEM_INIT_UNLOCK_STATE; /* variable tag bl_xxx */


static BOARD_ERROR be_board_system_init_unlock(void);
BOARD_ERROR be_board_system_init_motor_calibration(void);




#endif