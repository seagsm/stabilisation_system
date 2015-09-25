
#include "api_gps_nav.h"

static GPS_POSITION_DATA gpd_gps_WP_position[GPS_MAX_WP_VALUE];

static COURCE_PID cp_gps_dir_pid;
static COURCE_PID cp_gps_alt_pid;

BOARD_ERROR api_gps_nav_initialisation(void)
{
    BOARD_ERROR         be_result = BOARD_ERR_OK;

    be_result = api_gps_nav_COURCE_pid_initialisation();
    
    return(be_result);
}

BOARD_ERROR api_gps_nav_processing(void)
{
    BOARD_ERROR         be_result = BOARD_ERR_OK;
    BOARD_DEV_STATE     bds_value = BOARD_DEV_OFF;
    GPS_NAVIGATION_DATA gnd_nav_data;
    GPS_POSITION_DATA   gpd_gps_data;
    GPS_POSITION_DATA   gpd_get_wp_data;
    BOARD_DATA_STATE    bds_gps_data_ready_flag;
    COURCE_PID          cp_pid_value;
    
    float fl_course     = 0.0f;
    float fl_distance   = 0.0f;
    
    /*Get is GPS power ON status. */
    be_board_gps_get_gps_dev_state(&bds_value);

    /* If GPS is ON and work. */
    if(bds_value == BOARD_DEV_ON)
    {
        /* Copy received data to UART3 buffer. */
        be_board_dma_DMA1_CH3_buffer_copy_to_UART3_buffer();

        /* Decoding received from GPS data. */
        be_result = api_ublox_msg_input_decode(USART3);

        /* Check if new data come from GPS receiver. */
        api_ublox_msg_get_gps_data_status(&bds_gps_data_ready_flag);

        if(bds_gps_data_ready_flag == BOARD_DATA_READY)
        {
            /* Reset DATA READY FLAG. */
            api_ublox_msg_set_gps_data_status(BOARD_DATA_NOT_READY );

            /* Read fresh navigation data. */
            api_ublox_msg_get_navigation_data(&gnd_nav_data);

            /* Convert UBL nav data to real word float value. */
            api_gps_nav_ubl_float_converter(&gnd_nav_data, &gpd_gps_data);

            /* Get target WP coordinate. */
            api_gps_nav_get_wp(&gpd_get_wp_data, 0U); /* 0 is home WP */

            /* 
                Calculate cource and distance from current position to target WP.
            */
            api_gps_nav_course_to_target(gpd_gps_data, gpd_get_wp_data, &fl_course, &fl_distance);
            
            /* 
                Convert absolute cource to turn-angle relatively heading cource. 
                ( + is CW, - is CCW direction.)
            */
            fl_course = fl_course - gpd_gps_data.fl_heading;
            if(fl_course < -180.0f)
            {
                fl_course = fl_course + 360.0f;
            }  
            if(fl_course > 180.0f)
            {
                fl_course = fl_course - 360.0f;
            }  

            /* It is a place to call GPS PID controller with turn angle as input parameter. */
            /* Get COURCE PID value. */
            api_gps_nav_get_COURCE_pid_parameters(&cp_pid_value);
            /* Update PID frame. */
            api_gps_nav_pid(fl_course, &cp_pid_value);
            /* Set COURCE PID value. */  
            api_gps_nav_set_COURCE_pid_parameters(cp_pid_value);
            

#if 0 /* test only */            
            /* REMOVE IT TO RIGHT PLACE. It just send data to base station. */
            fl_api_body_angle_wind_angles[2] = -fl_course + 360.0f;

            /* temporary, just for test */
            api_baro_set_altitude_estimation(gnd_nav_data.i32_height / 100);
#endif            
            
        }

        /* Indication of received error. */
        if(be_result != BOARD_ERR_OK)
        {
            be_result = api_led_flash_set_fast_period(400000U);
        }
        else
        {
            be_result = api_led_flash_set_fast_period(100000U);
        }
    }
    else
    {
        be_result = BOARD_ERR_OFF;
    }
    return(be_result);
}

/* It add PID output to Roll wing angle to provide airplane course changing. */
BOARD_ERROR api_gps_nav_set_navigation_influence(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    COURCE_PID  cp_value;
    float fl_value;
    BOARD_DATA_STATE bds_gps_data_ready_flag;
    
    /* 
        Get gps data ready flag. 
    */
    api_ublox_msg_get_gps_data_status(&bds_gps_data_ready_flag);

    if(bds_gps_data_ready_flag == BOARD_DATA_READY)
    {
        /* Get current COURSE PID output */
        api_gps_nav_get_COURCE_pid_parameters(&cp_value);
    
        /* Get current Roll value from body WING ANGLE. */
        be_result |= be_api_body_get_angle_calculation(&fl_value, Roll);
    
        /* Add COURSE PID output error */
        fl_value = fl_value + cp_value.fl_out; 
    
        /* Set Roll value to body WING ANGLE. */
        be_result |= be_api_body_set_angle_calculation(fl_value, Roll);
    }
    
    return(be_result);
}

/*
    Function set WP to WP array:
    GPS_POSITION_DATA gpd_gps_data - WP coordinates.
    uint32_t u32_WP_number         - WP number.
*/
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

/*
    Function gett WP[x] from WP array.
*/
static BOARD_ERROR api_gps_nav_get_wp(GPS_POSITION_DATA *gpd_gps_data, uint32_t u32_WP_number)
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

/* Function convert GPS UBL RAW data to real word float value. */
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

/*
    Function calculate heading and distance between two WPs.
    Returns initial course in degrees (North=0, West=270) from
    position 1 to position 2, both specified as signed decimal-degrees
    latitude and longitude. And returns distance in meters between
    two positions, both specified as signed decimal-degrees latitude
    and longitude. Uses great-circle distance computation for
    hypothised sphere of radius 6372795 meters. Because Earth is
    no exact sphere, rounding errors may be upto 0.5%.

    GPS_POSITION_DATA gpd_current_wp - current WP( We are here.)
    GPS_POSITION_DATA gpd_target_wp  - target WP (Our destination.)
    float            *fl_course      - heading to destination.
    float            *fl_distance    - distance between two WPs.
*/
static BOARD_ERROR api_gps_nav_course_to_target(GPS_POSITION_DATA gpd_current_wp, GPS_POSITION_DATA gpd_target_wp, float *fl_course, float *fl_distance)
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

/* 
    This function using for testing HEADING calculation algorithm. 
   
*/
BOARD_ERROR  api_gps_nav_test(void)
{
    BOARD_ERROR be_result    = BOARD_ERR_OK;

    GPS_POSITION_DATA gpd_wp[5] =   {
                                        {49.226844f, 16.557175f, 0.0f, 0.0f ,0.0f}, /* WP0->WP1 328.4402°      442m  */
                                        {49.230229f, 16.553991f, 0.0f, 0.0f ,0.0f}, /* WP1->WP2 60.8299°       547m  */
                                        {49.232621f, 16.560554f, 0.0f, 0.0f ,0.0f}, /* WP2->WP3 303.9954°      908m  */
                                        {49.237174f, 16.550213f, 0.0f, 0.0f ,0.0f}, /* WP3->WP4 82.8964°       584m  */
                                        {49.237821f, 16.558168f, 0.0f, 0.0f ,0.0f}, /* WP4->WP0 183.3810°      1223m */
                                    };
    float fl_course   = 0.0f;
    float fl_distance = 0.0f;
    int i_old = 0;
    int i_new = 1;

    while(i_new < 100)
    {
        /*  dbl_course = (double)f_api_nmea_initial_course(gpd_wp[i_old].fl_latitude, gpd_wp[i_old].fl_longitude, gpd_wp[i_new].fl_latitude, gpd_wp[i_new].fl_longitude);*/
        api_gps_nav_course_to_target(gpd_wp[i_old], gpd_wp[i_new], &fl_course, &fl_distance);
        i_old++;
        i_new++;
        if(i_old >= 5)
        {
            i_old = 0;
        }
        if(i_new >= 5)
        {
            i_new = 0;
        }
    }

    return(be_result);
}

/*  
    NAVIGATION PID CONTROLLER :
    u(t) = P(t) + I(t) + D(t)
    P(t) = Cp * e(t)
    I(t) = I(t – T) + Ci * e(t)
    D(t) = Cd * ( e(t) – e(t-T) )
    Cp, Ci, Cd – coefficients of PID controller.
    T - period of PID controller.
*/
static BOARD_ERROR api_gps_nav_COURCE_pid_initialisation(void)
{
    BOARD_ERROR be_result    = BOARD_ERR_OK;
    
    COURCE_PID cp_pid_value;
    
    /* Set start value for direction PID controller. */
    /* P */
    cp_pid_value.fl_p = 2.0f;
    /* I */
    cp_pid_value.fl_i = 0.001f;
    cp_pid_value.fl_integrall = 0.0f;
    cp_pid_value.fl_integrall_max =  10.0f;
    cp_pid_value.fl_integrall_min = -10.0f;
    
    /* D */
    cp_pid_value.fl_d = 0.5f;
    cp_pid_value.fl_old_value = 0.0f;

    /* Output. */
    cp_pid_value.fl_out       =  0.0f; /* degree */
    cp_pid_value.fl_out_max   =  30.0f; /* degree */
    cp_pid_value.fl_out_min   = -30.0f; /* degree */

    be_result = api_gps_nav_set_COURCE_pid_parameters(cp_pid_value);
   
    return (be_result);
}

/* Set COURCE PID controller parameters. */
static BOARD_ERROR  api_gps_nav_set_COURCE_pid_parameters(COURCE_PID cp_value)
{
    BOARD_ERROR be_result    = BOARD_ERR_OK;

    /* Set new value to COURCE PID . */
    /* P */
    cp_gps_dir_pid.fl_p = cp_value.fl_p;
    /* I */
    cp_gps_dir_pid.fl_i = cp_value.fl_i;
    cp_gps_dir_pid.fl_integrall     = cp_value.fl_integrall;
    cp_gps_dir_pid.fl_integrall_max = cp_value.fl_integrall_max;
    cp_gps_dir_pid.fl_integrall_min = cp_value.fl_integrall_min;
    /* D */
    cp_gps_dir_pid.fl_d = cp_value.fl_d;
    cp_gps_dir_pid.fl_old_value = cp_value.fl_old_value;

    /* Output. */
    cp_gps_dir_pid.fl_out = cp_value.fl_out;
    cp_gps_dir_pid.fl_out_max = cp_value.fl_out_max;
    cp_gps_dir_pid.fl_out_min = cp_value.fl_out_min;

    return (be_result);
}

/* Get COURCE PID controller parameters. */
static BOARD_ERROR api_gps_nav_get_COURCE_pid_parameters(COURCE_PID *cp_value)
{
    BOARD_ERROR be_result    = BOARD_ERR_OK;

    /* Get current value of GPS PID . */

    /* P */
    cp_value->fl_p = cp_gps_dir_pid.fl_p;
    
    /* I */
    cp_value->fl_i = cp_gps_dir_pid.fl_i;
    cp_value->fl_integrall     = cp_gps_dir_pid.fl_integrall;
    cp_value->fl_integrall_max = cp_gps_dir_pid.fl_integrall_max;
    cp_value->fl_integrall_min = cp_gps_dir_pid.fl_integrall_min;
    
    /* D */
    cp_value->fl_d = cp_gps_dir_pid.fl_d;
    cp_value->fl_old_value     = cp_gps_dir_pid.fl_old_value;

    /* Output. */
    cp_value->fl_out           = cp_gps_dir_pid.fl_out;
    cp_value->fl_out_max       = cp_gps_dir_pid.fl_out_max;
    cp_value->fl_out_min       = cp_gps_dir_pid.fl_out_min;
    
    return (be_result);
}

/* 
    Update PID controller frame. It can be used for COURCE and for ALTITUDE.
    Input:
        float fl_err        - error angle in degree. 
        GPS_PID *gp_gps_pid - pointer to PID controller structure.
    Output is PID state saved to PID structure.
*/
static BOARD_ERROR api_gps_nav_pid(float fl_err, COURCE_PID *cp_gps_pid)
{
    BOARD_ERROR be_result    = BOARD_ERR_OK;

    float fl_proportional = 0.0f;
    float fl_integrall    = 0.0f;
    float fl_differencial = 0.0f;
    /* P */
    fl_proportional = cp_gps_pid->fl_p * fl_err;

    /* I */
    /* Calculation of integrall. */
    fl_integrall = cp_gps_pid->fl_integrall + cp_gps_pid->fl_i * fl_err;

    /* Constrain integrall value. */
    fl_integrall = fl_constrain(fl_integrall, cp_gps_pid->fl_integrall_min, cp_gps_pid->fl_integrall_max);

    /* Save new integrall value to PID structure*/
    cp_gps_pid->fl_integrall = fl_integrall;

    /* D */
    /* Calculation of differential component. */
    fl_differencial = cp_gps_pid->fl_d * (fl_err - cp_gps_pid->fl_old_value);

    /* Save current error angle value to PID structure. */
    cp_gps_pid->fl_old_value = fl_err;

    /* Output. */
    /* Calculation of output. */
    cp_gps_pid->fl_out = fl_proportional + fl_integrall + fl_differencial;

    /* Constrain output value and save it to PID structure. */
    cp_gps_pid->fl_out = fl_constrain(cp_gps_pid->fl_out, cp_gps_pid->fl_out_min, cp_gps_pid->fl_out_max);

    return (be_result);
}










