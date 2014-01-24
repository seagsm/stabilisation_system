
#include "api_i2c_acquisition.h"


BOARD_I2C_DATA api_i2c_data;


static BOARD_DEVICE_PARAMETERS bdp_i2c_devices[MAX_DEV_NUM];



 /* Read start sequence acquisition   : */
BOARD_ERROR be_api_i2c_acquisition_start(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Reset device number. */
    api_i2c_data.u8_device = 0U;
    /* Reset data ready flag. */
    api_i2c_data.u8_ready  = 0U;
    /* Start process by starting first writing. */
    be_result = be_api_i2c_write_process_start();
    return(be_result);
}





 /* Data acquisition init function.*/

BOARD_ERROR be_api_i2c_acquisition_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    api_i2c_data.u8_device = 0U;
    api_i2c_data.u8_ready  = 0U;

    bdp_i2c_devices[0].u8_slave_address    = GYRO_DEVICE_ADDRESS;
    bdp_i2c_devices[0].u8_w_data	       = GYRO_READ_REGISTER;
    bdp_i2c_devices[0].u16_w_sizeof        = GYRO_WRITE_SIZE;
    bdp_i2c_devices[0].u16_r_sizeof        = GYRO_READ_SIZE;

    bdp_i2c_devices[1].u8_slave_address    = ACCELEROMETER_DEVICE_ADDRESS;
    bdp_i2c_devices[1].u8_w_data           = ACCELEROMETER_READ_REGISTER;
    bdp_i2c_devices[1].u16_w_sizeof        = ACCELEROMETER_WRITE_SIZE;
    bdp_i2c_devices[1].u16_r_sizeof        = ACCELEROMETER_READ_SIZE;

    bdp_i2c_devices[2].u8_slave_address    = MAGNETOMETER_DEVICE_ADDRESS;
    bdp_i2c_devices[2].u8_w_data           = MAGNETOMETER_READ_REGISTER;
    bdp_i2c_devices[2].u16_w_sizeof        = MAGNETOMETER_WRITE_SIZE;
    bdp_i2c_devices[2].u16_r_sizeof        = MAGNETOMETER_READ_SIZE;

    return(be_result);
}



BOARD_ERROR be_api_i2c_write_process_start(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t* pu8_buffer;
    uint16_t u16_num_byte_to_write;
    uint8_t u8_device_address;

	pu8_buffer            = &bdp_i2c_devices[api_i2c_data.u8_device].u8_w_data;       /* Pointer to variable with device write register number.*/
	u16_num_byte_to_write =  bdp_i2c_devices[api_i2c_data.u8_device].u16_w_sizeof;    /* Number of byte for write. ( here always 1 ) */
	u8_device_address	  =  bdp_i2c_devices[api_i2c_data.u8_device].u8_slave_address;/* Address of write device. */
	/* Call write-init function. */
    be_result = be_board_i2c_write_start(pu8_buffer, u16_num_byte_to_write, u8_device_address);
    return(be_result);
}

BOARD_ERROR be_api_i2c_read_process_start(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t* pu8_buffer;
    uint16_t u16_num_byte_to_read;
    uint8_t u8_device_address;

    pu8_buffer              = &api_i2c_data.array[api_i2c_data.u8_device].data[0];      /* Pointer of start of read buffer. */
	u16_num_byte_to_read    =  bdp_i2c_devices[api_i2c_data.u8_device].u16_r_sizeof;    /* Number of byte to read. ( here always 6 ) */
	u8_device_address	    =  bdp_i2c_devices[api_i2c_data.u8_device].u8_slave_address;/* Address of read device. */

    be_result = be_board_i2c_read_start(pu8_buffer, u16_num_byte_to_read, u8_device_address);

    return(be_result);
}







