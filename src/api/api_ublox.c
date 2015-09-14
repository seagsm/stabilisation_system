
#include "api_ublox.h"

static uint8_t          u8_ubl_message[MAX_UBL_MESSAGE_LENGTH];
static uint8_t          u8_ubl_ack[3]; /* [0] Class, [1] ID, [2] ACK state 1 - ACK,2 - NAK, 0 - undefined */
static uint8_t          u8_ubl_nav_status[3]; /* [0] gpsFix, [1] Navigation Status Flag, [2] Fix Status Information */

static UBL_STATE        us_message_state;
static uint32_t         u32_message_position;
static uint32_t         u32_message_length;

static int32_t          i32_longitude;
static int32_t          i32_latitude;
static int32_t          i32_height;
static uint32_t         u32_speed;
static int32_t          i32_heading;


BOARD_ERROR api_ublox_gps_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;  
    
    be_result = api_ublox_parcer_init(); 
                    
    i32_longitude   = 0;
    i32_latitude    = 0;
    i32_height      = 0;
    u32_speed       = 0U;
    i32_heading     = 0; 
    


    return (be_result);
}


static BOARD_ERROR api_ublox_parcer_init(void) 
{ 
    BOARD_ERROR be_result = BOARD_ERR_OK;
    
     us_message_state           = PACKET_SYNC0; 
     u32_message_length         = 0U; 
     u32_message_position       = 0U; 
     
     return (be_result);
} 



static BOARD_ERROR api_ublox_message_decode(uint8_t u8_buff[], uint32_t u32_length) 
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    
    uint8_t u8_flag = 1U;
    uint8_t u8_ck_a = 0U;
    uint8_t u8_ck_b = 0U;
    uint32_t u32_packet_id = 0U;
    uint32_t u32_value = 0U;
    
    
    if (us_message_state == PACKET_RECEIVED)
    {
        /* Check SYNC bytes */
        if(u8_buff[0] == 0xB5U)
        {
            u8_flag = 0U;
        }
        if(u8_buff[1] == 0x62U)
        {
            u8_flag = 0U;
        }  
        /* Check correct CRC */
        be_result = api_ublox_ck_a_b(u8_buff,u32_length, &u8_ck_a, &u8_ck_b);        
        if(u8_buff[u32_length - 2U] != u8_ck_a)
        {
            u8_flag = 0U;
        }
        if(u8_buff[u32_length - 1U] != u8_ck_b)
        {
            u8_flag = 0U;
        }  
        
        if(u8_flag == 1U)
        {    
            /* Get CLASS and ID */
            u32_packet_id =  (((uint32_t)(u8_buff[3])) << 8) + (uint32_t)(u8_buff[4]);
            
            switch(u32_packet_id)
            { 
                case 0x0501:    /* ACK-ACK (0x05 0x01)*/ 
                    u8_ubl_ack[0] = u8_buff[6];
                    u8_ubl_ack[1] = u8_buff[7];
                    u8_ubl_ack[2] = 0x01U; /* Set ACK state valid. */
                    break;
                case 0x0500:    /* ACK-NAK (0x05 0x00)*/ 
                    u8_ubl_ack[0] = u8_buff[6];
                    u8_ubl_ack[1] = u8_buff[7];
                    u8_ubl_ack[2] = 0x02U; /* Set NAK state valid. */
                    break;                    
                case 0x0102:    /* NAV-POSLLH (0x01 0x02) */
                    /*
                        0 U4 - iTOW ms GPS time of week of the navigation epoch.  Unsigned Long See the description of iTOW for details.
                        4 I4 1e-7 lon deg Longitude GPS.G7-SW-12001-B Signed Long
                        8 I4 1e-7 lat deg Latitude                    Signed Long
                        12 I4 - height mm Height above ellipsoid       Signed Long
                        16 I4 - hMSL mm Height above mean sea level    Signed Long
                        20 U4 - hAcc mm Horizontal accuracy estimate   Unsigned Long
                        24 U4 - vAcc mm Vertical accuracy estimate     Unsigned Long
                    */
                    u32_value     = (((uint32_t)(u8_buff[13])) << 24) + (((uint32_t)(u8_buff[12])) << 16) + (((uint32_t)(u8_buff[11])) << 8) + (uint32_t)u8_buff[10];        
                    i32_longitude = (int32_t)u32_value;
                    u32_value     = (((uint32_t)(u8_buff[17])) << 24) + (((uint32_t)(u8_buff[16])) << 16) + (((uint32_t)(u8_buff[15])) << 8) + (uint32_t)u8_buff[14]; 
                    i32_latitude  = (int32_t)u32_value;
                    u32_value     = (((uint32_t)(u8_buff[21])) << 24) + (((uint32_t)(u8_buff[20])) << 16) + (((uint32_t)(u8_buff[19])) << 8) + (uint32_t)u8_buff[18]; 
                    i32_height    = (int32_t)u32_value;
                    break;
                case 0x0103: /* NAV-STATUS (0x01 0x03) */
                    u8_ubl_nav_status[0] = u8_buff[10];
                    u8_ubl_nav_status[1] = u8_buff[11];
                    u8_ubl_nav_status[2] = u8_buff[12];
                    break;
                case 0x0112: /* NAV-VELNED (0x01 0x12) */
                    /*
                    0006    0 U4 - iTOW ms GPS time of week of the navigation epoch.  See the description of iTOW for details.
                    0010    4 I4 - velN cm/s North velocity component
                    0014    8 I4 - velE cm/s East velocity component
                    0018    12 I4 - velD cm/s Down velocity component
                    0022    16 U4 - speed cm/s Speed (3-D)
                    0026    20 U4 - gSpeed cm/s Ground speed (2-D)
                    0030    24 I4 1e-5 heading deg Heading of motion 2-D
                    */
                    u32_speed   = (((uint32_t)(u8_buff[25])) << 24) + (((uint32_t)(u8_buff[24])) << 16) + (((uint32_t)(u8_buff[23])) << 8) + (uint32_t)u8_buff[22]; /* Speed 3D */       
                    u32_value   = (((uint32_t)(u8_buff[33])) << 24) + (((uint32_t)(u8_buff[32])) << 16) + (((uint32_t)(u8_buff[31])) << 8) + (uint32_t)u8_buff[30]; /* Heading of moution 2D */
                    i32_heading = (int32_t)u32_value; /* Heading of moution 2D */
                    break;
                default:
                    /* unrecognized packet */
                    be_result |= BOARD_ERR_ID;
                    break;
            }
        
        }
        else
        {
            be_result |= BOARD_ERR_RANGE;
        }
    }
    else
    {
        be_result |= BOARD_ERR_STATE;
    }
    
    return (be_result);
}


static BOARD_ERROR api_ublox_ck_a_b(uint8_t buff[], uint32_t u32_length, uint8_t *CK_A, uint8_t *CK_B)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;  
    
    uint32_t i = 0U;
    
    for (i = 2U; i < (u32_length-2U); i++)
    { 
        *CK_A += buff[i]; 
        *CK_B += *CK_A; 
    } 

    return (be_result);
}



static BOARD_ERROR api_ublox_decode(char c) 
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    
    static uint32_t u32_data_length = 0U;
    
    if (us_message_state != PACKET_RECEIVED) 
    {    
        u8_ubl_message[u32_message_position] = (uint8_t)c; 
        u32_message_position++;
        if (u32_message_position >= MAX_UBL_MESSAGE_LENGTH) 
        {
            be_result |= api_ublox_parcer_init();
            be_result |= BOARD_ERR_OVERLOAD;
        }
    }
 
    switch (us_message_state) 
    { 
        case PACKET_SYNC0: 
            if (c == 0xB5U) 
            {  
                us_message_state = PACKET_SYNC1; 
            }
            else
            {  
                be_result |= api_ublox_parcer_init(); 
            }
            break; 
        
        case PACKET_SYNC1: 
            if (c == 0x62U)
            {  
                us_message_state |= PACKET_CLASS; 
            }
            else 
            { 
                if (c == 0xB5U) 
                {     
                    us_message_state |= PACKET_SYNC0; 
                }
                else 
                {
                    be_result |= api_ublox_parcer_init();  
                }
            }    
            break; 
        case PACKET_CLASS: 
            us_message_state = PACKET_ID; 
        break; 
        
        case PACKET_ID: 
            us_message_state = PACKET_LENGTH0;
            u32_data_length = 0U;
        break; 
        case PACKET_LENGTH0: 
            u32_data_length = (uint32_t)c; 
            us_message_state = PACKET_LENGTH1; 
            break; 
        case PACKET_LENGTH1: 
            u32_data_length = u32_data_length + ((uint32_t)c) << 8;
            us_message_state = PACKET_LOAD; 
            break; 
        case PACKET_LOAD:
            if (u32_message_position == u32_data_length + 6U) /* 6 is :  sync - 2 bytes, Class+Id - 2 bytes, payload length - 2 bytes */
            {
                us_message_state = PACKET_CK_A; 
            }
            if (u32_message_position >= (MAX_UBL_MESSAGE_LENGTH - 1U)) 
            {
                be_result |= api_ublox_parcer_init();
            }
            break; 
        case PACKET_CK_A: 
            us_message_state = PACKET_CK_B; 
            break; 
        case PACKET_CK_B: 
            u32_message_length = 6U + u32_data_length + 2U; /* 6 is packet header size, 2 is CK_A + CK_B size */ 
            us_message_state = PACKET_RECEIVED; 
            break; 
        case PACKET_RECEIVED: 
            break;
        default: 
            break; 
    } 
    
    return (be_result);
}


