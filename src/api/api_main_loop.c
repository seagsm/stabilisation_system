
#include "api_main_loop.h"


static void v_api_main_loop_process(void)
{
    static uint8_t u8_calibration = 0U;
    static uint16_t u16_calibration_counter = 0U;
    static int32_t i32_calibration_summ[3] = {0};

    if(api_i2c_data.u8_ready == 1U)
    {
        /* Convertind data from raw data array to sensors raw data. */
        v_api_data_prepr_sensor_data_preprocessing();

        /* Start data acquisition. */
        be_api_i2c_acquisition_start();

        /* Calibration gyro. */
        if(u8_calibration == 0U)
        { /* bi163d_api_data_gyro_calibration_data; */

            i32_calibration_summ[0] = i32_calibration_summ[0] + bi163d_api_data_prepr_gyro_raw_data.i16_X;
            i32_calibration_summ[1] = i32_calibration_summ[1] + bi163d_api_data_prepr_gyro_raw_data.i16_Y;
            i32_calibration_summ[2] = i32_calibration_summ[2] + bi163d_api_data_prepr_gyro_raw_data.i16_Z;
            u16_calibration_counter++;
            if(u16_calibration_counter >= CALIBRATION_COUNT)
            {
                i32_calibration_summ[0] = i32_calibration_summ[0] / (int32_t)CALIBRATION_COUNT;
                i32_calibration_summ[1] = i32_calibration_summ[1] / (int32_t)CALIBRATION_COUNT;
                i32_calibration_summ[2] = i32_calibration_summ[2] / (int32_t)CALIBRATION_COUNT;
                bi163d_api_data_gyro_calibration_data.i16_X = (int16_t)i32_calibration_summ[0] / 2;
                bi163d_api_data_gyro_calibration_data.i16_Y = (int16_t)i32_calibration_summ[1] / 2;
                bi163d_api_data_gyro_calibration_data.i16_Z = (int16_t)(i32_calibration_summ[2]/ 2);
                u8_calibration = 1U;
            }
        }
        else
        {
            /* Convert raw sensor data to float. */
            v_api_data_normalising();

            /* Start control frame. */
            v_api_main_loop_control_loop();
        }
        /* Copy received by UART1 data from DMA1_CH5 buffer to UART1_RX buffer. */
        be_board_dma_DMA1_CH5_buffer_copy_to_UART1_buffer();

        /* TODO: function name should be fixed. */
        /* Read and decode packets from UART1 RX buffer.*/
        api_cmd_reading_packet();

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
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Get body wind angle in degree. */
    be_result = be_api_body_angle_calculation();

    if(be_result == BOARD_ERR_OK)
    {
        /* Call  calculation of next frame of PDF. It body angles. */
        api_pid_update_frame();

        /* Call motor and servo control. It use data from previous step. */
        api_end_device_update();
    }
}

/* Set main loop timet period interrupt and do first data acquisition. */
BOARD_ERROR be_api_main_loop_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_board_main_loop_init(PERIOD_OF_MAIN_LOOP);

    /* Get data first time after start. */
    be_api_i2c_acquisition_start();

    /* Init PIDs elements. */
    api_pid_init();

    return(be_result);
}

/* Start main loop timer. */
BOARD_ERROR be_api_main_loop_start(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_board_main_loop_start();
    return(be_result);
}

/* Timer 1 interrupt handler. */
void TIM1_UP_IRQHandler(void)
{
    GPIO_SetBits( GPIOB, GPIO_Pin_12);


    if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {

        /* Clear flags. */
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
        /* Start main loop interrupt function.*/
        v_api_main_loop_process();
    }
   GPIO_ResetBits( GPIOB, GPIO_Pin_12);
}