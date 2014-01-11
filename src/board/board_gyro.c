
#include "board_gyro.h"

BOARD_3X_DATA board_gyro_data;

/* Detecting gyro module. */
BOARD_ERROR be_board_gyro_detect(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_board_drv_l3g4200d_detect();

    return(be_result);
}

/* Init gyro module. */
BOARD_ERROR be_board_gyro_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_board_drv_l3g4200d_init();

    return(be_result);
}

/* Read data from gyro module. */
BOARD_ERROR be_board_gyro_read(BOARD_3X_DATA *p_board_gyro_data)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_board_drv_l3g4200d_read(p_board_gyro_data);

    return(be_result);
}