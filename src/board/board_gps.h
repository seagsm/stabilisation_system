#ifndef BOARD_GPS_H
#define BOARD_GPS_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "api_nmea.h"
#include "api_ublox.h"

#define GPS_TIMEOUT 3000U  /* It is timeout for waiting of the GPS any GPS packet.*/

BOARD_ERROR be_board_gps_init(void);
BOARD_ERROR be_board_gps_get_gps_dev_state(BOARD_DEV_STATE *bds_value);
static BOARD_ERROR be_board_gps_set_gps_dev_state(BOARD_DEV_STATE bds_value);
static BOARD_ERROR be_board_UBLOX_gps_init(void);
static BOARD_ERROR be_board_UBLOX_gps_check(void);


#endif