
/* Code style rules:                */
/* 1. Code should be an ideal.      */
/* 2. An ideal code does not exist. */
/*                 Vadym Volokitin. */


#include "main.h"


#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <misc.h>



int main( void)
{
    BOARD_SYSTEM_STATE  bss_state;
    BOARD_ERROR         be_result = BOARD_ERR_OK;



    v_board_state_set_required_state(BOARD_SYSTEM_INIT);
    /*TODO:
                    Should be done calibration of PPM input for current minimum
                    and maximum value for each channel.
     */
    while(1U)
    {
        if(be_result == BOARD_ERR_ERROR)
        {
            v_board_state_set_required_state(BOARD_SYSTEM_FAULT);
        }
        bss_state = bss_board_state_get_required_state();
        switch (bss_state)
        {
            case BOARD_SYSTEM_INIT:
                v_board_state_update_current_state(BOARD_SYSTEM_INIT);
                be_result = be_board_init_main_init();      /* init main hardware moduls.*/
                break;
            case BOARD_SYSTEM_READY_TO_RUN:
                v_board_state_update_current_state(BOARD_SYSTEM_READY_TO_RUN);
#if 0
                be_result = be_board_system_init_unlock();  /* init all board system .*/
#endif
                /* Should be removed. Real setup inside  be_board_system_init_unlock() */
                v_board_state_set_required_state(BOARD_SYSTEM_RUN);

                break;

            case BOARD_SYSTEM_RUN:
                /* One time switch a board to RUN state. */
                if(bss_board_state_get_current_state() != BOARD_SYSTEM_RUN )
                {
                    /* Start main loop interrupt. */
                    be_api_main_loop_start();
                    v_board_state_update_current_state(BOARD_SYSTEM_RUN);
                }

                gv_board_sys_tick_delay(100U);
                /* timer2_PWM_duty_CH1(bc_channel_value_structure.u16_channel_1_value); *//* send data to servo. */

#if 0
                GPIO_SetBits( GPIOA, GPIO_Pin_12);
                gv_board_sys_tick_fast_delay(50U);
                GPIO_ResetBits( GPIOA, GPIO_Pin_12);
#endif
                /* Led. */
                GPIO_SetBits( GPIOB, GPIO_Pin_1);
                gv_board_sys_tick_fast_delay(50U);
                GPIO_ResetBits( GPIOB, GPIO_Pin_1);

            /*    gv_board_dma_send_packet(); */
                board_dma_send_buff();

                GPIO_SetBits( GPIOB, GPIO_Pin_1);
                gv_board_sys_tick_fast_delay(50U);
                GPIO_ResetBits( GPIOB, GPIO_Pin_1);
                break;
            case BOARD_SYSTEM_MOTOR_CALIBRATION:/* Calibration of motor ESD controller.*/
                v_board_state_update_current_state(BOARD_SYSTEM_MOTOR_CALIBRATION);
                be_result = be_board_system_init_motor_calibration();
                break;
            case BOARD_SYSTEM_FAULT:
                v_board_state_update_current_state(BOARD_SYSTEM_FAULT);
                GPIO_SetBits( GPIOB, GPIO_Pin_1);
                gv_board_sys_tick_delay(400U);
                GPIO_ResetBits( GPIOB, GPIO_Pin_1);
                gv_board_sys_tick_delay(400U);
                break;
            default:
                break;
        }
    }
}












#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
uint32_t assert_failed(uint8_t* file, uint32_t line)
{
    /*  User can add his own implementation to report the file name and line number,
        ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1U)
    {
        return(1U);
    }
}
#endif

