


#include "api_end_device.h"

/* Value of current HEAD HOLD direction. This value is using in PDF YAW filter. */
int32_t i32_head_hold     = 0;

/*This function calculate value of PPM (PWM) for all end device. (Motors, servos for tricopter configuration.) */
#if API_END_DEVICE_TRICOPTER
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
    static uint8_t u8_head_hold_mode = 0U;
    int32_t i32_heading              = 0;
    int32_t i32_heading_error        = 0;
    float f_head_angle               = 0.0f;

    /* BARO */
    static uint8_t u8_baro_mode = 0U;

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

    /* Turn on BaroAltHold. BARO ON/OFF */
    if( bc_channel_value_structure.u16_channel_5_value <= API_BARO_MODE_ON)
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
    else
    {
        u8_baro_mode = 0U;
    }

    /* If baro stabilisation is ON. */
    if(u8_baro_mode == 1U)
    {
        /* Change AltHold and Throttle Hold. */
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

    /* Get current RC YAW channel value. */
    i32_rc_chanel_yaw_value = (int32_t)bc_channel_value_structure.u16_channel_1_value;

    /* Calc aproximation value. */
    i32_rc_chanel_yaw_value = api_rx_channels_approximation(i32_rc_chanel_yaw_value, (int32_t)BOARD_PPM_ZERO_VALUE);

    /* Get MA filtered value. It give some small delay. */
    i32_rc_chanel_yaw_value = i32_api_filters_ma_rx_yaw(i32_rc_chanel_yaw_value);


#if API_END_DEVICE_HEAD_HOLDING
 /* HEAD HOLDING. */
    /* Check if headhold mode ON. */
    if( bc_channel_value_structure.u16_channel_6_value <= (uint16_t)API_END_DEVICE_HEAD_HOLD_MODE_ON)
    {
      if(u8_head_hold_mode == 0U)
      {
        u8_head_hold_mode = 1U;
        f_head_angle      = fl_api_body_angle_wind_angles[Yaw];

        /* Save current head position( direction). */
        i32_head_hold     = (int32_t)f_head_angle;
      }
    }
    else
    {
        u8_head_hold_mode = 0U;
    }

    /* Get current course dirrection. */
    f_head_angle = fl_api_body_angle_wind_angles[Yaw];
    i32_heading  = (int32_t)f_head_angle;

    /* Try to calculate RC Yaw compensation or set new head holding position. */
    if ((abs_t(i32_rc_chanel_yaw_value) < 30) && u8_head_hold_mode)
    {
        /* Calc head error. */
        i32_heading_error = i32_heading - i32_head_hold;

        /* If body position is enough in horisontal level:*/
        if (abs_t((int32_t)fl_api_body_angle_wind_angles[Roll]) < 25)
        {
            if(abs_t((int32_t)fl_api_body_angle_wind_angles[Pitch]) < 25)
            {
                /* Calc compensation for Yaw value*/
                i32_rc_chanel_yaw_value = i32_rc_chanel_yaw_value - i32_heading_error * API_END_DEVICE_HEAD_HOLD_P_PID/30;
            }
        }
    }
    else
    {   /* Set new head hold possition. */
        i32_head_hold = i32_heading;
    }
#endif

    /* Calculate compensation of RC YAW with PDF filter.*/
    i32_servo = (int32_t)BOARD_PPM_ZERO_VALUE - i32_rc_chanel_yaw_value + i32_Yaw;
    i32_servo = constrain_i32(i32_servo,BOARD_PPM_MIN_VALUE,BOARD_PPM_MAX_VALUE);

    /* Drive end devices. */
    timer2_PWM_duty_CH1((uint16_t)i32_motor[0]);
    timer2_PWM_duty_CH2((uint16_t)i32_motor[1]);
    timer2_PWM_duty_CH3((uint16_t)i32_motor[2]);
    timer2_PWM_duty_CH4((uint16_t)i32_servo);
}
#endif

#if API_END_DEVICE_FLIGHT_SAURCER

void api_end_device_update(void)
{
    /* Motors. */
    int32_t i32_motor;
    int32_t i32_throttle;


    /* Direction. */
    int32_t i32_servo_yaw;

    int32_t i32_servo_forward;
    int32_t i32_servo_backward;
    int32_t i32_servo_left;
    int32_t i32_servo_right;


    int32_t i32_rc_chanel_yaw_value;

    /* Angles. */
    int32_t i32_Pitch;
    int32_t i32_Roll;
    int32_t i32_Yaw;
    static uint8_t u8_head_hold_mode = 0U;
    int32_t i32_heading              = 0;
    int32_t i32_heading_error        = 0;
    float f_head_angle               = 0.0f;

    /* BARO */
    static uint8_t u8_baro_mode = 0U;

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

    /* Turn on BaroAltHold. BARO ON/OFF */
    if( bc_channel_value_structure.u16_channel_5_value <= API_BARO_MODE_ON)
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
    else
    {
        u8_baro_mode = 0U;
    }

    /* If baro stabilisation is ON. */
    if(u8_baro_mode == 1U)
    {
        /* Change AltHold and Throttle Hold. */
        if(abs_t(i32_throttle - bp_baro_pid.i32_ThrHold) > ALT_BARO_HOLD_THROTTLE_NEUTRAL_ZONE)
        {
            u8_baro_mode = 0U;
        }

        /* Calculate baro stabilised throuttle. */
        i32_throttle = bp_baro_pid.i32_ThrHold + bp_baro_pid.i32_BaroPID;
    }

    float_api_common_variables[1] = (float)i32_throttle;

    i32_motor = i32_throttle;
   /* Saucer configuration. */

    i32_servo_forward   = (int32_t)BOARD_PPM_ZERO_VALUE + i32_Pitch;
    i32_servo_backward  = (int32_t)BOARD_PPM_ZERO_VALUE - i32_Pitch;
    i32_servo_left      = (int32_t)BOARD_PPM_ZERO_VALUE + i32_Roll;
    i32_servo_right     = (int32_t)BOARD_PPM_ZERO_VALUE - i32_Roll;

    i32_servo_forward   = constrain_i32(i32_servo_forward,  BOARD_PPM_MIN_VALUE, BOARD_PPM_MAX_VALUE);
    i32_servo_backward  = constrain_i32(i32_servo_backward, BOARD_PPM_MIN_VALUE, BOARD_PPM_MAX_VALUE);
    i32_servo_left      = constrain_i32(i32_servo_left,     BOARD_PPM_MIN_VALUE, BOARD_PPM_MAX_VALUE);
    i32_servo_right     = constrain_i32(i32_servo_right,    BOARD_PPM_MIN_VALUE, BOARD_PPM_MAX_VALUE);


    /* Get current RC YAW channel value. */
    i32_rc_chanel_yaw_value = (int32_t)bc_channel_value_structure.u16_channel_1_value;

    /* Calc aproximation value. */
    i32_rc_chanel_yaw_value = api_rx_channels_approximation(i32_rc_chanel_yaw_value, (int32_t)BOARD_PPM_ZERO_VALUE);

    /* Get MA filtered value. It give some small delay. */
    i32_rc_chanel_yaw_value = i32_api_filters_ma_rx_yaw(i32_rc_chanel_yaw_value);


#if API_END_DEVICE_HEAD_HOLDING
 /* HEAD HOLDING. */
    /* Check if headhold mode ON. */
    if( bc_channel_value_structure.u16_channel_6_value <= (uint16_t)API_END_DEVICE_HEAD_HOLD_MODE_ON)
    {
      if(u8_head_hold_mode == 0U)
      {
        u8_head_hold_mode = 1U;
        f_head_angle      = fl_api_body_angle_wind_angles[Yaw];

        /* Save current head position( direction). */
        i32_head_hold     = (int32_t)f_head_angle;
      }
    }
    else
    {
        u8_head_hold_mode = 0U;
    }

    /* Get current course dirrection. */
    f_head_angle = fl_api_body_angle_wind_angles[Yaw];
    i32_heading  = (int32_t)f_head_angle;

    /* Try to calculate RC Yaw compensation or set new head holding position. */
    if ((abs_t(i32_rc_chanel_yaw_value) < 30) && u8_head_hold_mode)
    {
        /* Calc head error. */
        i32_heading_error = i32_heading - i32_head_hold;

        /* If body position is enough in horisontal level:*/
        if (abs_t((int32_t)fl_api_body_angle_wind_angles[Roll]) < 25)
        {
            if(abs_t((int32_t)fl_api_body_angle_wind_angles[Pitch]) < 25)
            {
                /* Calc compensation for Yaw value*/
                i32_rc_chanel_yaw_value = i32_rc_chanel_yaw_value - i32_heading_error * API_END_DEVICE_HEAD_HOLD_P_PID/30;
            }
        }
    }
    else
    {   /* Set new head hold possition. */
        i32_head_hold = i32_heading;
    }
#endif

    /* Calculate compensation of RC YAW with PDF filter.*/
    i32_servo_yaw = (int32_t)BOARD_PPM_ZERO_VALUE - i32_rc_chanel_yaw_value + i32_Yaw;
    i32_servo_yaw = constrain_i32(i32_servo_yaw, BOARD_PPM_MIN_VALUE, BOARD_PPM_MAX_VALUE);

    /* Drive end devices. */
    timer2_PWM_duty_CH1((uint16_t)i32_servo_forward);
    timer2_PWM_duty_CH2((uint16_t)i32_servo_backward);
    timer2_PWM_duty_CH3((uint16_t)i32_servo_left);
    timer2_PWM_duty_CH4((uint16_t)i32_servo_right);

    timer3_PWM_duty_CH3((uint16_t)i32_motor);
    timer3_PWM_duty_CH4((uint16_t)i32_servo_yaw);
}




#endif




