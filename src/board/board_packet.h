#ifndef BOARD_PACK_H
#define BOARD_PACK_H 1



#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_dma.h"
#include "api_CRC.h"



void board_packet_send_WRITE_OK(void);
void board_packet_send_ERROR(void);
void board_packet_send_answ_float(uint16_t u16_data_id, float float_data);
void board_packet_send_answ_int32(uint16_t u16_data_id, int32_t i32_data);
void board_packet_send_answ_uint64(uint16_t u16_data_id, uint64_t u64_data);
void board_packet_send_answ_quaternion(uint16_t u16_data_id, BOARD_QUAT  bq_data);
void board_packet_send_answ_float_vector3d(uint16_t u16_data_id, BOARD_FLOAT_3X_DATA fv3d_data);
void board_packet_send_answ_float_nav_data(uint16_t u16_data_id, GPS_POSITION_DATA gpd_gps_data);

static void board_packet_add_u16_to_packet(uint16_t *pu16_i, uint16_t u16_value);
static void board_packet_add_bi163x_to_packet(uint16_t *pu16_i, BOARD_I16_3X_DATA bi163x_value);
static void board_packet_add_i32_to_packet(uint16_t *pu16_i, int32_t i32_value);
static void board_packet_add_u64_to_packet(uint16_t *pu16_i, uint64_t u64_data);
static void board_packet_add_b_float3d_to_packet(uint16_t *pu16_i, BOARD_FLOAT_3X_DATA b_float3d_value);
static void board_packet_add_float_to_packet(uint16_t *pu16_i, float f_value);
static void board_packet_add_head_of_tx_packet(uint16_t *pu16_i);



#endif