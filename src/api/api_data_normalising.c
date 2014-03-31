

#include "api_data_normalising.h"


BOARD_FLOAT_3X_DATA b_float3d_api_data_norm_gyro_data;
BOARD_FLOAT_3X_DATA b_float3d_api_data_norm_acce_data;
static BOARD_FLOAT_3X_DATA b_float3d_api_data_norm_magn_data;

/* Calculation of GYRO normalising coefficient to have degre per second (dps). */
static float f_gyro_rate = ( 500.0 *((float)(BOARD_DRV_GYRO_RATE >> 4) - 11.0))/(65536.0);

void v_api_data_normalising_gyro(void)
{
    b_float3d_api_data_norm_gyro_data.fl_X = (float)bi163d_api_data_prepr_gyro_raw_data.i16_X * f_gyro_rate;
    b_float3d_api_data_norm_gyro_data.fl_Y = (float)bi163d_api_data_prepr_gyro_raw_data.i16_Y * f_gyro_rate;
    b_float3d_api_data_norm_gyro_data.fl_Z = (float)bi163d_api_data_prepr_gyro_raw_data.i16_Z * f_gyro_rate;

    b_float3d_api_data_norm_gyro_data.fl_X = -deg2rad(b_float3d_api_data_norm_gyro_data.fl_X);
    b_float3d_api_data_norm_gyro_data.fl_Y = -deg2rad(b_float3d_api_data_norm_gyro_data.fl_Y);
    b_float3d_api_data_norm_gyro_data.fl_Z =  deg2rad(b_float3d_api_data_norm_gyro_data.fl_Z);
}

void v_api_data_normalising_acce(void)
{   /* Remove bias. */
    b_float3d_api_data_norm_acce_data.fl_X = -(((float)bi163d_api_data_prepr_acce_raw_data.i16_X) - accelRTBias.fl_X);
    b_float3d_api_data_norm_acce_data.fl_Y = -(((float)bi163d_api_data_prepr_acce_raw_data.i16_Y) - accelRTBias.fl_Y);
    b_float3d_api_data_norm_acce_data.fl_Z =  (((float)bi163d_api_data_prepr_acce_raw_data.i16_Z) - accelRTBias.fl_Z);
}

void v_api_data_normalising_magn(void)
{
    b_float3d_api_data_norm_magn_data.fl_X = (float)bi163d_api_data_prepr_magn_raw_data.i16_X;
    b_float3d_api_data_norm_magn_data.fl_Y = (float)bi163d_api_data_prepr_magn_raw_data.i16_Y;
    b_float3d_api_data_norm_magn_data.fl_Z = (float)bi163d_api_data_prepr_magn_raw_data.i16_Z;
}

/* Normalize sensor value to float. */
void v_api_data_normalising(void)
{
    v_api_data_normalising_gyro();
    v_api_data_normalising_acce();
    v_api_data_normalising_magn();
}







