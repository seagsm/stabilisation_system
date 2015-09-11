#ifndef BOARD_GPS_H
#define BOARD_GPS_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "api_nmea.h"


#define GPS_TIMEOUT 3000U  /* It is timeout for waiting of the GPS any GPS packet.*/

extern uint32_t u32_flag_GPS_on;

BOARD_ERROR be_board_gps_init(void);
static BOARD_ERROR be_board_UBLOX_gps_init(void);
static BOARD_ERROR be_board_UBLOX_gps_check(void);
static BOARD_ERROR be_board_UBLOX_gps_set_speed(void);





#endif