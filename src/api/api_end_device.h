#ifndef API_END_DEVICE_H
#define API_END_DEVICE_H 1

#include "stm32f10x.h"
#include "board_system_type.h"

#include "api_pid.h"
#include "board_ppm.h"
#include "board_pwm.h"
#include "api_common.h"
#include "api_baro.h"

#define API_END_DEVICE_MIN_THROTTLE 1200

#define API_END_DEVICE_HEAD_HOLD_MODE_ON 1370

#define API_END_DEVICE_HEAD_HOLD_P_PID    20
#define API_END_DEVICE_HEAD_HOLDING        1 /* On / Off head holding mode. */
void api_end_device_update(void);







#endif