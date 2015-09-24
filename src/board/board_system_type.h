#ifndef BOARD_SYSTEM_TYPE_H
#define BOARD_SYSTEM_TYPE_H 1



#include "stm32f10x.h"

#define _M_PI 3.141592653589793

typedef struct
{
    uint16_t u16_X;
    uint16_t u16_Y;
    uint16_t u16_Z;
} BOARD_U16_3X_DATA;

typedef struct
{
    int16_t i16_X;
    int16_t i16_Y;
    int16_t i16_Z;
} BOARD_I16_3X_DATA;

typedef struct
{
    int32_t i32_X;
    int32_t i32_Y;
    int32_t i32_Z;
} BOARD_I32_3X_DATA;

typedef struct
{
    float fl_X;
    float fl_Y;
    float fl_Z;
} BOARD_FLOAT_3X_DATA;

typedef struct
{
    float fl_q0;
    float fl_q1;
    float fl_q2;
    float fl_q3;
} BOARD_QUAT;


typedef struct
{
    uint8_t   u8_slave_address;
    uint16_t  u16_r_sizeof;
    uint16_t  u16_w_sizeof;
    uint8_t   u8_w_data;
} BOARD_DEVICE_PARAMETERS;


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
    BOARD_ERR_EMPTY     = 3U,
    BOARD_ERR_BUSY      = 4U,
    BOARD_ERR_OVERLOAD  = 5U,
    BOARD_ERR_RANGE     = 6U,
    BOARD_ERR_ID        = 7U,
    BOARD_ERR_STATE     = 8U,
    BOARD_ERR_PACKET_OK = 9U,
    BOARD_ERR_TIMEOUT   = 10U,
    BOARD_ERR_OFF       = 11U
}   BOARD_ERROR; /* variable tag be_xxx */

typedef enum
{
    BOARD_DEV_OFF       = 0U,
    BOARD_DEV_ON        = 1U
}   BOARD_DEV_STATE; /* device state */

typedef enum
{
    BOARD_DATA_NOT_READY    = 0U,
    BOARD_DATA_READY        = 1U
}   BOARD_DATA_STATE; /* device state */


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

#define MAX_CHANNEL_NUMBER      9U


typedef struct
{
    float   fl_latitude;
    float   fl_longitude;
    float   fl_height;
    float   fl_speed;
    float   fl_heading;

} GPS_POSITION_DATA;


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
    uint32_t    u32_ttff;  /* Time to first fix (millisecond time tag) */
    uint32_t    u32_msss;  /* Milliseconds since Startup / Reset */
    uint8_t     u8_gpsFix; /* GPSfix Type, 0x03 is "3D FIX OK" */
    uint8_t     u8_flags;  /* Navigation Status Flags. flag & 0x01 -> 1 in first bit means "GPS FIX OK" */
    uint8_t     u8_fixStat;/* Fix Status Information */
    uint8_t     u8_flags2; /* Further information about navigation output */
} GPS_RECEIVER_STATE;

typedef struct
{
    float fl_p;
    float fl_i;
    float fl_d;
    float fl_integrall;
    float fl_integrall_min;
    float fl_integrall_max;
    float fl_old_value;
    float fl_out;
    float fl_out_min;
    float fl_out_max;
} COURCE_PID;






















#endif