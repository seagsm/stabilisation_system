#ifndef API_DATA_PREPROCESSING_H
#define API_DATA_PREPROCESSING_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "api_data_normalising.h"

extern  BOARD_I16_3X_DATA bi163d_api_data_prepr_gyro_raw_data;
extern  BOARD_I16_3X_DATA bi163d_api_data_prepr_out_gyro_raw_data;
extern  BOARD_I16_3X_DATA bi163d_api_data_gyro_calibration_data;
extern  BOARD_I16_3X_DATA bi163d_api_data_prepr_acce_raw_data;
extern  BOARD_I16_3X_DATA bi163d_api_data_prepr_magn_raw_data;

void v_api_data_prepr_sensor_data_preprocessing(void);


#endif