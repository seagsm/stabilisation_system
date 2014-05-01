
#include "api_data_preprocessing.h"

BOARD_I16_3X_DATA       bi163d_api_data_prepr_gyro_raw_data;
BOARD_I16_3X_DATA       bi163d_api_data_prepr_out_gyro_raw_data;
BOARD_I16_3X_DATA       bi163d_api_data_gyro_calibration_data;
BOARD_I16_3X_DATA       bi163d_api_data_prepr_acce_raw_data;
BOARD_I16_3X_DATA       bi163d_api_data_prepr_magn_raw_data;

/*
    This function copy and convert data from api_i2c_data structure to
    sensor structure.
*/
void v_api_data_prepr_sensor_data_preprocessing(void)
{
    uint16_t u16_tmp;
    /* Convert api_i2c_data data buffer to int16 sensors data. */
    /* Gyro. */
    u16_tmp = (((uint16_t)  api_i2c_data.array[0].data[0]) << 8U) + ((uint16_t)api_i2c_data.array[0].data[1]);
    bi163d_api_data_prepr_gyro_raw_data.i16_X = (int16_t) u16_tmp;
    u16_tmp = (((uint16_t)  api_i2c_data.array[0].data[2]) << 8U) + ((uint16_t)api_i2c_data.array[0].data[3]);
    bi163d_api_data_prepr_gyro_raw_data.i16_Y = (int16_t) u16_tmp;
    u16_tmp = (((uint16_t)  api_i2c_data.array[0].data[4]) << 8U) + ((uint16_t)api_i2c_data.array[0].data[5]);
    bi163d_api_data_prepr_gyro_raw_data.i16_Z = (int16_t) u16_tmp;
    /* Accelerometer. */
    u16_tmp = (((uint16_t)  api_i2c_data.array[1].data[1]) << 8U) + ((uint16_t)api_i2c_data.array[1].data[0]);
    bi163d_api_data_prepr_acce_raw_data.i16_X = (int16_t) u16_tmp;
    u16_tmp = (((uint16_t)  api_i2c_data.array[1].data[3]) << 8U) + ((uint16_t)api_i2c_data.array[1].data[2]);
    bi163d_api_data_prepr_acce_raw_data.i16_Y = (int16_t) u16_tmp;
    u16_tmp = (((uint16_t)  api_i2c_data.array[1].data[5]) << 8U) + ((uint16_t)api_i2c_data.array[1].data[4]);
    bi163d_api_data_prepr_acce_raw_data.i16_Z = (int16_t) u16_tmp;
    /* Magnetometer. */
    u16_tmp = (((uint16_t)  api_i2c_data.array[2].data[0]) << 8U) + ((uint16_t)api_i2c_data.array[2].data[1]);
    bi163d_api_data_prepr_magn_raw_data.i16_X = (int16_t) u16_tmp;
    u16_tmp = (((uint16_t)  api_i2c_data.array[2].data[2]) << 8U) + ((uint16_t)api_i2c_data.array[2].data[3]);
    bi163d_api_data_prepr_magn_raw_data.i16_Y = (int16_t) u16_tmp;
    u16_tmp = (((uint16_t)  api_i2c_data.array[2].data[4]) << 8U) + ((uint16_t)api_i2c_data.array[2].data[5]);
    bi163d_api_data_prepr_magn_raw_data.i16_Z = (int16_t) u16_tmp;

    /* Normalize sensor value to float. */
    v_api_data_normalising_gyro();
    v_api_data_normalising_acce();
    v_api_data_normalising_magn();
}
