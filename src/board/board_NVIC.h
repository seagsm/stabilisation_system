#ifndef BOARD_NVIC_H
#define BOARD_NVIC_H 1


#include "stm32f10x.h"

/*  Define current priority and subpriority. */
/*  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_0 */
/*  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_1 */
/*  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_2 */
    #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_3    /* preemption 0-7 , sub 0-1 */
/*  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_4 */

/* USART1 *********************/
#define USART1_PRIORITY_GROUP 6
#define USART1_SUB_PRIORITY   0

/* USART2 */
#define USART2_PRIORITY_GROUP 5
#define USART2_SUB_PRIORITY   1

/* USART3 */
#define USART3_PRIORITY_GROUP 6
#define USART3_SUB_PRIORITY   1

/* TIMER1 INTERUPT PERIOD **********************/
#define TIMER1_PERIOD_INTERUPT_PRIORITY_GROUP       7
#define TIMER1_PERIOD_INTERUPT_SUB_PRIORITY_GROUP   1

/* TIMER2 CAPTURE */
#define TIMER2_CAPTURE_PRIORITY_GROUP 6
#define TIMER2_CAPTURE_SUB_PRIORITY   0
/* TIMER3 CAPTURE */
#define TIMER3_CAPTURE_PRIORITY_GROUP 6
#define TIMER3_CAPTURE_SUB_PRIORITY   1
/* TIMER4 CAPTURE ******************************/
#define TIMER4_CAPTURE_PRIORITY_GROUP 0
#define TIMER4_CAPTURE_SUB_PRIORITY   0

/* SYS TICK ************************************/
#define SYSTEM_TICK_PRIORITY_GROUP     1U
#define SYSTEM_TICK_SUB_PRIORITY_GROUP 0U

/* ADC *****************************************/
#define ADC_PRIORITY_GROUP 6
#define ADC_SUB_PRIORITY   0

/* I2C1*/
#define I2C1_EV_PRIORITY_GROUP      0
#define I2C1_EV_SUB_PRIORITY_GROUP  0
#define I2C1_ER_PRIORITY_GROUP      1
#define I2C1_ER_SUB_PRIORITY_GROUP  0

/* I2C2******************************************/
#define I2C2_EV_PRIORITY_GROUP      0
#define I2C2_EV_SUB_PRIORITY_GROUP  0
#define I2C2_ER_PRIORITY_GROUP      0
#define I2C2_ER_SUB_PRIORITY_GROUP  1


/* DMA ******************************************/
#define DMA1_Channel4_PRIORITY_GROUP      4
#define DMA1_Channel4_SUB_PRIORITY_GROUP  0



void NVIC_init(void);






#endif

