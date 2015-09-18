
#include "api_gps_nav.h"

static GPS_POSITION_DATA gpd_gps_WP_position[GPS_MAX_WP_VALUE];


BOARD_ERROR api_gps_nav_set_wp(GPS_POSITION_DATA gpd_gps_data, uint32_t u32_WP_number)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    if( u32_WP_number >= GPS_MAX_WP_VALUE )
    {
        be_result = BOARD_ERR_RANGE;
    }
    else
    {
        gpd_gps_WP_position[u32_WP_number].fl_heading   = gpd_gps_data.fl_heading;
        gpd_gps_WP_position[u32_WP_number].fl_height    = gpd_gps_data.fl_height;
        gpd_gps_WP_position[u32_WP_number].fl_latitude  = gpd_gps_data.fl_latitude;
        gpd_gps_WP_position[u32_WP_number].fl_longitude = gpd_gps_data.fl_longitude;
        gpd_gps_WP_position[u32_WP_number].fl_speed     = gpd_gps_data.fl_speed;
    }
    return(be_result);
}

BOARD_ERROR api_gps_nav_get_wp(GPS_POSITION_DATA *gpd_gps_data, uint32_t u32_WP_number)
{
    BOARD_ERROR be_result    = BOARD_ERR_OK;

    if( u32_WP_number >= GPS_MAX_WP_VALUE )
    {
        be_result = BOARD_ERR_RANGE;
    }
    else
    {
        gpd_gps_data->fl_heading   = gpd_gps_WP_position[u32_WP_number].fl_heading;
        gpd_gps_data->fl_height    = gpd_gps_WP_position[u32_WP_number].fl_height;
        gpd_gps_data->fl_latitude  = gpd_gps_WP_position[u32_WP_number].fl_latitude;
        gpd_gps_data->fl_longitude = gpd_gps_WP_position[u32_WP_number].fl_longitude;
        gpd_gps_data->fl_speed     = gpd_gps_WP_position[u32_WP_number].fl_speed;
    }

    return(be_result);
}

BOARD_ERROR api_gps_nav_ubl_float_converter(GPS_NAVIGATION_DATA *gnd_nav_data, GPS_POSITION_DATA *gpd_gps_data)
{
    BOARD_ERROR be_result    = BOARD_ERR_OK;

    gpd_gps_data->fl_heading    = ((float)(gnd_nav_data->i32_heading))  / 100000.0f;   /* degree */
    gpd_gps_data->fl_height     = ((float)(gnd_nav_data->i32_height))   / 1000.0f;     /* meters */
    gpd_gps_data->fl_latitude   = ((float)(gnd_nav_data->i32_latitude)) / 10000000.0f; /* degree.minutes */
    gpd_gps_data->fl_longitude  = ((float)(gnd_nav_data->i32_longitude))/ 10000000.0f; /* degree.minutes */
    gpd_gps_data->fl_speed      = ((float)(gnd_nav_data->u32_speed))    / 100.0f;      /* Speed in m/s */

    return(be_result);
}


BOARD_ERROR  api_gps_nav_course_to_target(GPS_POSITION_DATA gpd_current_wp, GPS_POSITION_DATA gpd_target_wp, float *fl_course, float *fl_distance)
{
    BOARD_ERROR be_result    = BOARD_ERR_OK;

    double lat1      = 0.0;
    double long1     = 0.0;
    double lat2      = 0.0;
    double long2     = 0.0;

    double delta     = 0.0;
    double sdlong    = 0.0;
    double cdlong    = 0.0;
    double slat1     = 0.0;
    double clat1     = 0.0;
    double slat2     = 0.0;
    double clat2     = 0.0;
    double denom     = 0.0;


    double dlon  = 0.0;
    double a1    = 0.0;
    double a2    = 0.0;

    lat1  = gpd_current_wp.fl_latitude;
    long1 = gpd_current_wp.fl_longitude;
    lat2  = gpd_target_wp.fl_latitude;
    long2 = gpd_target_wp.fl_longitude;

    /*
        returns distance in meters between two positions, both specified
        as signed decimal-degrees latitude and longitude. Uses great-circle
        distance computation for hypothised sphere of radius 6372795 meters.
        Because Earth is no exact sphere, rounding errors may be upto 0.5%.
    */
    /*
        returns initial course in degrees (North=0, West=270) from
        position 1 to position 2, both specified as signed decimal-degrees
        latitude and longitude.
    */

    dlon = dbl_deg2rad(long2-long1);
    delta = - dlon;
    sdlong = sin(delta);
    cdlong = cos(delta);

    lat1 = dbl_deg2rad(lat1);
    lat2 = dbl_deg2rad(lat2);
    slat1 = sin(lat1);
    clat1 = cos(lat1);
    slat2 = sin(lat2);
    clat2 = cos(lat2);

    delta = sqrt(pow(clat2 * sdlong, 2.0) + pow(clat1 * slat2 - slat1 * clat2 * cdlong, 2.0));
    denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
    delta = atan2(delta, denom);

    /* set distance */
    *fl_distance = (float)(delta * 6372795.0);

    /* Heading calculation */
    a1 = -sdlong * clat2;
    a2 = slat1 * clat2 * cdlong;
    a2 = clat1 * slat2 - a2;
    a2 = atan2(a1, a2);
    if (a2 < 0.0)
    {
        a2 += (_M_PI + _M_PI ); /* TWO_PI; */
    }

    /* set course */
    *fl_course = (float)dbl_rad2deg(a2);

    return(be_result);
}





















