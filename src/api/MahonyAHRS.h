#ifndef MAHONYAHRS_H
#define MAHONYAHRS_H

#include "stm32f10x.h"
#include "user_processing.h"

extern float Quaternion[4];
extern float mahony_Ki;
extern float mahony_Kp;
extern float SamplePeriod;

void mahony_update_AccGyroMag(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void mahony_update_AccGyro(float gx, float gy, float gz, float ax, float ay, float az);

#endif