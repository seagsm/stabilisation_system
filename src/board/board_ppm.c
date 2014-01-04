
#include "board_ppm.h"

static volatile uint16_t IC4ReadValue = 0U;

/* This function initialise PPM capture module. */
BOARD_ERROR be_board_ppm_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_TIMER4_CAPTURE_channel_init(PPM_TIM4_CH3, TIM_ICSelection_DirectTI, TIM_ICPolarity_Rising, TIM_ICPSC_DIV1,0x0FU);

    return(be_result);
}


static BOARD_ERROR be_TIMER4_CAPTURE_channel_init(
                                                    uint16_t u16_ch_number,
                                                    uint16_t TIM_TIxExternalCLKSource,
                                                    uint16_t TIM_ICPolarity,
                                                    uint16_t TIM_ICPSC_divider,
                                                    uint16_t ICFilter
                                                 )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /*TODO: Extra initialization should be removed. (Extra chanel and GPIO.)*/

    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;


    /*  GPIO  */
    /* GPIOB clock enable (for timer3) */
    /* Connect clock to port B for ch1 (PB6),ch2 (PB7),ch3 (PB8), ch4(PB9). */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    if(u16_ch_number==1U)
    {
        /* TIM4 channel 1 pin (PB.06) configuration */
        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
    }
    else if(u16_ch_number==2U)
    {
        /* TIM4 channel 2 pin (PB.07) configuration */
        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
    }
    else if(u16_ch_number==3U)
    {
        /* TIM4 channel 3 pin (PB.08) configuration */
        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
    }
    else if(u16_ch_number==4U)
    {
        /* TIM4 channel 4 pin (PB.09) configuration */
        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
    }
    else
    {
        be_result = BOARD_ERR_ERROR;
    }

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*   NVIC  */
    /* Enable the TIM4 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = (unsigned char)TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIMER4_CAPTURE_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIMER4_CAPTURE_SUB_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*  TIMER  */
    /* TIM4 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /* TIM4 configuration: Input Capture mode. */
    if(u16_ch_number==1U)
    {
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    }
    else if(u16_ch_number==2U)
    {
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    }
    else if(u16_ch_number==3U)
    {
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
    }
    else if(u16_ch_number==4U)
    {
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
    }
    TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity;
    TIM_ICInitStructure.TIM_ICSelection = TIM_TIxExternalCLKSource;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_divider;
    TIM_ICInitStructure.TIM_ICFilter    = ICFilter;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    /* TIM enable counter */
    TIM_Cmd(TIM4, ENABLE);
    /* Enable the CCx Interrupt Request */
    if(u16_ch_number==1U)
    {
        TIM_ITConfig(TIM4, TIM_IT_CC1|TIM_IT_Update, ENABLE);
    }
    else if(u16_ch_number==2U)
    {
        TIM_ITConfig(TIM4, TIM_IT_CC2|TIM_IT_Update, ENABLE);
    }
    else if(u16_ch_number==3U)
    {
        TIM_ITConfig(TIM4, TIM_IT_CC3|TIM_IT_Update, ENABLE);
    }
    else if(u16_ch_number==4U)
    {
        TIM_ITConfig(TIM4, TIM_IT_CC4|TIM_IT_Update, ENABLE);
    }

    return(be_result);
}



void TIM4_IRQHandler(void)
{
    volatile static  int32_t c_overflow = 0U;
    volatile static uint32_t over_flag  = 0U;

    if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
    {
        /* Clear TIM3 Capture compare interrupt pending bit */
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        /* OverfloW counter. */
        c_overflow++;
        /* Set flag if overflow. */
        over_flag=1U;
    }

    if(TIM_GetITStatus(TIM4, TIM_IT_CC1) == SET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
    }

    if(TIM_GetITStatus(TIM4, TIM_IT_CC2) == SET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);
    }

    if(TIM_GetITStatus(TIM4, TIM_IT_CC3) == SET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);
        /* PPM input connected to CH3 .*/
        /* Get capture value. */
        IC4ReadValue = TIM_GetCapture2(TIM4);
    }

    if(TIM_GetITStatus(TIM4, TIM_IT_CC4) == SET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);
    }
    over_flag=0U;
}







