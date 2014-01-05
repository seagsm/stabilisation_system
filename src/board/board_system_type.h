#ifndef BOARD_SYSTEM_TYPE_H
#define BOARD_SYSTEM_TYPE_H 1



#include "stm32f10x.h"

typedef enum
{
    CHANEL_0 = 0U,
    CHANEL_1 = 1U,
    CHANEL_2 = 2U,
    CHANEL_3 = 3U,
    CHANEL_4 = 4U
}   BOARD_CHANEL;

typedef enum
{
    BOARD_ERR_OK        = 0U,
    BOARD_ERR_ERROR     = 1U,
    BOARD_ERR_FULL      = 2U,
    BOARD_ERR_EMPTY     = 3U

}   BOARD_ERROR; /* variable tag be_xxx */

typedef struct
{
    uint16_t u16_channel_1_value;
    uint16_t u16_channel_2_value;
    uint16_t u16_channel_3_value;
    uint16_t u16_channel_4_value;
    uint16_t u16_channel_5_value;
    uint16_t u16_channel_6_value;
    uint16_t u16_channel_7_value;
    uint16_t u16_channel_8_value;
    uint16_t u16_channel_9_value;
} BOARD_CHANNEL_VALUE;


#endif