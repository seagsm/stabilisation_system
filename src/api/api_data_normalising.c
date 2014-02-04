

#include "api_data_normalising.h"


static BOARD_FLOAT_3X_DATA b_float3d_api_main_loop_gyro_data;
static BOARD_FLOAT_3X_DATA b_float3d_api_main_loop_acce_data;
static BOARD_FLOAT_3X_DATA b_float3d_api_main_loop_magn_data;




void v_api_data_normalising_gyro(void)
{
    b_float3d_api_main_loop_gyro_data.fl_X = (float)bi163d_api_main_loop_gyro_raw_data.i16_X;
    b_float3d_api_main_loop_gyro_data.fl_Y = (float)bi163d_api_main_loop_gyro_raw_data.i16_Y;
    b_float3d_api_main_loop_gyro_data.fl_Z = (float)bi163d_api_main_loop_gyro_raw_data.i16_Z;
}

void v_api_data_normalising_acce(void)
{
    b_float3d_api_main_loop_acce_data.fl_X = ((float)bi163d_api_main_loop_acce_raw_data.i16_X) - accelRTBias.fl_X;
    b_float3d_api_main_loop_acce_data.fl_Y = ((float)bi163d_api_main_loop_acce_raw_data.i16_Y) - accelRTBias.fl_Y;
    b_float3d_api_main_loop_acce_data.fl_Z = ((float)bi163d_api_main_loop_acce_raw_data.i16_Z) - accelRTBias.fl_Z;
}

void v_api_data_normalising_magn(void)
{
    b_float3d_api_main_loop_magn_data.fl_X = (float)bi163d_api_main_loop_magn_raw_data.i16_X;
    b_float3d_api_main_loop_magn_data.fl_Y = (float)bi163d_api_main_loop_magn_raw_data.i16_Y;
    b_float3d_api_main_loop_magn_data.fl_Z = (float)bi163d_api_main_loop_magn_raw_data.i16_Z;
}











