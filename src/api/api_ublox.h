#ifndef API_UBLOX_H
#define API_UBLOX_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "api_ublox_msg.h"
#include "api_led.h"




BOARD_ERROR  api_ublox_gps_init(void);
static BOARD_ERROR  api_ublox_gps_parameters_init(void);



#endif