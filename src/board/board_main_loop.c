


#include "board_main_loop.h"

/*  Start of Tiner 1 and allow Timer 1 interrupt */
BOARD_ERROR be_board_main_loop_start(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    TIM_Cmd(TIM1, ENABLE);
    NVIC_EnableIRQ(TIM1_UP_IRQn);

    return(be_result);
}

BOARD_ERROR be_board_main_loop_init(uint16_t u16_period_of_interupt)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_board_main_loop_TIMER1_interupt_period_init(u16_period_of_interupt);

    return(be_result);
}

/* Function do initialisation of TImer_1. */
static BOARD_ERROR be_board_main_loop_TIMER1_interupt_period_init(uint16_t u16_period_of_interupt)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    NVIC_InitTypeDef   NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    /* Enable the TIM1 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = (unsigned char)TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = TIMER1_PERIOD_INTERUPT_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority         = TIMER1_PERIOD_INTERUPT_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Init Timer1 like PWM. */
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_TIM1, ENABLE);

    /* Time Base configuration */
    TIM_TimeBaseStructure.TIM_Period        = u16_period_of_interupt;
    TIM_TimeBaseStructure.TIM_Prescaler     = 72U;          /* Ftimer=fsys/Prescaler,for Prescaler=72 ,Ftimer=1MHz */
    TIM_TimeBaseStructure.TIM_ClockDivision = 0U;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);             /* Clear pending interrupt. */
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    return(be_result);
}