#ifndef API_GPS_NAV_H
#define API_GPS_NAV_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "api_common.h"
#include "math.h"

#define GPS_MAX_WP_VALUE       50U


typedef struct
{
    float   fl_latitude;
    float   fl_longitude;
    float   fl_height;
    float   fl_speed;
    float   fl_heading;

} GPS_POSITION_DATA;


typedef struct
{
    int32_t     i32_longitude;
    int32_t     i32_latitude;
    int32_t     i32_height;
    uint32_t    u32_speed;
    int32_t     i32_heading;
} GPS_NAVIGATION_DATA;

typedef struct
{
    uint8_t     u8_gpsFix;
    uint8_t     u8_flags;
    uint8_t     u8_fixStat;
    uint8_t     u8_flags2;
    uint32_t    u32_ttff;
    uint32_t    u32_msss;
} GPS_RECEIVER_STATE;




BOARD_ERROR api_gps_nav_ubl_float_converter(GPS_NAVIGATION_DATA *gnd_nav_data, GPS_POSITION_DATA *gpd_gps_data);

BOARD_ERROR api_gps_nav_set_wp(GPS_POSITION_DATA gpd_gps_data,  uint32_t u32_WP_number);
BOARD_ERROR api_gps_nav_get_wp(GPS_POSITION_DATA *gpd_gps_data, uint32_t u32_WP_number);
BOARD_ERROR  api_gps_nav_course_to_target(GPS_POSITION_DATA gpd_current_wp, GPS_POSITION_DATA gpd_target_wp, float *fl_course, float *fl_distance);




#endif