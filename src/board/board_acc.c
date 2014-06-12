


#include "board_acc.h"

/* Init acceerometer module. */
BOARD_ERROR be_board_acc_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = board_drv_adxl345_init();

    return(be_result);
}

/* Read acceerometer module. */
BOARD_ERROR be_board_acc_read(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t u8_buffer[6] = {0U};

    be_result = board_drv_adxl345_read(u8_buffer);

    return(be_result);
}