#ifndef BOARD_SYSTEM_TYPE_H
#define BOARD_SYSTEM_TYPE_H 1



#include "stm32f10x.h"

typedef enum
{
  BOARD_SYSTEM_INIT = 0U,
  BOARD_SYSTEM_RUN  = 1U
} board_system_state;

#endif