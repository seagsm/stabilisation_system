
#include "main.h"


#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <misc.h>

/*board_system_state   bss_current_state; */

int main( void)
{
  static board_system_state   bss_current_state;
  bss_current_state = BOARD_SYSTEM_INIT;

   /* main_init(); */

  while(1U)
  {
    switch (bss_current_state)
    {
      case BOARD_SYSTEM_INIT:
      break;

      case BOARD_SYSTEM_RUN:
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1U)
  {
    return(1U);
  }

}
#endif

