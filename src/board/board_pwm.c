


#include "board_pwm.h"




BOARD_ERROR be_board_pwm_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_TIMER2_PWM_channel_init(CHANEL_1,PWM_PERIOD_ANALOG,PWM_DUTY_INITIAL);
    be_TIMER2_PWM_channel_init(CHANEL_2,PWM_PERIOD_ANALOG,PWM_DUTY_INITIAL);
    be_TIMER2_PWM_channel_init(CHANEL_3,PWM_PERIOD_ANALOG,PWM_DUTY_INITIAL);
    be_TIMER2_PWM_channel_init(CHANEL_4,PWM_PERIOD_ANALOG,PWM_DUTY_INITIAL_MIDDLE);

    /* Start TIMER2. */
    TIM_Cmd(TIM2, ENABLE);

    be_TIMER3_PWM_channel_init(CHANEL_1,PWM_PERIOD_DIGITAL,PWM_DUTY_INITIAL);
    be_TIMER3_PWM_channel_init(CHANEL_2,PWM_PERIOD_DIGITAL,PWM_DUTY_INITIAL);
    /* PB0 */
    be_TIMER3_PWM_channel_init(CHANEL_3,PWM_PERIOD_DIGITAL,PWM_DUTY_INITIAL);
    /* PB1 */
    /*  be_TIMER3_PWM_channel_init(4,PWM_PERIOD_TIMER2,PWM_DUTY_INITIAL_TIMER2); // Pin connected to led.*/
    /* Start TIMER4. */
    TIM_Cmd(TIM3, ENABLE);

    return(be_result);
}


static BOARD_ERROR be_TIMER2_PWM_channel_init(
                                                uint16_t u16_ch_number,  /* CH number. */
                                                uint16_t u16_pwm_period, /* microSec */
                                                uint16_t u16_pwm_duty    /* microSec */
                                             )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    /* Connect clock to port A */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

    /* Duty cycle start value */
    TIM_OCInitStructure.TIM_Pulse       = u16_pwm_duty;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;

    /* Init Timer2 like PWM. */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* Time Base configuration */
    TIM_TimeBaseStructure.TIM_Period        = u16_pwm_period;   /* PWM period */
    TIM_TimeBaseStructure.TIM_Prescaler     = 72U;          /* Ftimer=Fsys/Prescaler,for Fsys=72MHz and Prescaler=72 ,Ftimer=1MHz */
    TIM_TimeBaseStructure.TIM_ClockDivision = 0U;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    if(u16_ch_number == 1U)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  /* PA0 -> TIM2_CH1 */
        TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    }
    else if(u16_ch_number == 2U)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  /* PA1 -> TIM2_CH2 */
        TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    }
    else if(u16_ch_number == 3U)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  /* PA2 -> TIM2_CH3 */
        TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    }
    else if(u16_ch_number == 4U)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;  /* PA3 -> TIM2_CH4 */
        TIM_OC4Init(TIM2, &TIM_OCInitStructure);
    }
    else
    {
        be_result = BOARD_ERR_ERROR;
    }
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    return(be_result);
}

/* Change duty, microSeconds. */
void timer2_PWM_duty_CH1(uint16_t u16_duty)
{
  TIM_SetCompare1(TIM2, u16_duty);
}

void timer2_PWM_duty_CH2(uint16_t u16_duty)
{
  TIM_SetCompare2(TIM2, u16_duty);
}

void timer2_PWM_duty_CH3(uint16_t u16_duty)
{
  TIM_SetCompare3(TIM2, u16_duty);
}

void timer2_PWM_duty_CH4(uint16_t u16_duty)
{
  TIM_SetCompare4(TIM2, u16_duty);
}

/* Init timer 3 */
static BOARD_ERROR be_TIMER3_PWM_channel_init(
                                                uint16_t u16_ch_number,
                                                uint16_t u16_pwm_period, /* microSec */
                                                uint16_t u16_pwm_duty    /* microSec */
                                             )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    /* Connect clock to port A for ch 1(PA6) and ch2(PA7). */
    if((u16_ch_number == 1U)||(u16_ch_number == 2U))
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;
        TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    }
    else if((u16_ch_number == 3U)||(u16_ch_number == 4U))
    {
        /* Connect clock to port B for ch 3(PB0) and ch4(PB1). */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;
        TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    }
    else
    {
        be_result = BOARD_ERR_ERROR;
    }
    /* Duty cycle start value */
    TIM_OCInitStructure.TIM_Pulse       = u16_pwm_duty;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;

    /* Init Timer3 like PWM. */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    /* Time Base configuration. */
    TIM_TimeBaseStructure.TIM_Period        = u16_pwm_period;
    TIM_TimeBaseStructure.TIM_Prescaler     = 72U;         /* Ftimer=Fsys/Prescaler,for Fsys=72MHz and Prescaler=72, Ftimer=1MHz. */
    TIM_TimeBaseStructure.TIM_ClockDivision = 0U;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    if(u16_ch_number == 1U)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  /*PA6 -> TIM3_CH1 */
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    }
    else if(u16_ch_number == 2U)
    {   /* TODO: PWM should be reconected to PB5. */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;  /*PA7 -> TIM3_CH2 */
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    }
    else if(u16_ch_number == 3U)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  /*PB0 -> TIM3_CH3 */
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    }
    else if(u16_ch_number == 4U)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  /*PB1 -> TIM3_CH4 */
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    }
    else
    {
        be_result = BOARD_ERR_ERROR;
    }
    return(be_result);
}

/* Change duty, microSeconds. */
void timer3_PWM_duty_CH1(uint16_t u16_duty)
{
    TIM_SetCompare1(TIM3, u16_duty);
}

void timer3_PWM_duty_CH2(uint16_t u16_duty)
{
    TIM_SetCompare2(TIM3, u16_duty);
}

void timer3_PWM_duty_CH3(uint16_t u16_duty)
{
    TIM_SetCompare3(TIM3, u16_duty);
}

void timer3_PWM_duty_CH4(uint16_t u16_duty)
{
    TIM_SetCompare4(TIM3, u16_duty);
}


