#ifndef API_CMD_H
#define API_CMD_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_dma.h"
#include "api_packet_structure.h"
#include "api_CRC.h"
#include "api_pid.h"
#include "board_baro.h"
#include "api_baro.h"

#define API_CMD_MAX_SIZE_OF_LINE_BUFFER 32U

void api_cmd_reading_packet(void);
static BOARD_ERROR be_api_CMD_decoding_packet(void);
static BOARD_ERROR be_api_CMD_data_answer_i32(uint16_t u16_data_id);
static BOARD_ERROR be_api_CMD_data_answer_u64(uint16_t u16_data_id);
static BOARD_ERROR be_api_CMD_data_answer_quaternion(uint16_t u16_data_id);
static BOARD_ERROR be_api_CMD_data_answer_float_vector3d(uint16_t u16_data_id);
static BOARD_ERROR be_api_CMD_data_write_i32(uint16_t u16_data_id, int32_t i32_data_load);
static uint8_t su8_api_CMD_CRC8(uint8_t u8_start, uint8_t u8_length);



#endif