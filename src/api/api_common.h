#ifndef API_COMMON_H
#define API_COMMON_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "math.h"

extern BOARD_FLOAT_3X_DATA b_float3d_api_common_out_acce_data;
extern float float_api_common_variables[3];
extern float float_api_common_motors[3];


float deg2rad(float degrees);
float rad2deg(float rad);
double dbl_rad2deg(double rad);
double dbl_deg2rad(double degrees);

int32_t constrain_i32(int32_t i32_x,int32_t i32_y,int32_t i32_z);
float   fl_constrain(float fl_x, float fl_y, float fl_z);
int32_t min(int32_t i32_x,int32_t i32_y);
int32_t abs_t(int32_t i32_x);

float f_sin(float f_value);
float f_cos(float f_value);
float f_pow(float f_value, float fl_i);
float f_sqrt(float fl_value);

#endif