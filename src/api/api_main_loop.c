
#include "api_main_loop.h"

BOARD_I16_3X_DATA bi163d_api_main_loop_gyro_raw_data;
BOARD_I16_3X_DATA bi163d_api_main_loop_acce_raw_data;
BOARD_I16_3X_DATA bi163d_api_main_loop_magn_raw_data;
float fl_quaternion[4] = {1.0f, 0.0f, 1.0f, .0f };

static float float_api_main_loop_old_time;
static float float_api_main_loop_now_time;
       float float_api_main_loop_sample_period;

static void v_api_main_loop_process(void)
{
    GPIO_SetBits( GPIOB, GPIO_Pin_12);

    if(api_i2c_data.u8_ready == 1U)
    {
        v_api_main_loop_sensor_data_preprocessing();
        be_api_i2c_acquisition_start();

        /* Sample period calculation. */
        float_api_main_loop_now_time      = (float)gu64_read_system_time();
        float_api_main_loop_sample_period = float_api_main_loop_now_time - float_api_main_loop_old_time;
        float_api_main_loop_old_time      = float_api_main_loop_now_time;
        /* Convert period from tick to seconds. */
        float_api_main_loop_sample_period = float_api_main_loop_sample_period / 1000.0f;

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

    madgwick_AccGyro(
                         deg2rad(b_float3d_api_main_loop_gyro_data.fl_X),
                         deg2rad(b_float3d_api_main_loop_gyro_data.fl_Y),
                         deg2rad(b_float3d_api_main_loop_gyro_data.fl_Z),
                                 b_float3d_api_main_loop_acce_data.fl_X,
                                 b_float3d_api_main_loop_acce_data.fl_Y,
                                 b_float3d_api_main_loop_acce_data.fl_Z
                    );
}
/*
    This function copy and convert data from api_i2c_data structure to
    sensor structure.
*/
static void v_api_main_loop_sensor_data_preprocessing(void)
{
    uint16_t u16_tmp;
    /* Convert api_i2c_data data buffer to int16 sensors data. */
    /* Gyro. */
    u16_tmp = (((uint16_t)  api_i2c_data.array[0].data[0]) << 8U) + ((uint16_t)api_i2c_data.array[0].data[1]);
    bi163d_api_main_loop_gyro_raw_data.i16_X = (int16_t) u16_tmp;
    u16_tmp = (((uint16_t)  api_i2c_data.array[0].data[2]) << 8U) + ((uint16_t)api_i2c_data.array[0].data[3]);
    bi163d_api_main_loop_gyro_raw_data.i16_Y = (int16_t) u16_tmp;
    u16_tmp = (((uint16_t)  api_i2c_data.array[0].data[4]) << 8U) + ((uint16_t)api_i2c_data.array[0].data[5]);
    bi163d_api_main_loop_gyro_raw_data.i16_Z = (int16_t) u16_tmp;
    /* Accelerometer. */
    u16_tmp = (((uint16_t)  api_i2c_data.array[1].data[1]) << 8U) + ((uint16_t)api_i2c_data.array[1].data[0]);
    bi163d_api_main_loop_acce_raw_data.i16_X = (int16_t) u16_tmp;
    u16_tmp = (((uint16_t)  api_i2c_data.array[1].data[3]) << 8U) + ((uint16_t)api_i2c_data.array[1].data[2]);
    bi163d_api_main_loop_acce_raw_data.i16_Y = (int16_t) u16_tmp;
    u16_tmp = (((uint16_t)  api_i2c_data.array[1].data[5]) << 8U) + ((uint16_t)api_i2c_data.array[1].data[4]);
    bi163d_api_main_loop_acce_raw_data.i16_Z = (int16_t) u16_tmp;
    /* Magnetometer. */
    u16_tmp = (((uint16_t)  api_i2c_data.array[2].data[0]) << 8U) + ((uint16_t)api_i2c_data.array[2].data[1]);
    bi163d_api_main_loop_magn_raw_data.i16_X = (int16_t) u16_tmp;
    u16_tmp = (((uint16_t)  api_i2c_data.array[2].data[2]) << 8U) + ((uint16_t)api_i2c_data.array[2].data[3]);
    bi163d_api_main_loop_magn_raw_data.i16_Y = (int16_t) u16_tmp;
    u16_tmp = (((uint16_t)  api_i2c_data.array[2].data[4]) << 8U) + ((uint16_t)api_i2c_data.array[2].data[5]);
    bi163d_api_main_loop_magn_raw_data.i16_Z = (int16_t) u16_tmp;

    /* Normalize sensor value to float. */
    v_api_data_normalising_gyro();
    v_api_data_normalising_acce();
    v_api_data_normalising_magn();
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