
#include "board_ppm.h"

BOARD_CHANNEL_VALUE bc_channel_value_structure;

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
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

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

    /* Init TIMER4 base counter. */
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72U;/* 72 -> 1MHz counter frequence. */
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);


    /* TIM4 configuration: Input Capture mode. */
    if(u16_ch_number == 1U)
    {
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    }
    else if(u16_ch_number == 2U)
    {
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    }
    else if(u16_ch_number == 3U)
    {
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
    }
    else if(u16_ch_number == 4U)
    {
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
    }
    /* Timer input capture configuration. */
    TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity;
    TIM_ICInitStructure.TIM_ICSelection = TIM_TIxExternalCLKSource;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_divider;
    TIM_ICInitStructure.TIM_ICFilter    = ICFilter;

    TIM_ICInit(TIM4, &TIM_ICInitStructure);

    /* TIM enable counter */
    TIM_Cmd(TIM4, ENABLE);

    /* Enable the CCx Interrupt Request */
    if(u16_ch_number == 1U)
    {
        TIM_ITConfig(TIM4, TIM_IT_CC1|TIM_IT_Update, ENABLE);
    }
    else if(u16_ch_number == 2U)
    {
        TIM_ITConfig(TIM4, TIM_IT_CC2|TIM_IT_Update, ENABLE);
    }
    else if(u16_ch_number == 3U)
    {
        TIM_ITConfig(TIM4, TIM_IT_CC3|TIM_IT_Update, ENABLE);
    }
    else if(u16_ch_number == 4U)
    {
        TIM_ITConfig(TIM4, TIM_IT_CC4|TIM_IT_Update, ENABLE);
    }
    return(be_result);
}



void TIM4_IRQHandler(void)
{
    static uint32_t u32_overflow = 0U;
    static uint32_t u32_over_flag  = 0U;
    static uint32_t u32_channel_number  = 1U;
           uint16_t u16_current_period;

    if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
    {
        /* Clear TIM4 Capture compare interrupt pending bit */
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        /* OverfloW counter. */
        u32_overflow++;
        /* Set flag if overflow. */
        u32_over_flag=1U;
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
        u16_current_period = TIM_GetCapture3(TIM4);
        /* Select start PPM channel. It is first pulse after timeout more than 5000. */
        if((u32_over_flag != 0U)||(u16_current_period > 5000U))
        {
            u32_channel_number  = 1U;
        }
        else
        {   /* Fill of "bc_channel_value_structure" by value of each channel in acording with CH number. */
            switch(u32_channel_number)
            {
              case 1U:
                    bc_channel_value_structure.u16_channel_1_value = u16_current_period;
                    u32_channel_number++;/* TODO: should be optimized. */
                break;
              case 2U:
                    bc_channel_value_structure.u16_channel_2_value = u16_current_period;
                    u32_channel_number++;
                break;
              case 3U:
                    bc_channel_value_structure.u16_channel_3_value = u16_current_period;
                    u32_channel_number++;
                break;
              case 4U:
                    bc_channel_value_structure.u16_channel_4_value = u16_current_period;
                    u32_channel_number++;
                break;
              case 5U:
                    bc_channel_value_structure.u16_channel_5_value = u16_current_period;
                    u32_channel_number++;
                break;
              case 6U:
                    bc_channel_value_structure.u16_channel_6_value = u16_current_period;
                    u32_channel_number++;
                break;
              case 7U:
                    bc_channel_value_structure.u16_channel_7_value = u16_current_period;
                    u32_channel_number++;
                break;
              case 8U:
                    bc_channel_value_structure.u16_channel_8_value = u16_current_period;
                    u32_channel_number++;
                break;
              default:
               break;
            }
        }

        TIM4->CNT     = 0U;
        u32_overflow  = 0U;
        u32_over_flag = 0U;
    }

    if(TIM_GetITStatus(TIM4, TIM_IT_CC4) == SET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);
    }

}










