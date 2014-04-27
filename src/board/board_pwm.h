#ifndef BOARD_PWM_H
#define BOARD_PWM_H 1



#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_NVIC.h"


/* PWM period in microSecond. */
#define PWM_PERIOD_ANALOG          20000U /* One PPM frame is 20  milliSeconds.*/
#define PWM_PERIOD_DIGITAL         2300U  /* One PPM frame is 2.3 milliSeconds.*/
#define PWM_DUTY_INITIAL           1000U
#define PWM_DUTY_INITIAL_MIDDLE    1500U

BOARD_ERROR be_board_pwm_init(void);
static BOARD_ERROR be_TIMER2_PWM_channel_init(uint16_t u16_ch_number, uint16_t u16_pwm_period, uint16_t u16_pwm_duty);
void timer2_PWM_duty_CH1(uint16_t u16_duty);
void timer2_PWM_duty_CH2(uint16_t u16_duty);
void timer2_PWM_duty_CH3(uint16_t u16_duty);
void timer2_PWM_duty_CH4(uint16_t u16_duty);
static BOARD_ERROR be_TIMER3_PWM_channel_init(uint16_t u16_ch_number, uint16_t u16_pwm_period, uint16_t u16_pwm_duty );
void timer3_PWM_duty_CH1(uint16_t u16_duty);
void timer3_PWM_duty_CH2(uint16_t u16_duty);
void timer3_PWM_duty_CH3(uint16_t u16_duty);
void timer3_PWM_duty_CH4(uint16_t u16_duty);



#endif