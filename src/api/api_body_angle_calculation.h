#ifndef API_BODY_ANGLE_CALCULATION_H
#define API_BODY_ANGLE_CALCULATION_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "api_common.h"
#include "api_data_normalising.h"
#include "MadgwickAHRS.h"
#include "MahonyAHRS.h"
#include "api_filters.h"

extern  float fl_api_body_angle_quaternion[4];
extern  float fl_api_body_angle_wind_angles[3];
extern  float fl_api_body_angle_sample_period;

BOARD_ERROR be_api_body_angle_calculation(void);
static BOARD_ERROR be_api_body_angle_QuaternionToWindAngles(void);

#endif