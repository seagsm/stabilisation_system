

#include "board_system_init.h"



/* This function should wait unlock sequence from transmitter. */
static BOARD_ERROR be_board_system_init_unlock(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    BOARD_SYSTEM_INIT_UNLOCK_STATE bl_unlock_state = BOARD_SYSTEM_INIT_LOCKED;

    if( /* UNLOCK RUN - position of left stick right-down, right stick left-down. */
        (bc_channel_value_structure.u16_channel_3_value < CHANNEL_3_UNLOCK_RUN)&&
        (bc_channel_value_structure.u16_channel_1_value < CHANNEL_1_UNLOCK_RUN)&&
        (bc_channel_value_structure.u16_channel_2_value < CHANNEL_2_UNLOCK_RUN)&&
        (bc_channel_value_structure.u16_channel_4_value > CHANNEL_4_UNLOCK_RUN)
      )
    {
        bl_unlock_state = BOARD_SYSTEM_INIT_UNLOCK_RUN;
    }

    if( /* UNLOCK CALIBRATION - position of left stick right-up, right stick-up. */
        (bc_channel_value_structure.u16_channel_3_value > CHANNEL_3_UNLOCK_CALIBRATION)&&
        (bc_channel_value_structure.u16_channel_2_value > CHANNEL_2_UNLOCK_CALIBRATION)&&
        (bc_channel_value_structure.u16_channel_4_value > CHANNEL_4_UNLOCK_CALIBRATION)
      )
    {
        bl_unlock_state = BOARD_SYSTEM_INIT_UNLOCK_CALIBRATION;
    }

    if(bl_unlock_state == BOARD_SYSTEM_INIT_UNLOCK_RUN)
    {
        v_board_state_set_required_state(BOARD_SYSTEM_RUN);
        /* Turn LED ON-OFF-ON-OFF-ON.*/
        GPIO_SetBits( GPIOB, GPIO_Pin_1);
        gv_board_sys_tick_delay(500U);
        GPIO_ResetBits( GPIOB, GPIO_Pin_1);
        gv_board_sys_tick_delay(300U);
        GPIO_SetBits( GPIOB, GPIO_Pin_1);
        gv_board_sys_tick_delay(500U);
        GPIO_ResetBits( GPIOB, GPIO_Pin_1);
        gv_board_sys_tick_delay(300U);
        GPIO_SetBits( GPIOB, GPIO_Pin_1);
    }
    else if(bl_unlock_state == BOARD_SYSTEM_INIT_UNLOCK_CALIBRATION)
    {
        v_board_state_set_required_state(BOARD_SYSTEM_MOTOR_CALIBRATION);
        /* Turn LED ON-OFF.*/
        GPIO_SetBits( GPIOB, GPIO_Pin_1);
        gv_board_sys_tick_delay(1000U);
        GPIO_ResetBits( GPIOB, GPIO_Pin_1);
        gv_board_sys_tick_delay(600U);
    }
    return(be_result);
}

BOARD_ERROR be_board_system_init_motor_calibration(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Should be used motor channels only. */
    timer2_PWM_duty_CH1(bc_channel_value_structure.u16_channel_3_value);
    timer2_PWM_duty_CH2(bc_channel_value_structure.u16_channel_3_value);
    timer2_PWM_duty_CH3(bc_channel_value_structure.u16_channel_3_value);
    timer2_PWM_duty_CH4(bc_channel_value_structure.u16_channel_3_value);
    timer3_PWM_duty_CH1(bc_channel_value_structure.u16_channel_3_value);
    timer3_PWM_duty_CH2(bc_channel_value_structure.u16_channel_3_value);
    timer3_PWM_duty_CH3(bc_channel_value_structure.u16_channel_3_value);

    /* Check if calibration done. */
    be_result = be_board_system_init_unlock();

    return(be_result);
}





