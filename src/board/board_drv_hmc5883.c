

#include "board_drv_hmc5883.h"


BOARD_FLOAT_3X_DATA bf3d_mag_scale_factor;
BOARD_3X_DATA b3d_raw_magnitometer;

void v_board_drv_hmc5883_read(void)
{
    uint8_t u8_buffer[6];

    board_i2c_read(HMC5883_ADDRESS, HMC5883_DATA_X_MSB_REG, 6U, u8_buffer);

    b3d_raw_magnitometer.u16_X = (((uint16_t)  u8_buffer[0]) << 8U) + ((uint16_t)u8_buffer[1]);
    b3d_raw_magnitometer.u16_Y = (((uint16_t)  u8_buffer[2]) << 8U) + ((uint16_t)u8_buffer[3]);
    b3d_raw_magnitometer.u16_Z = (((uint16_t)  u8_buffer[4]) << 8U) + ((uint16_t)u8_buffer[5]);
}

void v_board_drv_hmc5883_init(void)
{
    bf3d_mag_scale_factor.fl_X = 1.0f;       /* (1.16F * 1090.0F) / (float)rawMag[XAXIS].value; */
    bf3d_mag_scale_factor.fl_Y = 1.0f;       /* (1.16F * 1090.0F) / (float)rawMag[YAXIS].value; */
    bf3d_mag_scale_factor.fl_Z = 1.0f;       /* (1.08F * 1090.0F) / (float)rawMag[ZAXIS].value; */

    board_i2c_write(HMC5883_ADDRESS, HMC5883_CONFIG_REG_A, SENSOR_CONFIG | NORMAL_MEASUREMENT_CONFIGURATION);
    gv_board_sys_tick_fast_delay(20U);

    board_i2c_write(HMC5883_ADDRESS, HMC5883_MODE_REG, OP_MODE_CONTINUOUS);
    gv_board_sys_tick_fast_delay(20U);

    v_board_drv_hmc5883_read();

    gv_board_sys_tick_fast_delay(20U);
}
