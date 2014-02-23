


#include "api_end_device.h"



/*This function calculate value of PPM (PWM) for all end device. (Motors, servos for tricopter configuration.) */
void api_end_device_update(void)
{
    /* Motors. */
    int32_t i32_motor[3];
    int32_t i32_throttle;
    /* Direction. */
    int32_t i32_servo;

    /* Angles. */
    int32_t i32_Pitch;
    int32_t i32_Roll;
    int32_t i32_Yaw;

    /* Copy PDF data. */
    i32_Pitch   = pid_api_pid_data[Pitch].i32_pid_output;
    i32_Roll    = pid_api_pid_data[Roll].i32_pid_output;
    i32_Yaw     = pid_api_pid_data[Yaw].i32_pid_output;

    /*
        Threshold of THROTTLE.
        For some condition without it possible problems during start of motors.
    */
    i32_throttle = (int32_t)bc_channel_value_structure.u16_channel_3_value;
    if(i32_throttle <= API_END_DEVICE_MIN_THROTTLE)
    {
        i32_motor[0] = i32_throttle;
        i32_motor[1] = i32_throttle;
        i32_motor[2] = i32_throttle;
    }
    else
    {   /* Tricopter configuration. */
        i32_motor[0] = i32_throttle - i32_Roll - (i32_Pitch * 2) / 3;
        i32_motor[1] = i32_throttle + i32_Roll - (i32_Pitch * 2) / 3;
        i32_motor[2] = i32_throttle + (i32_Pitch * 4)/3;
    }

    i32_motor[0] = constrain_i32(i32_motor[0],BOARD_PPM_MIN_VALUE,BOARD_PPM_MAX_VALUE);
    i32_motor[1] = constrain_i32(i32_motor[1],BOARD_PPM_MIN_VALUE,BOARD_PPM_MAX_VALUE);
    i32_motor[2] = constrain_i32(i32_motor[2],BOARD_PPM_MIN_VALUE,BOARD_PPM_MAX_VALUE);

    /* Calculate value for cource stabilisation servo. */
    i32_servo = BOARD_PPM_ZERO_VALUE + i32_Yaw;
    i32_servo = constrain_i32(i32_servo,BOARD_PPM_MIN_VALUE,BOARD_PPM_MAX_VALUE);

    /* Drive end devices. */
    timer2_PWM_duty_CH1((uint16_t)i32_motor[0]);
    timer2_PWM_duty_CH2((uint16_t)i32_motor[1]);
    timer2_PWM_duty_CH3((uint16_t)i32_motor[2]);
    timer2_PWM_duty_CH4((uint16_t)i32_servo);
}






