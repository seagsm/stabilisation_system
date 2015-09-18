
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


BOARD_ERROR  api_gps_nav_course_to_target(GPS_POSITION_DATA gpd_current_wp, GPS_POSITION_DATA gpd_target_wp, double *dbl_course, double *dbl_distance)
{
    BOARD_ERROR be_result    = BOARD_ERR_OK;
    
    double rad = 6372795.0; /* #pi - M_PI pi, rad - Earthe radius */ 

    double lat1  = 0.0;
    double long1 = 0.0;
    double lat2  = 0.0;
    double long2 = 0.0;
    
    double llat1  = 0.0;  
    double llong1 = 0.0;
    double llat2  = 0.0;  
    double llong2 = 0.0;

    double cl1 = 0.0;
    double cl2 = 0.0;
    double sl1 = 0.0;
    double sl2 = 0.0;
    double delta = 0.0;
    double cdelta = 0.0;
    double sdelta = 0.0;
    
    double y = 0.0;
    double x = 0.0;
    float x1 = 0.0f;
    float x2 = 0.0f;
    double z = 0.0;
    double z2 = 0.0;
    double ad = 0.0;
    double anglerad2 = 0.0;
    double dbl_tmp = 0.0;

    

    
    /* coordinate from */
    llat1  = (double)gpd_current_wp.fl_latitude;
    llong1 = (double)gpd_current_wp.fl_longitude;
    /* coordinate to */
    llat2  = (double)gpd_target_wp.fl_latitude;
    llong2 = (double)gpd_target_wp.fl_longitude;
 
    /* convert to radianes */
    lat1  = llat1  * _M_PI / 180.0;
    lat2  = llat2  * _M_PI / 180.0;
    long1 = llong1 * _M_PI / 180.0;
    long2 = llong2 * _M_PI / 180.0;
 
 /* Make calculation of sin and cosine */
    cl1 = cos(lat1);
    cl2 = cos(lat2);
    sl1 = sin(lat1);
    sl2 = sin(lat2);
    delta = long2 - long1;
    cdelta = cos(delta);
    sdelta = sin(delta);
 
 /* Calculation of radius of big circle */
 /* y = math.sqrt(math.pow(cl2*sdelta,2)+math.pow(cl1*sl2-sl1*cl2*cdelta,2)) */
 
    dbl_tmp = pow(cl2 * sdelta, 2.0) + pow( (cl1 * sl2) - (sl1 * cl2 * cdelta), 2.0);
    y = sqrt(dbl_tmp);
    x = (sl1 * sl2) + (cl1 * cl2 * cdelta);
    ad = atan2(y,x);
    
    *dbl_distance = ad * rad;
 
 /* Calculation of start asimut */
    
   /* x = (cl1 * sl2) - (sl1 * cl2 * cdelta); */
    x1 = ((float)cl1 * (float)sl2);
    x2 = ((float)sl1 * (float)cl2 * (float)cdelta);
    x  =( (double)x1 - (double)x2);
    
    x = (cl1 * sl2) - (sl1 * cl2 * cdelta);
    
    
    
    y = sdelta * cl2;
    
    z = atan2(-y,x);
    z = z * 180.0 / _M_PI;
   
    if (x < 0.0)
    {
        z = z+180.0;
    }
    
    /* z2 = (z + 180.0) % 360.0 - 180.0; */
    z2 = fmod((z + 180.0), 360.0);
    z2 = z2 - 180.0;
    
    
    z2 = -(z2) * _M_PI / 180.0;
     
    /* anglerad2 = z2 - ( (2 *_M_PI) * floor(( z2 / (2 * _M_PI ) ) )); */
    dbl_tmp = floor( ( z2 / (2.0 * _M_PI ) ) );
    
    anglerad2 = z2 - ( (2.0 *_M_PI) * dbl_tmp );
    
    *dbl_course = (anglerad2 * 180.0)/_M_PI;
 
    return(be_result);
}





















