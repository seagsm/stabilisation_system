#ifndef BOARD_SYSTEM_CONFIG_H
#define BOARD_SYSTEM_CONFIG_H 1



#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_NVIC.h"


#define BOARD_SYSTEM_CONFIG_DEVICE_MODE                 2

#if (BOARD_SYSTEM_CONFIG_DEVICE_MODE == 0)
    #define BOARD_SYSTEM_CONFIG_TRICOPTER_MODE          1
    #define BOARD_SYSTEM_CONFIG_FLIGHT_SAURCER_MODE0    0
    #define BOARD_SYSTEM_CONFIG_AERO_0_MODE             0

#endif

#if (BOARD_SYSTEM_CONFIG_DEVICE_MODE == 1)
    #define BOARD_SYSTEM_CONFIG_TRICOPTER_MODE          0
    #define BOARD_SYSTEM_CONFIG_FLIGHT_SAURCER_MODE     1
    #define BOARD_SYSTEM_CONFIG_AERO_0_MODE             0

#endif

#if (BOARD_SYSTEM_CONFIG_DEVICE_MODE == 2)
    #define BOARD_SYSTEM_CONFIG_TRICOPTER_MODE          0
    #define BOARD_SYSTEM_CONFIG_FLIGHT_SAURCER_MODE     0
    #define BOARD_SYSTEM_CONFIG_AERO_0_MODE             1
#endif




#endif