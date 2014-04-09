#ifndef BOARD_NVIC_H
#define BOARD_NVIC_H 1


#include "stm32f10x.h"

/*  Define current priority and subpriority. */
/*  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_0 */
/*  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_1 */
/*  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_2 */
/*  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_3 */  /* preemption 0-7 , sub 0-1 */
  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_4       /* just priority 0-15*/
 /* It means that priority group can be 0-15, but subpriority only 0. */

/* I2C1 ***********************/
#define I2C1_EV_PRIORITY_GROUP      2U
#define I2C1_EV_SUB_PRIORITY_GROUP  0U

#define I2C1_ER_PRIORITY_GROUP      1U
#define I2C1_ER_SUB_PRIORITY_GROUP  0U

/* USART1 *********************/
#define USART1_PRIORITY_GROUP 6U
#define USART1_SUB_PRIORITY   0U

/* USART3 */
#define USART3_PRIORITY_GROUP 6U
#define USART3_SUB_PRIORITY   0U

/* TIMER1 MAIN LOOP INTERRUPT ******************/
#define TIMER1_PERIOD_INTERUPT_PRIORITY_GROUP       8U
#define TIMER1_PERIOD_INTERUPT_SUB_PRIORITY_GROUP   0U


/* TIMER4 CAPTURE ******************************/
#define TIMER4_CAPTURE_PRIORITY_GROUP 0U        /* PPM capture. */
#define TIMER4_CAPTURE_SUB_PRIORITY   0U

/* DMA ******************************************/

#define DMA1_Channel1_PRIORITY_GROUP      6U    /* ADC channels DMA transfer end.  */
#define DMA1_Channel1_SUB_PRIORITY_GROUP  0U

#define DMA1_Channel2_PRIORITY_GROUP      4U    /* UART3 DMA TX transfer end. GPS  */
#define DMA1_Channel2_SUB_PRIORITY_GROUP  0U

#define DMA1_Channel3_PRIORITY_GROUP      4U    /* UART3 DMA RX transfer end. GPS  */
#define DMA1_Channel3_SUB_PRIORITY_GROUP  0U

#define DMA1_Channel4_PRIORITY_GROUP      14U   /* UART1 DMA TX transfer end. COM1 */
#define DMA1_Channel4_SUB_PRIORITY_GROUP  0U

#define DMA1_Channel5_PRIORITY_GROUP      13U   /* UART1 DMA RX receive end. COM1 */
#define DMA1_Channel5_SUB_PRIORITY_GROUP  0U

#define DMA1_Channel6_PRIORITY_GROUP      3U    /* I2C1 TX DMA transfer end.       */
#define DMA1_Channel6_SUB_PRIORITY_GROUP  0U

#define DMA1_Channel7_PRIORITY_GROUP      3U    /* I2C1 RX DMA transfer end.       */
#define DMA1_Channel7_SUB_PRIORITY_GROUP  0U


void NVIC_init(void);

#endif

