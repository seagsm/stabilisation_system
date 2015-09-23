#ifndef API_GPS_NAV_H
#define API_GPS_NAV_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "api_common.h"
#include "board_gps.h"
#include "math.h"

#define GPS_MAX_WP_VALUE       50U








BOARD_ERROR api_gps_nav_pid_initialisation(void);
BOARD_ERROR api_gps_nav_pid(float fl_err, GPS_PID *gp_gps_pid);

BOARD_ERROR api_gps_nav_ubl_float_converter(GPS_NAVIGATION_DATA *gnd_nav_data, GPS_POSITION_DATA *gpd_gps_data);

BOARD_ERROR api_gps_nav_set_wp(GPS_POSITION_DATA gpd_gps_data,  uint32_t u32_WP_number);
BOARD_ERROR api_gps_nav_get_wp(GPS_POSITION_DATA *gpd_gps_data, uint32_t u32_WP_number);
BOARD_ERROR api_gps_nav_course_to_target(GPS_POSITION_DATA gpd_current_wp, GPS_POSITION_DATA gpd_target_wp, float *fl_course, float *fl_distance);

BOARD_ERROR  api_gps_nav_test(void);

BOARD_ERROR api_gps_nav_processing(void);

#endif