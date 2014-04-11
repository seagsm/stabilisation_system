
#include "api_pid.h"

PID_element pid_api_pid_data[3];

/* Call it to initialize PID structures. */
void api_pid_init(void)
{
    api_pid_init_Pitch();
    api_pid_init_Roll();
    api_pid_init_Yaw();
}

/* Default Pitch PID value. */
static void api_pid_init_Pitch(void)
{
    pid_api_pid_data[Pitch].i32_p_gain           = PITCH_P_GAIN;
    pid_api_pid_data[Pitch].i32_p_dyn_gain       = PITCH_P_GAIN_DYN;
    pid_api_pid_data[Pitch].i32_i_gain           = PITCH_I_GAIN;
    pid_api_pid_data[Pitch].i32_i_dyn_gain       = PITCH_I_GAIN_DYN;
    pid_api_pid_data[Pitch].i32_i_state          = 0;
    pid_api_pid_data[Pitch].i32_i_min            = PITCH_I_MIN;
    pid_api_pid_data[Pitch].i32_i_min            = PITCH_I_MAX;
    pid_api_pid_data[Pitch].i32_d_gain           = PITCH_D_GAIN;
    pid_api_pid_data[Pitch].i32_d_dyn_gain       = PITCH_D_GAIN_DYN;
    pid_api_pid_data[Pitch].i32_d_state          = 0;
    pid_api_pid_data[Pitch].i32_d_0_state        = 0;
    pid_api_pid_data[Pitch].i32_d_1_state        = 0;
    pid_api_pid_data[Pitch].i32_d_angle_speed    = 0;
}

/* Default Roll PID value. */
static void api_pid_init_Roll(void)
{
    pid_api_pid_data[Roll].i32_p_gain           = ROLL_P_GAIN;
    pid_api_pid_data[Roll].i32_p_dyn_gain       = ROLL_P_GAIN_DYN;
    pid_api_pid_data[Roll].i32_i_gain           = ROLL_I_GAIN;
    pid_api_pid_data[Roll].i32_i_dyn_gain       = ROLL_I_GAIN_DYN;
    pid_api_pid_data[Roll].i32_i_state          = 0;
    pid_api_pid_data[Roll].i32_i_min            = ROLL_I_MIN;
    pid_api_pid_data[Roll].i32_i_min            = ROLL_I_MAX;
    pid_api_pid_data[Roll].i32_d_gain           = ROLL_D_GAIN;
    pid_api_pid_data[Roll].i32_d_dyn_gain       = ROLL_D_GAIN_DYN;
    pid_api_pid_data[Roll].i32_d_state          = 0;
    pid_api_pid_data[Roll].i32_d_0_state        = 0;
    pid_api_pid_data[Roll].i32_d_1_state        = 0;
    pid_api_pid_data[Roll].i32_d_angle_speed    = 0;
}

/* Default Yaw PID value. */
static void api_pid_init_Yaw(void)
{
    pid_api_pid_data[Yaw].i32_p_gain           = YAW_P_GAIN;
    pid_api_pid_data[Yaw].i32_p_dyn_gain       = YAW_P_GAIN_DYN;
    pid_api_pid_data[Yaw].i32_i_gain           = YAW_I_GAIN;
    pid_api_pid_data[Yaw].i32_i_dyn_gain       = YAW_I_GAIN_DYN;
    pid_api_pid_data[Yaw].i32_i_state          = 0;
    pid_api_pid_data[Yaw].i32_i_min            = YAW_I_MIN;
    pid_api_pid_data[Yaw].i32_i_min            = YAW_I_MAX;
    pid_api_pid_data[Yaw].i32_d_gain           = YAW_D_GAIN;
    pid_api_pid_data[Yaw].i32_d_dyn_gain       = YAW_D_GAIN_DYN;
    pid_api_pid_data[Yaw].i32_d_state          = 0;
    pid_api_pid_data[Yaw].i32_d_0_state        = 0;
    pid_api_pid_data[Yaw].i32_d_1_state        = 0;
    pid_api_pid_data[Yaw].i32_d_angle_speed    = 0;
}


/* Pseudo derivative filter. */
static void api_pid_update_PDF(PID_element *current_pid, int32_t i32_set_point, int32_t i32_angle_position, int32_t i32_angle_speed)
{
    int32_t i32_position_error;
    int32_t i32_p_term;
    int32_t i32_i_term;
    int32_t i32_d_term;
    int32_t i32_d_state;
    int32_t i32_d_summ;

    /*
    * And angle_position should be +-1800 in 0.1 deg, that means +-180 degree output of kalman filter
    * for Roll,Pitch or Yaw.
    */

    /* Level error calculation. */
    i32_position_error = constrain_i32(2 * i32_set_point - i32_angle_position, -MAX_ANGLE_INCLINATION, MAX_ANGLE_INCLINATION);

   /* Level proportional stabilisation. */
    i32_p_term = (current_pid->i32_p_gain * i32_position_error) /100;

   /*
    * p_dyn_gain->p roll or pitch arround 70
    * Gyro_Rate is +- 24000 is +- 300deg/Sec let fit it to +-8192 +-2000 deg/Sec
    * 300/ 24000 = 0.0125 degree/second  for one bit.
    * 2000/8192  =  0.244 degree/second  for one bit.
    * 0.244/0.0125 ~ 19
    * So 19 is fitting coefficient
    */

    /* Angle speed proportional stabilisation */
    i32_p_term = i32_p_term - (i32_angle_speed * current_pid->i32_p_dyn_gain)/ 80 / 19;

    /* Integration. */
    current_pid->i32_i_state = constrain_i32(current_pid->i32_i_state + i32_position_error, current_pid->i32_i_min, current_pid->i32_i_max);
    /* Calculation i_term. */
    i32_i_term = (current_pid->i32_i_state * current_pid->i32_i_gain)/4096; /* pid->i_gain arround 13,so resul arround +- 30. */

    /* To found acceleration calculate delta speed. */
    i32_d_state = i32_angle_speed - current_pid->i32_d_angle_speed;

    /* Save current speed value */
    current_pid->i32_d_angle_speed = i32_angle_speed;

    /* Smoothing filter. */
    i32_d_summ = i32_d_state + current_pid->i32_d_0_state + current_pid->i32_d_1_state;
    current_pid->i32_d_1_state = current_pid->i32_d_0_state;
    current_pid->i32_d_0_state = i32_d_state;

    i32_d_term = (i32_d_summ * current_pid->i32_d_dyn_gain)/2048 ;/* //32// pid->d_gain arround 40 , so pid->d_gain/32 arround 1~2 */

    /* Save result of PID calculation back to structure. */
    current_pid->i32_pid_output = i32_p_term + i32_i_term - i32_d_term;
}


/* Function calculate PDF values for all three channels. */
void api_pid_update_frame(void)
{
    int32_t i32_rc_chanel_value;
    int32_t i32_body_angle;
    float f_body_angle;
    int32_t i32_body_angle_speed;
    float f_body_angle_speed;

    /* Calculation of Pitch PDF frame. */

    /* Convert channel value to int32. */
    i32_rc_chanel_value = (int32_t)bc_channel_value_structure.u16_channel_2_value;
    /* Get deviation from ZERO value. */
    i32_rc_chanel_value = i32_rc_chanel_value - BOARD_PPM_ZERO_VALUE;
    /* Convert body inclination from degree to 0.1 degree. ( 10degree = 100(0.1degree))*/
    f_body_angle        = fl_api_body_angle_wind_angles[Pitch] * 10.0f;
    i32_body_angle      = (int32_t)f_body_angle;
    /* Convert body angle speed from degree/sec to 0.1 degree/sec. */
    f_body_angle_speed  = b_float3d_api_data_norm_gyro_data.fl_Y * 10.0f;
    i32_body_angle_speed= (int32_t)f_body_angle_speed;
    /* update Pitch frame. */
    api_pid_update_PDF( &pid_api_pid_data[Pitch], i32_rc_chanel_value, i32_body_angle, i32_body_angle_speed);

    /* Calculation of Roll PDF frame. */
    i32_rc_chanel_value = (int32_t)bc_channel_value_structure.u16_channel_4_value;
    i32_rc_chanel_value = i32_rc_chanel_value - BOARD_PPM_ZERO_VALUE;
    f_body_angle        = fl_api_body_angle_wind_angles[Roll] * 10.0f;
    i32_body_angle      = (int32_t)f_body_angle;
    f_body_angle_speed  = b_float3d_api_data_norm_gyro_data.fl_X * 10.0f;
    i32_body_angle_speed= (int32_t)f_body_angle_speed;
    /* update Roll frame. */
    api_pid_update_PDF( &pid_api_pid_data[Roll], i32_rc_chanel_value, i32_body_angle, i32_body_angle_speed);

    /* Calculation of Yaw PDF frame. */
    i32_rc_chanel_value = (int32_t)bc_channel_value_structure.u16_channel_1_value;
    i32_rc_chanel_value = i32_rc_chanel_value - BOARD_PPM_ZERO_VALUE;
    f_body_angle        = fl_api_body_angle_wind_angles[Yaw] * 10.0f;
    i32_body_angle      = (int32_t)f_body_angle;
    f_body_angle_speed  = b_float3d_api_data_norm_gyro_data.fl_Z * 10.0f;
    i32_body_angle_speed= (int32_t)f_body_angle_speed;
    /* update Yaw frame. */
    api_pid_update_PDF( &pid_api_pid_data[Yaw], i32_rc_chanel_value, i32_body_angle, i32_body_angle_speed);
}