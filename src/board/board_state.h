#ifndef BOARD_STATE_H
#define BOARD_STATE_H 1



#include "stm32f10x.h"
#include "board_system_type.h"

typedef enum
{
    BOARD_SYSTEM_INIT               = 0U,
    BOARD_SYSTEM_READY_TO_RUN       = 1U,
    BOARD_SYSTEM_RUN                = 2U,
    BOARD_SYSTEM_MOTOR_CALIBRATION  = 3U,
    BOARD_SYSTEM_FAULT              = 4U

}   BOARD_SYSTEM_STATE; /* variable tag bss_xxx */

BOARD_SYSTEM_STATE bss_board_state_get_current_state(void);
void v_board_state_update_current_state(BOARD_SYSTEM_STATE bss_set_state);

BOARD_SYSTEM_STATE bss_board_state_get_required_state(void);
void v_board_state_set_required_state(BOARD_SYSTEM_STATE  bss_set_state);




#endif