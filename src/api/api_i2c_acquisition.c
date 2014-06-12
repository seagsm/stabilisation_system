#include "api_i2c_acquisition.h"

/*
    Global structure, has read from device data.
    Each device has six bytes array inside of that structure.
*/
BOARD_I2C_DATA api_i2c_data;

static BOARD_DEVICE_PARAMETERS bdp_i2c_devices[MAX_DEV_NUM];

/* Data acquisition init function. Should be started first. */
BOARD_ERROR be_api_i2c_acquisition_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    api_i2c_data.u8_device = 0U;
    api_i2c_data.u8_ready  = 0U;

    bdp_i2c_devices[0].u8_slave_address = GYRO_DEVICE_ADDRESS;
    bdp_i2c_devices[0].u8_w_data        = GYRO_READ_REGISTER;
    bdp_i2c_devices[0].u16_w_sizeof     = GYRO_WRITE_SIZE;
    bdp_i2c_devices[0].u16_r_sizeof     = GYRO_READ_SIZE;

    bdp_i2c_devices[1].u8_slave_address = ACCELEROMETER_DEVICE_ADDRESS;
    bdp_i2c_devices[1].u8_w_data        = ACCELEROMETER_READ_REGISTER;
    bdp_i2c_devices[1].u16_w_sizeof     = ACCELEROMETER_WRITE_SIZE;
    bdp_i2c_devices[1].u16_r_sizeof     = ACCELEROMETER_READ_SIZE;

    bdp_i2c_devices[2].u8_slave_address = MAGNETOMETER_DEVICE_ADDRESS;
    bdp_i2c_devices[2].u8_w_data        = MAGNETOMETER_READ_REGISTER;
    bdp_i2c_devices[2].u16_w_sizeof     = MAGNETOMETER_WRITE_SIZE;
    bdp_i2c_devices[2].u16_r_sizeof     = MAGNETOMETER_READ_SIZE;

    /* Set BARO state machine start state. */
    be_result = be_board_baro_set_state(START_CONVERSION);



    return(be_result);
}

/*
    Start of read sequence acquisition.
    It took 20 uSec, and data will be ready after:
    speed 100000 - 2.65mSec.
    speed 400000 - 0.79mSec.
    This function start write sequence for first device and exit. Read sequence is full automatic
    done by combination of DMA and interrupt.
    DMA interupt CH6 "writing done" start reading sequence from first device.
    DMA interupt CH7 "reading done" copy data from static DMA CH7 array to
    api_i2c_data.array[0], increase device number for 1 and start write
    sequence for device 1 ...
    At the end of operation, then device number = 3 (0,1,2 - 3 devices),
    DMA interrupt function set data ready flag  api_i2c_data.u8_ready to 1
    end exit.
*/
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

/* Write process start service function. */
BOARD_ERROR be_api_i2c_write_process_start(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t* pu8_buffer;
    uint16_t u16_num_byte_to_write;
    uint8_t u8_device_address;

    /* Write valid for GYRO, MAGN, ACC. */
    if(api_i2c_data.u8_device < 3U )
    {
        pu8_buffer              = &bdp_i2c_devices[api_i2c_data.u8_device].u8_w_data;       /* Pointer to variable with device write register number.*/
        u16_num_byte_to_write   =  bdp_i2c_devices[api_i2c_data.u8_device].u16_w_sizeof;    /* Number of byte for write. ( here always 1 ) */
        u8_device_address       =  bdp_i2c_devices[api_i2c_data.u8_device].u8_slave_address;/* Address of write device. */

        /* Call write-init function. */
        be_result = be_board_i2c_write_start(pu8_buffer, u16_num_byte_to_write, u8_device_address);
    }
    else if(api_i2c_data.u8_device >= 3U ) /* Call baro state machine. */
    {
        /* Call baro state machine. */
        be_result = be_board_drv_bmp085_state_machine();
    }
    return(be_result);
}

/* Read process start service function. */
BOARD_ERROR be_api_i2c_read_process_start(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t* pu8_buffer;
    uint16_t u16_num_byte_to_read;
    uint8_t u8_device_address;

    /* Read valid for GYRO, MAGN, ACC. */
    if(api_i2c_data.u8_device < 3U )
    {
        pu8_buffer              = &api_i2c_data.array[api_i2c_data.u8_device].data[0];      /* Pointer of start of read buffer. */
        u16_num_byte_to_read    =  bdp_i2c_devices[api_i2c_data.u8_device].u16_r_sizeof;    /* Number of byte to read. ( here always 6 ) */
        u8_device_address       =  bdp_i2c_devices[api_i2c_data.u8_device].u8_slave_address;/* Address of read device. */
        /* Call read-init function. */
        be_result = be_board_i2c_read_start(pu8_buffer, u16_num_byte_to_read, u8_device_address);
    }
    else if(api_i2c_data.u8_device == 3U ) /* Call baro state machine. */
    {
        /* End of baro state machine itteration. */
        api_i2c_data.u8_ready = 1U;
        GPIO_ResetBits( GPIOB, GPIO_Pin_12);/* for test only */
    }
    return(be_result);
}

