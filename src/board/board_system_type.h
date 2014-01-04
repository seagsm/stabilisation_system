#ifndef BOARD_SYSTEM_TYPE_H
#define BOARD_SYSTEM_TYPE_H 1



#include "stm32f10x.h"

typedef enum
{
    BOARD_ERR_OK        = 0U,
    BOARD_ERR_ERROR     = 1U,
    BOARD_ERR_FULL      = 2U,
    BOARD_ERR_EMPTY     = 3U,

}   BOARD_ERROR; /* variable tag be_xxx */




#endif