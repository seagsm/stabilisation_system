
#include "api_body_angle_calculation.h"

    float fl_api_body_angle_quaternion[4] = {1.0f, 0.0f, 0.0f, .0f };
    float fl_api_body_angle_wind_angles[3];
    float fl_api_body_angle_sample_period;

static  float fl_api_body_angle_old_time;

/* This function calculate body quaternion. */
BOARD_ERROR be_api_body_angle_calculation(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    float fl_api_body_angle_now_time;

    /* Sample period calculation. */
    /* TODO: should be optimised. */
    fl_api_body_angle_now_time      = (float)gu64_read_system_time();
    fl_api_body_angle_sample_period = fl_api_body_angle_now_time - fl_api_body_angle_old_time;
    fl_api_body_angle_old_time      = fl_api_body_angle_now_time;

    /* Convert period from tick to seconds. */
    fl_api_body_angle_sample_period = fl_api_body_angle_sample_period / 1000.0f;

/*
    b_float3d_api_data_norm_acce_data.fl_X = float_api_filters_iir_acc_x(b_float3d_api_data_norm_acce_data.fl_X);
    b_float3d_api_data_norm_acce_data.fl_Y = float_api_filters_iir_acc_y(b_float3d_api_data_norm_acce_data.fl_Y);
    b_float3d_api_data_norm_acce_data.fl_Z = float_api_filters_iir_acc_z(b_float3d_api_data_norm_acce_data.fl_Z);
*/
/*
    b_float3d_api_data_norm_acce_data.fl_X = float_api_filters_iir_acc_x_1Hz(b_float3d_api_data_norm_acce_data.fl_X);
    b_float3d_api_data_norm_acce_data.fl_Y = float_api_filters_iir_acc_y_1Hz(b_float3d_api_data_norm_acce_data.fl_Y);
    b_float3d_api_data_norm_acce_data.fl_Z = float_api_filters_iir_acc_z_1Hz(b_float3d_api_data_norm_acce_data.fl_Z);
*/


    b_float3d_api_data_norm_acce_data.fl_X = float_api_filters_ma_acc_x(b_float3d_api_data_norm_acce_data.fl_X);
    b_float3d_api_data_norm_acce_data.fl_Y = float_api_filters_ma_acc_y(b_float3d_api_data_norm_acce_data.fl_Y);
    b_float3d_api_data_norm_acce_data.fl_Z = float_api_filters_ma_acc_z(b_float3d_api_data_norm_acce_data.fl_Z);

    b_float3d_api_common_out_acce_data.fl_X = b_float3d_api_data_norm_acce_data.fl_X;
    b_float3d_api_common_out_acce_data.fl_Y = b_float3d_api_data_norm_acce_data.fl_Y;
    b_float3d_api_common_out_acce_data.fl_Z = b_float3d_api_data_norm_acce_data.fl_Z;


    b_float3d_api_data_norm_gyro_data.fl_X = float_api_filters_iir_gyro_x(b_float3d_api_data_norm_gyro_data.fl_X);
    b_float3d_api_data_norm_gyro_data.fl_Y = float_api_filters_iir_gyro_y(b_float3d_api_data_norm_gyro_data.fl_Y);
    b_float3d_api_data_norm_gyro_data.fl_Z = float_api_filters_iir_gyro_z(b_float3d_api_data_norm_gyro_data.fl_Z);

    /* Calculation of body quaternion. */
#if 0
    be_result = madgwick_AccGyro(
                                    deg2rad(b_float3d_api_data_norm_gyro_data.fl_X),
                                    deg2rad(b_float3d_api_data_norm_gyro_data.fl_Y),
                                    deg2rad(b_float3d_api_data_norm_gyro_data.fl_Z),
                                    b_float3d_api_data_norm_acce_data.fl_X,
                                    b_float3d_api_data_norm_acce_data.fl_Y,
                                    b_float3d_api_data_norm_acce_data.fl_Z
                                );
#else
    be_result = mahony_update_AccGyro(
                                        deg2rad(b_float3d_api_data_norm_gyro_data.fl_X),
                                        deg2rad(b_float3d_api_data_norm_gyro_data.fl_Y),
                                        deg2rad(b_float3d_api_data_norm_gyro_data.fl_Z),
                                        b_float3d_api_data_norm_acce_data.fl_X,
                                        b_float3d_api_data_norm_acce_data.fl_Y,
                                        b_float3d_api_data_norm_acce_data.fl_Z
                                     );
#endif

    /* Calculation of body wind angles. */
    be_result |= be_api_body_angle_QuaternionToWindAngles();

    return (be_result);
}

/* This function convert body quaternion to body wind angles, see reference in MathLab DCM->Wind. */
static BOARD_ERROR be_api_body_angle_QuaternionToWindAngles(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    float q0 = fl_api_body_angle_quaternion[0];
    float q1 = fl_api_body_angle_quaternion[1];
    float q2 = fl_api_body_angle_quaternion[2];
    float q3 = fl_api_body_angle_quaternion[3];  /* short name local variable for readability. */
    float fl_a, fl_b;

    /* TODO: should be added dividing by zero controls. */
    /* Mu angle. */
    fl_a = 2.0f * (q2 * q3 + q0 * q1);
    fl_b = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;
    fl_api_body_angle_wind_angles[0] = rad2deg((float)atan2((double)fl_a,(double)fl_b));
    /* Gamma angle. */
    fl_a = -2.0f * (q1 * q3 - q0 * q2);
    fl_api_body_angle_wind_angles[1] = rad2deg((float)asin((double)fl_a));
    /* Kzetta angle. */
    fl_a = 2.0f * (q1 * q2 + q0 * q3);
    fl_b = q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3;
    fl_api_body_angle_wind_angles[2] = rad2deg((float)atan2((double)fl_a,(double)fl_b));

    return(be_result);
}
