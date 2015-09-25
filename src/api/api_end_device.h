#ifndef API_END_DEVICE_H
#define API_END_DEVICE_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_system_config.h"
#include "api_pid.h"
#include "board_ppm.h"
#include "board_pwm.h"
#include "api_common.h"
#include "api_baro.h"

#include "api_end_tricopter.h"
#include "api_end_saucer.h"
#include "api_end_aero.h"

#if BOARD_SYSTEM_CONFIG_TRICOPTER_MODE
    #define API_END_DEVICE_TRICOPTER            1   /* define TRICOPTER mode. */
    #define API_END_DEVICE_FLIGHT_SAURCER       0   /* define SAUCER mode. */
    #define API_END_DEVICE_AERO_0               0   /* define AEROPLANE 0 mode. */
#endif

#if BOARD_SYSTEM_CONFIG_FLIGHT_SAURCER_MODE
    #define API_END_DEVICE_TRICOPTER            0  /* define TRICOPTER mode. */
    #define API_END_DEVICE_FLIGHT_SAURCER       1   /* define SAUCER mode. */
    #define API_END_DEVICE_AERO_0               0   /* define AEROPLANE 0 mode. */
#endif

#if BOARD_SYSTEM_CONFIG_AERO_0_MODE
    #define API_END_DEVICE_TRICOPTER            0  /* define TRICOPTER mode. */
    #define API_END_DEVICE_FLIGHT_SAURCER       0   /* define SAUCER mode. */
    #define API_END_DEVICE_AERO_0               1   /* define AEROPLANE 0 mode. */
#endif


   void api_end_device_update(void);
int32_t api_end_device_get_head_hold(void);





#endif