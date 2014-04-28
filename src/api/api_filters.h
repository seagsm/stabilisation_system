#ifndef API_FILTERS_H
#define API_FILTERS_H 1

#include "stm32f10x.h"
#include "board_system_type.h"


float float_api_filters_iir_acc_x(float float_new_sample);
float float_api_filters_iir_acc_y(float float_new_sample);
float float_api_filters_iir_acc_z(float float_new_sample);
float float_api_filters_iir_gyro_x(float float_new_sample);
float float_api_filters_iir_gyro_y(float float_new_sample);
float float_api_filters_iir_gyro_z(float float_new_sample);

float float_api_filters_ma_acc_x(float float_new_sample);
float float_api_filters_ma_acc_y(float float_new_sample);
float float_api_filters_ma_acc_z(float float_new_sample);

float float_api_filters_iir_acc_x_1Hz(float float_new_sample);
float float_api_filters_iir_acc_y_1Hz(float float_new_sample);
float float_api_filters_iir_acc_z_1Hz(float float_new_sample);


#endif