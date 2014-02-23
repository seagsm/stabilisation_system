#ifndef API_COMMON_H
#define API_COMMON_H 1

#include "stm32f10x.h"
#include "board_system_type.h"



float deg2rad(float degrees);
float rad2deg(float rad);
int32_t constrain_i32(int32_t i32_x,int32_t i32_y,int32_t i32_z);
#endif