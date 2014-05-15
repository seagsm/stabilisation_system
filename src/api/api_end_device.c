


#include "api_end_device.h"



/*This function calculate value of PPM (PWM) for all end device. (Motors, servos for tricopter configuration.) */
void api_end_device_update(void)
{
    /* Motors. */
    int32_t i32_motor[3];
    int32_t i32_throttle;
    /* Direction. */
    int32_t i32_servo;
    int32_t i32_rc_chanel_yaw_value;

    /* Angles. */
    int32_t i32_Pitch;
    int32_t i32_Roll;
    int32_t i32_Yaw;

    /* BARO */
    uint8_t u8_baro_mode = 0U;
    
    /* Copy PDF data. */
    i32_Pitch   = pid_api_pid_data[Pitch].i32_pid_output;
    i32_Roll    = pid_api_pid_data[Roll].i32_pid_output;
    i32_Yaw     = pid_api_pid_data[Yaw].i32_pid_output;

    /*
        Threshold of THROTTLE.
        For some condition without it possible problems during start of motors.
    */
    i32_throttle = (int32_t)bc_channel_value_structure.u16_channel_3_value;

    float_api_common_variables[0] = (float)i32_throttle;

    i32_throttle = i32_api_filters_ma_rx_throttle(i32_throttle);

    /* BARO ON/OFF */  /* Check the ch number. */
    if( bc_channel_value_structure.u16_channel_6_value >= API_BARO_MODE_ON)
    {
        if(u8_baro_mode == 0U)
        {  
            u8_baro_mode = 1U;
            bp_baro_pid.i32_AltHold = bp_baro_pid.i32_EstAlt;
            bp_baro_pid.i32_errorAltitudeI = 0;
            bp_baro_pid.i32_BaroPID = 0;
            bp_baro_pid.i32_ThrHold = i32_throttle;
        }
    }
    
    /* If baro stabilisation is ON. */  
    if(u8_baro_mode == 1U) 
    {   /* Change AltHold and Throttle Hold. */
        if(abs_t(i32_throttle - bp_baro_pid.i32_ThrHold) > ALT_BARO_HOLD_THROTTLE_NEUTRAL_ZONE)
        {
            u8_baro_mode = 0U;
        }  
        /* Calculate baro stabilised throuttle. */
        i32_throttle = bp_baro_pid.i32_ThrHold + bp_baro_pid.i32_BaroPID;
    }
 
    float_api_common_variables[1] = (float)i32_throttle;

    if(i32_throttle <= API_END_DEVICE_MIN_THROTTLE)
    {
        i32_motor[0] = i32_throttle;
        i32_motor[1] = i32_throttle;
        i32_motor[2] = i32_throttle;
    }
    else
    {   /* Tricopter configuration. */
        i32_motor[0] = i32_throttle + i32_Roll - (i32_Pitch * 2) / 3;
        i32_motor[1] = i32_throttle - i32_Roll - (i32_Pitch * 2) / 3;
        i32_motor[2] = i32_throttle + (i32_Pitch * 4)/3;

        i32_motor[0] = constrain_i32(i32_motor[0],API_END_DEVICE_MIN_THROTTLE,BOARD_PPM_MAX_VALUE);
        i32_motor[1] = constrain_i32(i32_motor[1],API_END_DEVICE_MIN_THROTTLE,BOARD_PPM_MAX_VALUE);
        i32_motor[2] = constrain_i32(i32_motor[2],API_END_DEVICE_MIN_THROTTLE,BOARD_PPM_MAX_VALUE);
    }

    /* Calculate value for cource stabilisation servo. */
    /* i32_servo = BOARD_PPM_ZERO_VALUE + i32_Yaw; */
    i32_rc_chanel_yaw_value = (int32_t)bc_channel_value_structure.u16_channel_1_value;
    i32_rc_chanel_yaw_value =  i32_rc_chanel_yaw_value - (int32_t)BOARD_PPM_ZERO_VALUE;

    /* i32_servo = (int32_t)bc_channel_value_structure.u16_channel_1_value + i32_Yaw; */
    i32_servo = (int32_t)BOARD_PPM_ZERO_VALUE - i32_rc_chanel_yaw_value + i32_Yaw;
    i32_servo = constrain_i32(i32_servo,BOARD_PPM_MIN_VALUE,BOARD_PPM_MAX_VALUE);

    /* Drive end devices. */
    timer2_PWM_duty_CH1((uint16_t)i32_motor[0]);
    timer2_PWM_duty_CH2((uint16_t)i32_motor[1]);
    timer2_PWM_duty_CH3((uint16_t)i32_motor[2]);
    timer2_PWM_duty_CH4((uint16_t)i32_servo);

}






