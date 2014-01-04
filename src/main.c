
#include "main.h"


#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <misc.h>



int main( void)
{
  BOARD_SYSTEM_STATE   bss_state;


  board_init_main_init();

  while(1U)
  {
    bss_state = bss_board_state_get_required_state();
    switch (bss_state)
    {
      case BOARD_SYSTEM_INIT:
        v_board_state_set_state(BOARD_SYSTEM_READY_TO_RUN);
      break;

      case BOARD_SYSTEM_READY_TO_RUN:
        v_board_state_set_state(BOARD_SYSTEM_RUN);
      break;

      case BOARD_SYSTEM_RUN:
      break;

      case BOARD_SYSTEM_FAULT:
      break;

      default:
      break;
    }
  }

}












#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
uint32_t assert_failed(uint8_t* file, uint32_t line)
{
    /*  User can add his own implementation to report the file name and line number,
        ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1U)
    {
        return(1U);
    }
}
#endif

