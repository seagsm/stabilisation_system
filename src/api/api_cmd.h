#ifndef API_CMD_H
#define API_CMD_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_dma.h"
#include "api_packet_structure.h"
#include "api_CRC.h"

#define API_CMD_MAX_SIZE_OF_LINE_BUFFER 32U

void api_cmd_reading_packet(void);
static BOARD_ERROR be_api_CMD_decoding_packet(void);
static uint8_t su8_api_CMD_CRC8(uint8_t u8_start, uint8_t u8_length);



#endif