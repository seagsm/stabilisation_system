#ifndef MADGWICKAHRS_H
#define MADGWICKAHRS_H

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "api_body_angle_calculation.h"

void madgwick_update_AccGyroMag(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
BOARD_ERROR madgwick_AccGyro(float gx, float gy, float gz, float ax, float ay, float az);
static float madgwick_3x_float_length(float x, float y, float z);
static float madgwick_4x_float_length(float f_a, float f_b, float f_c, float f_d);
#endif