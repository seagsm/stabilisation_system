
#include "api_main_loop.h"


static void v_api_main_loop_process(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    static uint8_t u8_calibration = 0U;
    GPS_RECEIVER_STATE nav_state;

    if(api_i2c_data.u8_ready == 1U)
    {
        /* Convertind data from raw data array to sensors raw data. */
        v_api_data_prepr_sensor_data_preprocessing();

        if(bsc_board_baro_get_state() == CONVERSION_DONE)
        {
            /* Calculation of real pressure and real temperature.*/
            v_board_baro_data_compensation();

            /* Pressure filtration and altitude calculation. */

            /* Filter pressure using MA filter*/
            u32_board_baro_set_filtered_pressure(ui32_api_filters_ma_pressure(u32_board_baro_get_pressure()));

            /* Altitude estimation and BaroPid output calculation. */
            api_baro_altitude_estimation();

            /* Set BARO state machine start state. */
            be_board_baro_set_state(START_CONVERSION);
        }

        /* Start data acquisition. */
        be_api_i2c_acquisition_start();

        /* Calibration gyro. */
        if(u8_calibration == 0U)
        {
            /* callibration function will be called CALIBRATION_COUNT time to calculate callibration values. */
            u8_calibration = v_api_data_normalising_gyro_calibration();
        }
        else
        {
            /* Convert raw sensor data to float. */
            v_api_data_normalising();

            /* Start control frame. */
            v_api_main_loop_control_loop();
        }

        /* Copy received by UART3 data from DMA1_CH3 buffer to UART3_RX buffer. */
        be_board_dma_DMA1_CH3_buffer_copy_to_UART3_buffer();

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
        /* This is for reading GPS test only. Should be removed. */
        /* Copy received by UART3 data from DMA1_CH3 buffer to UART3_RX buffer. */
        
        /* Check if GPS support is ON. */
        if(u32_flag_GPS_on)
        {   
            /* Copy received data to UART buffer */
            be_board_dma_DMA1_CH3_buffer_copy_to_UART3_buffer();
            /* Decoding received data */
            be_result = api_ublox_msg_input_decode(USART3); 
            
            api_ublox_msg_get_nav_status(&nav_state); 
            
            /* Indication of received error. */
            if(be_result != BOARD_ERR_OK)
            {  
                be_result = api_led_flash_set_fast_period(400000U);
            }
            else
            {
                be_result = api_led_flash_set_fast_period(100000U);
            }  
        }        
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
    /* Init PID elements of BARO. */
    api_baro_PID_init();

    return(be_result);
}

/* Start main loop timer. */
BOARD_ERROR be_api_main_loop_start(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Start main loop timer. It will cause timer interrupt. */
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