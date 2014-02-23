


#include "board_adc.h"

uint16_t u16_board_adc_result[4];

BOARD_ERROR be_board_adc_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result |= be_board_adc_gpio_init();
    be_result |= be_board_adc_dma_init();
    be_result |= be_board_adc_interrupt_init();
    be_result |= be_board_adc_module_init();

    /* Start of ADC conversion and DMA transfer. */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    return(be_result);
}


static BOARD_ERROR be_board_adc_interrupt_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* Setup DMA end of transfer interrupt. */
    NVIC_InitStructure.NVIC_IRQChannel = (unsigned char)DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_Channel1_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_Channel1_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);

    return(be_result);
}

static BOARD_ERROR be_board_adc_dma_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    DMA_InitTypeDef DMA_InitStructure;

    /* Enable DMA1 clock. */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    /* Configure DMA1 - Channel1. */
    DMA_DeInit(DMA1_Channel1); /* Set DMA registers to default values. */
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t)& ADC1->DR;                 /* Address of peripheral the DMA must map to   */
    DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)& u16_board_adc_result[0];  /* Variable to which ADC values will be stored */
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize            = 4U;                                   /* Buffer size (4 because we using 4 channels) */
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;

    /* Initialise the DMA */
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    return(be_result);
}

/* This function init ADC module and callibrates it. */
static BOARD_ERROR be_board_adc_module_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    ADC_InitTypeDef ADC_InitStructure;

    RCC_ADCCLKConfig(RCC_PCLK2_Div8); /* setup ADC speed. */

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    ADC_InitStructure.ADC_Mode                  = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode          = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode    = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv      = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign             = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel          = 4U; /* amount of chanells in set. */
    ADC_Init(ADC1, &ADC_InitStructure);

    /* ADC1 regular channels configuration for 4 channels. */
    ADC_RegularChannelConfig(ADC1, AIN_BARO_SENSOR_CH,      1U, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, AIN_BATT_VOLT_CH,        2U, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, AIN_BATT_CURRENT_CH,     3U, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, AIN_INTERN_TEMPERATURE,  4U, ADC_SampleTime_239Cycles5);

    /* Enable DMA */
	ADC_DMACmd(ADC1, ENABLE);
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* Reset ADC1 calibration. */
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1))
    {}

    /* Start ADC1 calibration. */
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1))
    {}

    return(be_result);
}

/* This function init ADC channel GPIO pins us AIN. */
static BOARD_ERROR be_board_adc_gpio_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result |= be_board_pin_init( AIN_BARO_SENSOR_PORT, AIN_BARO_SENSOR_PIN,  GPIO_Speed_10MHz,GPIO_Mode_AIN);
    be_result |= be_board_pin_init( AIN_BATT_VOLT_PORT,   AIN_BATT_VOLT_PIN,    GPIO_Speed_10MHz,GPIO_Mode_AIN);
    be_result |= be_board_pin_init( AIN_BATT_CURRENT_PORT,AIN_BATT_CURRENT_PIN, GPIO_Speed_10MHz,GPIO_Mode_AIN);

    return(be_result);
}


void DMA1_Channel1_IRQHandler(void)
{  /*4 ch - 110uS, int function min 3,6 uS. */

   DMA_ClearITPendingBit( DMA1_IT_TC1);
   DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, DISABLE);
/*
   if(curr_sample<OVER_SAMPL)
   {
   ADC_SUM[0]+=ADC_VAL[0];
   ADC_SUM[1]+=ADC_VAL[1];
   ADC_SUM[2]+=ADC_VAL[2];
   ADC_SUM[3]+=ADC_VAL[3];
   ADC_SUM[4]+=ADC_VAL[4];
   ADC_SUM[5]+=ADC_VAL[5];
   ADC_SUM[6]+=ADC_VAL[6];
   ADC_SUM[7]+=ADC_VAL[7];
   }
   curr_sample++;
*/
   DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);
/*
   if(curr_sample>=OVER_SAMPL)
   DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, DISABLE);
*/
}








