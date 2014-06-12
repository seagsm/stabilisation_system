#include "board_init.h"

/*
  Board init function.
*/
BOARD_ERROR be_board_init_main_init(void)
{
    uint16_t u16_step = 0U;
    BOARD_ERROR be_result = BOARD_ERR_OK;

    u16_board_adc_result[0] = 0U;/* It is temporery solution. */
    gv_board_sys_tick_init();
    /* Init interrupt priority group. */
    NVIC_init();
    /*TODO: It should be moved to suitable place. */
    __enable_irq();


    while (u16_step < BOARD_INIT_TOTAL_STEPS)
    {
        switch (u16_step)
		{
            case (0U):
                be_result = be_board_gpio_init();   /* Init GPIO. */
                break;
            case (1U):
                be_result = be_board_uart_init();   /* Init UART modules. */
                break;
            case (2U):
                be_result = be_board_adc_init();    /* Init ADC module. */
                break;
            case (3U):
                be_result = be_board_ppm_init();    /* Init PPM input capture. (Timer_4) */
                break;
            case (4U):
                be_result = be_board_pwm_init();    /* Init PWM chanels. (Timer_2 and Timer_3)*/
                break;
            case (5U):
                be_result = be_board_i2c_init();    /* Init I2C1 chanels. */
                break;
            case (6U):
                be_result = be_board_gyro_init();   /* Init gyro module. */
                break;
            case (7U):
                be_result = be_board_acc_init();    /* Init accelerometer module. */
                break;
            case (8U):
                be_result = be_board_mag_init();    /* Init magnetometer module. */
                break;
            case (9U):
                be_result = be_board_baro_init();    /* Init baro module. */
                break;
            case (10U):
                be_result = be_api_i2c_acquisition_init(); /* Init data acquisition process. It read chain of Gyro-Acc-Mag. */
                break;
            case (11U):
                be_result = be_api_main_loop_init();  /* Init of Timer_1 for main loop interrupt. */
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

    if(be_result == BOARD_ERR_OK)
    {
        v_board_state_set_required_state(BOARD_SYSTEM_READY_TO_RUN);
    }
    else
    {
        v_board_state_set_required_state(BOARD_SYSTEM_FAULT);
    }
    return(be_result);
}