
#include "api_main_loop.h"




static void v_api_main_loop_process(void)
{
    GPIO_SetBits( GPIOB, GPIO_Pin_12);

    if(api_i2c_data.u8_ready == 1U)
    {
        v_api_main_loop_sensor_data_copy();
        be_api_i2c_acquisition_start();
        v_api_main_loop_control_loop();
    }
    else
    {
        GPIO_SetBits( GPIOA, GPIO_Pin_12);
        gv_board_sys_tick_fast_delay(50U);
        GPIO_ResetBits( GPIOA, GPIO_Pin_12);
    }
}

/* This function calculate control lool of all system.*/
static void v_api_main_loop_control_loop(void)
{

}
/*
    This function convert and copy data from api_i2c_data structure to
    sensor structure.
*/
static void v_api_main_loop_sensor_data_copy(void)
{


}




BOARD_ERROR be_api_main_loop_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_board_main_loop_init(PERIOD_OF_MAIN_LOOP);
    be_api_i2c_acquisition_start();

    return(be_result);
}

BOARD_ERROR be_api_main_loop_start(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_board_main_loop_start();
    return(be_result);
}

/* Timer 1 interrupt handler. */
void TIM1_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
        /* Start main loop interrupt function.*/
        v_api_main_loop_process();
        /* Clear flags. */
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}