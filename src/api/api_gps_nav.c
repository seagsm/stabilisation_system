
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
