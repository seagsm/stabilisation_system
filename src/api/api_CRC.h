#ifndef API_CRC_H
#define API_CRC_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_dma.h"
#include "board_r_buff.h"


uint16_t gu16_api_CRC16_alg(uint16_t u16_start, uint16_t length);
uint8_t gu8_api_CRC8(uint16_t u16_start, uint16_t length);

#endif