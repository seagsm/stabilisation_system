
#include "board_sys_tick.h"


static volatile uint64_t u64_system_time = 0U;



void gv_board_sys_tick_init(void)
{
  /* Init interrupt eatch milliSecond. */
  SysTick_Config(SystemCoreClock / 1000U);

  /* Set SysTick interrupt priority. */
  NVIC_SetPriority (SysTick_IRQn, 0U);
}

/* System tick interrupt handler. */
void SysTick_Handler(void)
{
   u64_system_time++;
}

/* With function return system time in milliSeconds. */
static uint64_t gu64_read_system_time(void)
{
  return(u64_system_time);
}

/* With finction provide blocked system delay. Input in milliSecond. */
void gv_board_sys_tick_delay(uint64_t u64_delay_time)
{
  volatile uint64_t u64_temp;

  u64_temp = gu64_read_system_time();
  u64_temp = u64_temp + u64_delay_time;
  while(gu64_read_system_time() < u64_temp )
  {}
}

/**
  * fast delay .
  */
void gv_board_sys_tick_fast_delay( uint32_t u32_val)
{
    while(u32_val > 0U)
    {
        __NOP();
        u32_val--;
    }
}




