

#include "api_data_normalising.h"


BOARD_FLOAT_3X_DATA b_float3d_api_data_norm_gyro_data;
BOARD_FLOAT_3X_DATA b_float3d_api_data_norm_acce_data;

static BOARD_FLOAT_3X_DATA b_float3d_api_data_norm_magn_data;

/* Calculation of GYRO normalising coefficient to have degre per second (dps). */
/* static float f_gyro_rate = ( 500.0 *((float)(BOARD_DRV_GYRO_RATE >> 4) - 11.0))/(65536.0); */
static float f_gyro_rate = 0.00875 * (float)( 1U << ((BOARD_DRV_GYRO_RATE >> 4) - 12U));


/* This function calculating a gyro data bias for all three axes.  */
uint8_t v_api_data_normalising_gyro_calibration(void)
{
    static uint8_t u8_calibration_flag          = 0U;
    static int32_t i32_calibration_summ[3]      = {0};
    static uint16_t u16_calibration_counter     = 0U;

    /* Summ CALIBRATION_COUNT amount of gyro data. */
    i32_calibration_summ[0] = i32_calibration_summ[0] + bi163d_api_data_prepr_gyro_raw_data.i16_X;
    i32_calibration_summ[1] = i32_calibration_summ[1] + bi163d_api_data_prepr_gyro_raw_data.i16_Y;
    i32_calibration_summ[2] = i32_calibration_summ[2] + bi163d_api_data_prepr_gyro_raw_data.i16_Z;

    u16_calibration_counter++;
    if((u16_calibration_counter >= CALIBRATION_COUNT)&&(u8_calibration_flag == 0U))
    {
        /* Calculation of middle average of gyro data to know ZERO level.*/
        i32_calibration_summ[0] = i32_calibration_summ[0] / (int32_t)CALIBRATION_COUNT;
        i32_calibration_summ[1] = i32_calibration_summ[1] / (int32_t)CALIBRATION_COUNT;
        i32_calibration_summ[2] = i32_calibration_summ[2] / (int32_t)CALIBRATION_COUNT;

        /* Nor recognizet yet. */
        bi163d_api_data_gyro_calibration_data.i16_X = (int16_t)i32_calibration_summ[0] ;
        bi163d_api_data_gyro_calibration_data.i16_Y = (int16_t)i32_calibration_summ[1] ;
        bi163d_api_data_gyro_calibration_data.i16_Z = (int16_t)i32_calibration_summ[2];
        /* Set flag callibration done. */
        u8_calibration_flag = 1U;
    }
    return (u8_calibration_flag);
}

/* This function set the gyro axes to real body axis and removing gyro bias. */
void v_api_data_normalising_gyro(void)
{
    int16_t i16_tmp[3];

    /* Calibration, remove bias. */
    i16_tmp[0] = bi163d_api_data_prepr_gyro_raw_data.i16_X - bi163d_api_data_gyro_calibration_data.i16_X;
    i16_tmp[1] = bi163d_api_data_prepr_gyro_raw_data.i16_Y - bi163d_api_data_gyro_calibration_data.i16_Y;
    i16_tmp[2] = bi163d_api_data_prepr_gyro_raw_data.i16_Z - bi163d_api_data_gyro_calibration_data.i16_Z;

    /* Set output data. Fot information only. */
    bi163d_api_data_prepr_out_gyro_raw_data.i16_X = i16_tmp[0];
    bi163d_api_data_prepr_out_gyro_raw_data.i16_Y = i16_tmp[1];
    bi163d_api_data_prepr_out_gyro_raw_data.i16_Z = i16_tmp[2];

    /* Normalizing.*/
    b_float3d_api_data_norm_gyro_data.fl_X = (float)i16_tmp[0] * f_gyro_rate;
    b_float3d_api_data_norm_gyro_data.fl_Y = (float)i16_tmp[1] * f_gyro_rate;
    b_float3d_api_data_norm_gyro_data.fl_Z = (float)i16_tmp[2] * f_gyro_rate;

    /* Fitting to axis. */
    b_float3d_api_data_norm_gyro_data.fl_X = -b_float3d_api_data_norm_gyro_data.fl_X;
    b_float3d_api_data_norm_gyro_data.fl_Y = -b_float3d_api_data_norm_gyro_data.fl_Y;
    b_float3d_api_data_norm_gyro_data.fl_Z =  b_float3d_api_data_norm_gyro_data.fl_Z;
}

/* This function set acce axis to real body axis. */
void v_api_data_normalising_acce(void)
{   /* Remove bias. */
    b_float3d_api_data_norm_acce_data.fl_X = -(((float)bi163d_api_data_prepr_acce_raw_data.i16_X) - accelRTBias.fl_X);
    b_float3d_api_data_norm_acce_data.fl_Y = -(((float)bi163d_api_data_prepr_acce_raw_data.i16_Y) - accelRTBias.fl_Y);
    b_float3d_api_data_norm_acce_data.fl_Z =  (((float)bi163d_api_data_prepr_acce_raw_data.i16_Z) - accelRTBias.fl_Z);
}

/* This function set magnetometer axis to real body axis. */
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







