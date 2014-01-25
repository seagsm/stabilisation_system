#ifndef USER_PROCESSING_H
#define USER_PROCESSING_H 1

#include "stm32f10x.h"
#include "board_types.h"
#include "user_api_types.h"
#include "board_adxl345.h"
#include "board_hmc5883.h"
#include "board_bmp085.h"
#include "board_filter.h"
#include "MahonyAHRS.h"
#include "MadgwickAHRS.h"
#include "user_pid.h"
#include "math.h"


//0.00875f -> 250 degree/sec
//0.017.5  -> 500 degree/sec
//0.070    -> 2000 degree/sec
#define GyroCoef 0.00875 // How match degree/sec for one bit,
                          // This is parameter from Gyro datasheet.
extern float Quaternion[4];
extern float SamplePeriod;


void start_processing(control_pid_est_t *board,sensor_t *gyro);
void start_processing_sensors(sensor_raw_data_t *board,sensor_t *gyro);
void magnet_axis_correction(sensor_float_data_t* sensor);
void convert_quaternion_to_euler_angles(euler_angle_t* euler_angle);
void convert_quaternion_to_euler_angles_v2(euler_angle_t* euler_angle);

float deg2rad(float degrees);
float rad2deg(float rad);
#endif