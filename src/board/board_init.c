#include "board_init.h"

/*
  Board init function.
*/
BOARD_ERROR board_init_main_init(void)
{
    uint16_t u16_step = 0U;
    BOARD_ERROR be_result = BOARD_ERR_OK;

    while (u16_step < BOARD_INIT_TOTAL_STEPS)
    {
        switch (u16_step)
		{
            case (0U):
                be_result = be_board_gpio_init(); /* Init GPIO. */
                break;
            case (1U):
                be_result = be_board_uart_init(); /* Init UART modules. */
                break;
            default:
                be_result = BOARD_ERR_ERROR;
                break;
        }
        /* If during initialisation something going wrong. */
        if(be_result != BOARD_ERR_OK)
        {
            /* TODO: Print u16_step like number of error step. */
            break;
        }
        u16_step++;
    }
    /* Init priority group. */
    /* board_nvic_init();*/
    /* Init TIMER modules. */
    /* board_timer_init();*/
    /* Init ADC. */
    /*  adc_init();*/
    /* Init I2C. */
    /* board_i2c_init();*/

    /* If no error lets switch system to init state. */
    if(be_result == BOARD_ERR_OK)
    {
        v_board_state_set_state(BOARD_SYSTEM_INIT);
    }
    return(be_result);
}