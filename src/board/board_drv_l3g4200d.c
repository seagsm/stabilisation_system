
#include "board_drv_l3g4200d.h"

BOARD_ERROR be_board_drv_l3g4200d_detect(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t u8_device_id;

    /**/
    volatile uint32_t u32_i = 0xFFFFFFU;

    gv_board_sys_tick_fast_delay(25U);
    board_i2c_DMA_read(L3G4200D_ADDRESS, L3G4200D_WHO_AM_I, 1U, &u8_device_id);

    while(MasterReceptionComplete == 0U)
    {}
     u8_device_id =   u8_GyroId;
    if (u8_device_id != L3G4200D_ID)
    {
      be_result = BOARD_ERR_ERROR;
    }
    return (be_result);
}

BOARD_ERROR be_board_drv_l3g4200d_config(uint16_t u16_lpf)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t     u8_mpuLowPassFilter;
    switch (u16_lpf)
    {
        case 32:
            u8_mpuLowPassFilter = L3G4200D_DLPF_32HZ;
            break;
        case 54:
            u8_mpuLowPassFilter = L3G4200D_DLPF_54HZ;
            break;
        case 78:
            u8_mpuLowPassFilter = L3G4200D_DLPF_78HZ;
            break;
        case 93:
            u8_mpuLowPassFilter = L3G4200D_DLPF_93HZ;
            break;
        default:
            be_result = BOARD_ERR_ERROR;
            break;
    }
    if (be_result == BOARD_ERR_OK)
    {
        be_result = board_i2c_write(L3G4200D_ADDRESS, L3G4200D_CTRL_REG1, L3G4200D_POWER_ON | u8_mpuLowPassFilter);
    }
    return (be_result);
}

BOARD_ERROR be_board_drv_l3g4200d_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    gv_board_sys_tick_fast_delay(100U);
    /* setup gyro rate, see datasheet*/
    be_result = board_i2c_write(L3G4200D_ADDRESS, L3G4200D_CTRL_REG4, BOARD_DRV_GYRO_RATE);
    if (be_result == BOARD_ERR_OK)
    {
        gv_board_sys_tick_fast_delay(5U);
        be_result |= board_i2c_write(L3G4200D_ADDRESS, L3G4200D_CTRL_REG1, L3G4200D_POWER_ON | BOARD_DRV_LOWPASS_FILTER_RATE);
    }
    gv_board_sys_tick_fast_delay(5U);

    return(be_result);
}

BOARD_ERROR be_board_drv_l3g4200d_align(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    /* Any align is not neccessary. */
    return(be_result);
}


/* Read 3 gyro values into buffer. */
BOARD_ERROR be_board_drv_l3g4200d_read(BOARD_3X_DATA *p_board_drv_gyro_data)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t buf[6];

    be_result = board_i2c_DMA_read(L3G4200D_ADDRESS, L3G4200D_GYRO_OUT, 6U, buf);
/*
    p_board_drv_gyro_data->u16_X = (((uint16_t)  buf[0]) << 8U) + ((uint16_t)buf[1]);
    p_board_drv_gyro_data->u16_Y = (((uint16_t)  buf[2]) << 8U) + ((uint16_t)buf[3]);
    p_board_drv_gyro_data->u16_Z = (((uint16_t)  buf[4]) << 8U) + ((uint16_t)buf[5]);
*/
    be_result = BOARD_ERR_OK;
    return(be_result);
}






