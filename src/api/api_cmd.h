#ifndef API_CMD_H
#define API_CMD_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_dma.h"
#include "api_packet_structure.h"
#include "api_CRC.h"
#include "api_pid.h"
#include "board_baro_bmp085.h"
#include "api_baro.h"
#include "board_packet.h"


#define API_CMD_MAX_SIZE_OF_LINE_BUFFER 32U

#define API_CMD_FIRST_POSITION      3
#define API_CMD_SECOND_POSITION     7
#define API_CMD_THIRD_POSITION      11
#define API_CMD_FOURTH_POSITION     15
#define API_CMD_FIFTH_POSITION      19
#define API_CMD_SIXTH_POSITION      23

       BOARD_ERROR be_api_cmd_communication(void); /* Just start this function to make next loop of communication. */
static BOARD_ERROR be_api_cmd_reading_packet(void);
static BOARD_ERROR be_api_CMD_decoding_packet(void);
static BOARD_ERROR be_api_CMD_data_answer_i32(uint16_t u16_data_id);
static BOARD_ERROR be_api_CMD_data_answer_u64(uint16_t u16_data_id);
static BOARD_ERROR be_api_CMD_data_answer_quaternion(uint16_t u16_data_id);
static BOARD_ERROR be_api_CMD_data_answer_float_vector3d(uint16_t u16_data_id);
static BOARD_ERROR be_api_CMD_data_answer_float_nav_data(uint16_t u16_data_id);
static BOARD_ERROR be_api_CMD_data_write_i32(uint16_t u16_data_id, int32_t i32_data_load);
static uint8_t su8_api_CMD_CRC8(uint8_t u8_start, uint8_t u8_length);



#endif