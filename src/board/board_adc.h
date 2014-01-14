#ifndef BOARD_ADC_H
#define BOARD_ADC_H 1



#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_gpio.h"
#include "board_sys_tick.h"

#define AIN_BARO_SENSOR_PORT    GPIOA
#define AIN_BARO_SENSOR_PIN     GPIO_Pin_4
#define AIN_BARO_SENSOR_CH      ADC_Channel_4

#define AIN_BATT_VOLT_PORT      GPIOA
#define AIN_BATT_VOLT_PIN       GPIO_Pin_5
#define AIN_BATT_VOLT_CH        ADC_Channel_5

#define AIN_BATT_CURRENT_PORT   GPIOA
#define AIN_BATT_CURRENT_PIN    GPIO_Pin_7
#define AIN_BATT_CURRENT_CH     ADC_Channel_7

#define AIN_INTERN_TEMPERATURE  ADC_Channel_16

/*
    Array for saving ADC data from channels.
    Data collected by DMA1 CH1.
*/
extern uint16_t u16_board_adc_result[4];

BOARD_ERROR be_board_adc_init(void);

static BOARD_ERROR be_board_adc_gpio_init(void);
static BOARD_ERROR be_board_adc_module_init(void);
static BOARD_ERROR be_board_adc_dma_init(void);
static BOARD_ERROR be_board_adc_interrupt_init(void);
void   DMA1_Channel1_IRQHandler(void);

#endif