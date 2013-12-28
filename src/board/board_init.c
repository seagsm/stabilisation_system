#include "board_init.h"

/*
  Board init function.
*/
BOARD_ERRORS board_init_main_init(void)
{
  BOARD_ERRORS be_result = BOARD_ERR_OK;

  board_state_set_state(BOARD_SYSTEM_INIT);

  return(be_result);
}