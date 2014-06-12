#ifndef MAHONYAHRS_H
#define MAHONYAHRS_H

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "api_body_angle_calculation.h"


BOARD_ERROR mahony_update_AccGyroMag(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
BOARD_ERROR mahony_update_AccGyro(float gx, float gy, float gz, float ax, float ay, float az);


#endif

