#ifndef API_END_DEVICE_H
#define API_END_DEVICE_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "api_pid.h"
#include "board_ppm.h"
#include "board_pwm.h"
#include "api_common.h"
#include "api_baro.h"

#define API_END_DEVICE_MIN_THROTTLE      1200   /* Minimum stabilisation throuttle value. */

#define API_END_DEVICE_HEAD_HOLD_MODE_ON 1370   /* If CH value is less, HEAD_HOLD mode is ON. If CH is changed, value should be fitted. */

#define API_END_DEVICE_HEAD_HOLD_P_PID     20   /* HEAD_HOLD P_GAIN value. */

#define API_END_DEVICE_HEAD_HOLDING         1   /* On/Off head holding mode. */

#define API_END_DEVICE_TRICOPTER            0   /* define TRICOPTER mode. */

#define API_END_DEVICE_FLIGHT_SAURCER       1   /* define SAUCER mode. */

extern int32_t i32_head_hold;

void api_end_device_update(void);







#endif