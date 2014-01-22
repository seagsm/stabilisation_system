#include "board_mag.h"



/* Init magnetometer module. */
BOARD_ERROR be_board_mag_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_board_drv_hmc5883_init();

    return(be_result);
}

/* Read magnetometer module. */
BOARD_ERROR be_board_mag_read(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_board_drv_hmc5883_read();

    return(be_result);
}