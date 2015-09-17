#ifndef API_UBLOX_MSG_H
#define API_UBLOX_MSG_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_dma.h"
#include "board_r_buff.h"
#include "board_uart.h"
#include "api_gps_nav.h"

#define MAX_UBL_MESSAGE_LENGTH 128U
#define ACK_TIME_OUT           2000U
#define GPS_FIX_TIMEOUT        600000U  /*  wait for GPS fix satelites for 60 seconds */
#define GPS_UART_SPEED_TIMEOUT 300U     /* 300 mS */

#define UBL_NAV_VELNED_ID      0x0112U


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

typedef enum 
{
    UART_9600,
    UART_19200,
    UART_38400,
    UART_57600,
    UART_115200
} UART_BAUD_RATE;


typedef struct
{
    uint8_t     u8_class;
    uint8_t     u8_id;
} UBL_ID;

typedef struct
{
    UBL_ID     ub_id;
    uint8_t    u8_ack_state;
} UBL_ACK_STATE;


/*
typedef struct
{
    int32_t     i32_longitude;
    int32_t     i32_latitude;
    int32_t     i32_height;
    uint32_t    u32_speed;
    int32_t     i32_heading;
} GPS_NAVIGATION_DATA;

typedef struct
{
    uint8_t     u8_gpsFix;
    uint8_t     u8_flags;
    uint8_t     u8_fixStat;
    uint8_t     u8_flags2;
    uint32_t    u32_ttff;
    uint32_t    u32_msss;
} GPS_RECEIVER_STATE;
*/



       BOARD_ERROR api_ublox_msg_parcer_init(void); 
       BOARD_ERROR api_ublox_msg_send(uint8_t u8_message[], uint16_t u16_size);
       BOARD_ERROR api_ublox_msg_read_status(void);
       BOARD_ERROR api_ublox_msg_input_decode(USART_TypeDef*  USARTx);
       
       BOARD_ERROR api_ublox_msg_set_navigation_data(GPS_NAVIGATION_DATA nav_data); 
static BOARD_ERROR api_ublox_msg_get_navigation_data(GPS_NAVIGATION_DATA *nav_data);
       BOARD_ERROR api_ublox_msg_get_nav_status(GPS_RECEIVER_STATE *nav_state); 
       BOARD_ERROR api_ublox_msg_reset_nav_status(void); 
       
/* static BOARD_ERROR api_ublox_msg_decode(char c, uint8_t u8_message[]); */
       
static BOARD_ERROR api_ublox_msg_byte_decode(uint8_t u8_c, uint8_t u8_message[]); 
static BOARD_ERROR api_ublox_msg_message_decode(uint8_t u8_buff[], uint32_t u32_length, uint32_t *u32_packet_id);
static BOARD_ERROR api_ublox_msg_get_message_state(UBL_STATE  *us_state);
static BOARD_ERROR api_ublox_msg_set_message_state(UBL_STATE   us_state);
static BOARD_ERROR api_ublox_msg_ck_a_b(uint8_t buff[], uint32_t u32_length, uint8_t *CK_A, uint8_t *CK_B);


BOARD_ERROR api_ublox_msg_send_speed(UART_BAUD_RATE ub_baud_rate);

static BOARD_ERROR api_ublox_msg_UART3_TX_copy(uint8_t u8_out[], uint16_t u16_size);

static BOARD_ERROR api_ublox_msg_set_home_wp(GPS_POSITION_DATA gpd_gps_data); 






#endif