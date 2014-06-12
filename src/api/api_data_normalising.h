#ifndef API_DATA_NORMALISING_H
#define API_DATA_NORMALISING_H 1

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "api_data_preprocessing.h"
#include "api_common.h"
#include "board_drv_l3g4200d.h"


#define CALIBRATION_COUNT   1000U

extern BOARD_FLOAT_3X_DATA b_float3d_api_data_norm_gyro_data;
extern BOARD_FLOAT_3X_DATA b_float3d_api_data_norm_acce_data;

/* extern BOARD_FLOAT_3X_DATA b_float3d_api_data_norm_magn_data; */

void v_api_data_normalising_gyro(void);
uint8_t v_api_data_normalising_gyro_calibration(void);
void v_api_data_normalising_acce(void);
void v_api_data_normalising_magn(void);
void v_api_data_normalising(void);












#endif