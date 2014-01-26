
#include "api_main_loop.h"

BOARD_U16_3X_DATA bu163d_api_main_loop_gyro_raw_data;
BOARD_U16_3X_DATA bu163d_api_main_loop_acce_raw_data;
BOARD_U16_3X_DATA bu163d_api_main_loop_magn_raw_data;


static void v_api_main_loop_process(void)
{
    GPIO_SetBits( GPIOB, GPIO_Pin_12);

    if(api_i2c_data.u8_ready == 1U)
    {
        v_api_main_loop_sensor_data_preprocessing();
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
    This function copy and convert data from api_i2c_data structure to
    sensor structure.
*/
static void v_api_main_loop_sensor_data_preprocessing(void)
{
/* api_i2c_data */
    bu163d_api_main_loop_gyro_raw_data.u16_X = (((uint16_t)  api_i2c_data.array[0].data[0]) << 8U) + ((uint16_t)api_i2c_data.array[0].data[1]);
    bu163d_api_main_loop_gyro_raw_data.u16_Y = (((uint16_t)  api_i2c_data.array[0].data[2]) << 8U) + ((uint16_t)api_i2c_data.array[0].data[3]);
    bu163d_api_main_loop_gyro_raw_data.u16_Z = (((uint16_t)  api_i2c_data.array[0].data[4]) << 8U) + ((uint16_t)api_i2c_data.array[0].data[5]);

    bu163d_api_main_loop_acce_raw_data.u16_X = (((uint16_t)  api_i2c_data.array[1].data[0]) << 8U) + ((uint16_t)api_i2c_data.array[1].data[1]);
    bu163d_api_main_loop_acce_raw_data.u16_Y = (((uint16_t)  api_i2c_data.array[1].data[2]) << 8U) + ((uint16_t)api_i2c_data.array[1].data[3]);
    bu163d_api_main_loop_acce_raw_data.u16_Z = (((uint16_t)  api_i2c_data.array[1].data[4]) << 8U) + ((uint16_t)api_i2c_data.array[1].data[5]);

    bu163d_api_main_loop_magn_raw_data.u16_X = (((uint16_t)  api_i2c_data.array[2].data[0]) << 8U) + ((uint16_t)api_i2c_data.array[2].data[1]);
    bu163d_api_main_loop_magn_raw_data.u16_Y = (((uint16_t)  api_i2c_data.array[2].data[2]) << 8U) + ((uint16_t)api_i2c_data.array[2].data[3]);
    bu163d_api_main_loop_magn_raw_data.u16_Z = (((uint16_t)  api_i2c_data.array[2].data[4]) << 8U) + ((uint16_t)api_i2c_data.array[2].data[5]);

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