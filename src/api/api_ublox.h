#ifndef API_UBLOX_H
#define API_UBLOX_H 1

#include "stm32f10x.h"
#include "board_system_type.h"


#define MAX_UBL_MESSAGE_LENGTH 128U

typedef enum 
{
  PACKET_SYNC0,
  PACKET_SYNC1,
  PACKET_CLASS,
  PACKET_ID,
  PACKET_LENGTH0,
  PACKET_LENGTH1,
  PACKET_LOAD,
  PACKET_CK_A,
  PACKET_CK_B,
  PACKET_RECEIVED

} UBL_STATE;



BOARD_ERROR  api_ublox_gps_init(void);

static BOARD_ERROR api_ublox_decode(char c);
static BOARD_ERROR api_ublox_parcer_init(void); 
static BOARD_ERROR api_ublox_message_decode(uint8_t u8_buff[], uint32_t u32_length); 
static BOARD_ERROR api_ublox_ck_a_b(uint8_t buff[], uint32_t u32_length, uint8_t *CK_A, uint8_t *CK_B);




#endif