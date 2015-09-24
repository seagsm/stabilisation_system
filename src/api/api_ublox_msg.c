
#include "api_ublox_msg.h"

static GPS_NAVIGATION_DATA     gps_data;
static GPS_RECEIVER_STATE      gps_state;
static UBL_ACK_STATE           ack_state;


static uint8_t          u8_ubl_message[MAX_UBL_MESSAGE_LENGTH];
static UBL_STATE        us_message_state;
static uint32_t         u32_message_position;
static uint32_t         u32_message_length;

static BOARD_DATA_STATE bds_gps_fresh_data_ready = BOARD_DATA_NOT_READY;

static uint8_t UBLOX_9600_speed[]   = {0xB5,0x62,0x06,0x00,0x14,0x00,0x01,0x00,0x00,0x00,0xD0,0x08,0x00,0x00,0x80,0x25,0x00,0x00,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xA0,0xA9};
static uint8_t UBLOX_19200_speed[]  = {0xB5,0x62,0x06,0x00,0x14,0x00,0x01,0x00,0x00,0x00,0xD0,0x08,0x00,0x00,0x00,0x4B,0x00,0x00,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x46,0x4B};
static uint8_t UBLOX_38400_speed[]  = {0xB5,0x62,0x06,0x00,0x14,0x00,0x01,0x00,0x00,0x00,0xD0,0x08,0x00,0x00,0x00,0x96,0x00,0x00,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x91,0x84};
static uint8_t UBLOX_57600_speed[]  = {0xB5,0x62,0x06,0x00,0x14,0x00,0x01,0x00,0x00,0x00,0xD0,0x08,0x00,0x00,0x00,0xE1,0x00,0x00,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xDC,0xBD};
static uint8_t UBLOX_115200_speed[] = {0xB5,0x62,0x06,0x00,0x14,0x00,0x01,0x00,0x00,0x00,0xD0,0x08,0x00,0x00,0x00,0xC2,0x01,0x00,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xBE,0x72};


BOARD_ERROR api_ublox_msg_get_gps_data_status(BOARD_DATA_STATE *bds_value)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    *bds_value = bds_gps_fresh_data_ready;

    return (be_result);
}

BOARD_ERROR api_ublox_msg_set_gps_data_status(BOARD_DATA_STATE bds_value)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    bds_gps_fresh_data_ready = bds_value;

    return (be_result);
}



static BOARD_ERROR api_ublox_msg_get_nav_status(GPS_RECEIVER_STATE *nav_state)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    nav_state->u8_gpsFix  = gps_state.u8_gpsFix;
    nav_state->u32_msss   = gps_state.u32_msss;
    nav_state->u32_ttff   = gps_state.u32_ttff;
    nav_state->u8_fixStat = gps_state.u8_fixStat;
    nav_state->u8_flags   = gps_state.u8_flags;
    nav_state->u8_flags2  = gps_state.u8_flags2;

    return (be_result);
}

BOARD_ERROR api_ublox_msg_reset_nav_status(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    gps_state.u8_gpsFix  = 0U;
    gps_state.u32_msss   = 0U;
    gps_state.u32_ttff   = 0U;
    gps_state.u8_fixStat = 0U;
    gps_state.u8_flags   = 0U;
    gps_state.u8_flags2  = 0U;

    return (be_result);
}

BOARD_ERROR api_ublox_msg_set_navigation_data(GPS_NAVIGATION_DATA nav_data)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    gps_data.i32_heading   = nav_data.i32_heading;
    gps_data.i32_height    = nav_data.i32_height;
    gps_data.i32_latitude  = nav_data.i32_latitude;
    gps_data.i32_longitude = nav_data.i32_longitude;
    gps_data.u32_speed     = nav_data.u32_speed;

    return (be_result);
}

BOARD_ERROR api_ublox_msg_get_navigation_data(GPS_NAVIGATION_DATA *nav_data)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    nav_data->i32_heading   = gps_data.i32_heading;
    nav_data->i32_height    = gps_data.i32_height;
    nav_data->i32_latitude  = gps_data.i32_latitude;
    nav_data->i32_longitude = gps_data.i32_longitude;
    nav_data->u32_speed     = gps_data.u32_speed;

    return (be_result);
}




/* Function reset parcing variable. */
BOARD_ERROR api_ublox_msg_parcer_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result |= api_ublox_msg_set_message_state(PACKET_SYNC0);
    u32_message_length        = 0U;
    u32_message_position      = 0U;
    return (be_result);
}


/*
    Function decode UBLOX message.
Input value:
        uint8_t u8_buff[] is message buffer,
        uint32_t u32_length is length of message.
Output value:
        uint32_t *u32_packet_id  is decoded packet Class+ID.
*/
static BOARD_ERROR api_ublox_msg_message_decode(uint8_t u8_buff[], uint32_t u32_length, uint32_t *u32_packet_id )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    uint8_t     u8_flag         = 1U;
    uint8_t     u8_ck_a         = 0U;
    uint8_t     u8_ck_b         = 0U;
    uint32_t    u32_value       = 0U;
    UBL_STATE   us_state;

    be_result |= api_ublox_msg_get_message_state(&us_state);

    if (us_state == PACKET_RECEIVED)
    {
        /* Check SYNC bytes */
        if(u8_buff[0] != 0xB5U)
        {   /* Reset flag */
            u8_flag = 0U;
        }
        if(u8_buff[1] != 0x62U)
        {   /* Reset flag */
            u8_flag = 0U;
        }
        /* Check correct CRC */
        be_result = api_ublox_msg_ck_a_b(u8_buff,u32_length, &u8_ck_a, &u8_ck_b);
        if(u8_buff[u32_length - 2U] != u8_ck_a)
        {   /* Reset flag */
            u8_flag = 0U;
        }
        if(u8_buff[u32_length - 1U] != u8_ck_b)
        {   /* Reset flag */
            u8_flag = 0U;
        }

        if(u8_flag == 1U)
        {
            /* Get CLASS and ID */
            *u32_packet_id =  (((uint32_t)(u8_buff[2])) << 8) + (uint32_t)(u8_buff[3]);

            switch(*u32_packet_id)
            {
                case 0x0501:    /* ACK-ACK (0x05 0x01)*/
                    ack_state.ub_id.u8_class    = u8_buff[6];
                    ack_state.ub_id.u8_id       = u8_buff[7];
                    ack_state.u8_ack_state      = 0x01U; /* Set ACK state valid. */
                    break;
                case 0x0500:    /* ACK-NAK (0x05 0x00)*/
                    ack_state.ub_id.u8_class    = u8_buff[6];
                    ack_state.ub_id.u8_id       = u8_buff[7];
                    ack_state.u8_ack_state      = 0x02U; /* Set NAK state valid. */
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
                    u32_value           = (((uint32_t)(u8_buff[13])) << 24) + (((uint32_t)(u8_buff[12])) << 16) + (((uint32_t)(u8_buff[11])) << 8) + (uint32_t)u8_buff[10];
                    gps_data.i32_longitude = (int32_t)u32_value;
                    u32_value           = (((uint32_t)(u8_buff[17])) << 24) + (((uint32_t)(u8_buff[16])) << 16) + (((uint32_t)(u8_buff[15])) << 8) + (uint32_t)u8_buff[14];
                    gps_data.i32_latitude  = (int32_t)u32_value;
                    u32_value           = (((uint32_t)(u8_buff[21])) << 24) + (((uint32_t)(u8_buff[20])) << 16) + (((uint32_t)(u8_buff[19])) << 8) + (uint32_t)u8_buff[18];
                    gps_data.i32_height = (int32_t)u32_value;
                    break;
                case 0x0103: /* NAV-STATUS (0x01 0x03) */
                    gps_state.u8_gpsFix = u8_buff[10];
                    gps_state.u8_flags   = u8_buff[11];
                    gps_state.u8_fixStat = u8_buff[12];
                    gps_state.u8_flags2  = u8_buff[13];
                    gps_state.u32_ttff   = (((uint32_t)(u8_buff[17])) << 24) + (((uint32_t)(u8_buff[16])) << 16) + (((uint32_t)(u8_buff[15])) << 8) + (uint32_t)u8_buff[14];
                    gps_state.u32_msss   = (((uint32_t)(u8_buff[21])) << 24) + (((uint32_t)(u8_buff[20])) << 16) + (((uint32_t)(u8_buff[19])) << 8) + (uint32_t)u8_buff[18];
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
                    gps_data.u32_speed   = (((uint32_t)(u8_buff[25])) << 24) + (((uint32_t)(u8_buff[24])) << 16) + (((uint32_t)(u8_buff[23])) << 8) + (uint32_t)u8_buff[22]; /* Speed 3D */
                    u32_value            = (((uint32_t)(u8_buff[33])) << 24) + (((uint32_t)(u8_buff[32])) << 16) + (((uint32_t)(u8_buff[31])) << 8) + (uint32_t)u8_buff[30]; /* Heading of moution 2D */
                    gps_data.i32_heading = (int32_t)u32_value; /* Heading of moution 2D */
                    break;
                default:
                    /* unrecognized packet */
                    be_result |= BOARD_ERR_ID;
                    break;
            }
            /* Here we should reset packet decoding, because we decoded old packet and should start looking for new packet. */
            be_result |= api_ublox_msg_parcer_init();
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
    be_result |= api_ublox_msg_parcer_init();
    return (be_result);
}

/*
    Calculation of controll sull of UBL packet
    buff[]-buffer with packet, u32_length length of packet,
    *CK_A and *CK_B pointers to save CRC value
    CRC algorithm from UBLOX manual.
*/
static BOARD_ERROR api_ublox_msg_ck_a_b(uint8_t buff[], uint32_t u32_length, uint8_t *CK_A, uint8_t *CK_B)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    uint32_t i = 0U;

    for (i = 2U; i < (u32_length - 2U); i++)
    {
        *CK_A += buff[i];
        *CK_B += *CK_A;
    }
    return (be_result);
}


/* Get current message decoding state. */
static BOARD_ERROR api_ublox_msg_get_message_state(UBL_STATE  *us_state)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    *us_state = us_message_state;

    return (be_result);
}

/* Set current message decoding state. */
static BOARD_ERROR api_ublox_msg_set_message_state(UBL_STATE  us_state)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    us_message_state = us_state;

    return (be_result);
}

/*
    Function get character from input stream and try to decode UBLOX message.
    Message saved to u8_message[] array.
*/
static BOARD_ERROR api_ublox_msg_byte_decode(uint8_t u8_c, uint8_t u8_message[])
{
    BOARD_ERROR     be_result       = BOARD_ERR_OK;
    static uint32_t u32_data_length = 0U;
    UBL_STATE       us_state        = PACKET_SYNC0;

    be_result |= api_ublox_msg_get_message_state(&us_state);

    if (us_state != PACKET_RECEIVED)
    {
        u8_message[u32_message_position] = u8_c;
        u32_message_position++;
        if (u32_message_position >= MAX_UBL_MESSAGE_LENGTH)
        {
            be_result |= api_ublox_msg_parcer_init();
            be_result |= BOARD_ERR_OVERLOAD;
        }
    }

    switch (us_state)
    {
        case PACKET_SYNC0:
            if (u8_c == 0xB5U)
            {
                be_result |= api_ublox_msg_set_message_state(PACKET_SYNC1);
            }
            else
            {
                be_result |= api_ublox_msg_parcer_init();
            }
            break;

        case PACKET_SYNC1:
            if (u8_c == 0x62U)
            {
                be_result |= api_ublox_msg_set_message_state(PACKET_CLASS);
            }
            else
            {
                if (u8_c == 0xB5U)
                {
                    be_result |= api_ublox_msg_set_message_state(PACKET_SYNC0);
                }
                else
                {
                    be_result |= api_ublox_msg_parcer_init();
                }
            }
            break;

        case PACKET_CLASS:
            be_result |= api_ublox_msg_set_message_state(PACKET_ID);
            break;

        case PACKET_ID:
            u32_data_length = 0U;
            be_result |= api_ublox_msg_set_message_state(PACKET_LENGTH0);
            break;

        case PACKET_LENGTH0:
            u32_data_length = (uint32_t)u8_c;
            be_result |= api_ublox_msg_set_message_state(PACKET_LENGTH1);
            break;

        case PACKET_LENGTH1:
            u32_data_length = u32_data_length + (((uint32_t)u8_c) << 8);
            be_result |= api_ublox_msg_set_message_state(PACKET_LOAD);
            break;

        case PACKET_LOAD:
            if (u32_message_position == u32_data_length + 6U) /* 6 is :  sync - 2 bytes, Class+Id - 2 bytes, payload length - 2 bytes */
            {
                be_result |= api_ublox_msg_set_message_state(PACKET_CK_A);
            }
            if (u32_message_position >= (MAX_UBL_MESSAGE_LENGTH - 1U))
            {
                be_result |= api_ublox_msg_parcer_init();
            }
            break;

        case PACKET_CK_A:
            be_result |= api_ublox_msg_set_message_state(PACKET_CK_B);
            break;

        case PACKET_CK_B:
            u32_message_length = 6U + u32_data_length + 2U; /* 6 is packet header size, 2 is CK_A + CK_B size */
            be_result |= api_ublox_msg_set_message_state(PACKET_RECEIVED);
            break;

        case PACKET_RECEIVED:
            break;

        default:
            break;
    }
    return (be_result);
}

/* Copy UBL message to UART3 TX buffer. */
static BOARD_ERROR api_ublox_msg_UART3_TX_copy(uint8_t u8_out[], uint16_t u16_size)
{
    BOARD_ERROR be_result    = BOARD_ERR_OK;

    uint16_t   u16_i         = 0U;

    if(u16_size <= USART3_TX_DATA_PACKET_SIZE)
    {
        while(u16_i < u16_size)
        {
            u8_tx_UART3_data_packet[u16_i] = u8_out[u16_i];
            u16_i++;
        }
    }
    else
    {
        be_result = BOARD_ERR_RANGE;
    }
    return (be_result);
}

/* Function send UBLOX packet and wait TIMEOUT for ACK */
BOARD_ERROR api_ublox_msg_send_speed(UART_BAUD_RATE ub_baud_rate )
{
    BOARD_ERROR be_result    = BOARD_ERR_OK;
    uint16_t   u16_size      = 0U;
    uint32_t   u32_BaudRate  = 0U;

    switch (ub_baud_rate)
    {
        case UART_9600:
            u16_size = sizeof(UBLOX_9600_speed)/sizeof(*UBLOX_9600_speed);
            api_ublox_msg_UART3_TX_copy(UBLOX_9600_speed, u16_size);
            u32_BaudRate = 9600U;
            break;
        case UART_19200:
            u16_size = sizeof(UBLOX_19200_speed)/sizeof(*UBLOX_19200_speed);
            api_ublox_msg_UART3_TX_copy(UBLOX_19200_speed, u16_size);
            u32_BaudRate = 19200U;
            break;
        case UART_38400:
            u16_size = sizeof(UBLOX_38400_speed)/sizeof(*UBLOX_38400_speed);
            api_ublox_msg_UART3_TX_copy(UBLOX_38400_speed, u16_size);
            u32_BaudRate = 38400U;
            break;
        case UART_57600:
            u16_size = sizeof(UBLOX_57600_speed)/sizeof(*UBLOX_57600_speed);
            api_ublox_msg_UART3_TX_copy(UBLOX_57600_speed, u16_size);
            u32_BaudRate = 57600U;
            break;
        case UART_115200:
            u16_size = sizeof(UBLOX_115200_speed)/sizeof(*UBLOX_115200_speed);
            api_ublox_msg_UART3_TX_copy(UBLOX_115200_speed, u16_size);
            u32_BaudRate = 115200U;
            break;
        default:
            be_result    = BOARD_ERR_RANGE;
            break;
    }
    /* Send packet */
    sv_board_dma_ch2_send_packet(u16_size);
    /* Just wait 300 mS. It should be sent. */
    gv_board_sys_tick_delay(GPS_UART_SPEED_TIMEOUT);
    /* Reinit board UART module with new speed. */
    be_result |= be_board_uart_uart3_init(u32_BaudRate);
    /* Just wait 300 mS. It should be sent. */
    gv_board_sys_tick_delay(GPS_UART_SPEED_TIMEOUT);

    return (be_result);
}

/* Function send UBLOX packet and wait TIMEOUT for ACK */
BOARD_ERROR api_ublox_msg_send(uint8_t u8_message[], uint16_t u16_size)
{
    BOARD_ERROR be_result  = BOARD_ERR_OK;
    UBL_STATE   us_state   = PACKET_SYNC0;
    uint8_t   u8_read_byte = 0U;
    uint16_t  u16_i        = 0U;
    uint32_t  u32_ClassId  = 0U;

    while(u16_i < u16_size)
    {
        u8_tx_UART3_data_packet[u16_i] = u8_message[u16_i];
        u16_i++;
    }
    /* Send packet */
    sv_board_dma_ch2_send_packet(u16_size);

    /* We will wait for ACK packet. So, set ACK to undefined state.*/
    ack_state.ub_id.u8_class = 0U;
    ack_state.ub_id.u8_id    = 0U;
    ack_state.u8_ack_state   = 0U;

    /**** read ACK ****/
    u16_i = 0U;
    while(ack_state.u8_ack_state == 0U)
    {
        /* Read received bytes from DMA buffer to UART3 RX buffer */
        be_result = be_board_dma_DMA1_CH3_buffer_copy_to_UART3_buffer();

        while(be_result == BOARD_ERR_OK)
        {
            /* Read byte from UART3 Rx buffer. Remember, after reading size--, tail++. */
            be_result = be_board_r_buff_USARTx_RX_GET_byte(USART3, &u8_read_byte);
            if(be_result == BOARD_ERR_OK)
            {   /* Decode next RX received byte */
                be_result |= api_ublox_msg_byte_decode(u8_read_byte, u8_ubl_message);
                /* Check decoding state */
                be_result |= api_ublox_msg_get_message_state(&us_state);
                if(us_state == PACKET_RECEIVED)
                {   /* Decode UBLOX message */
                    be_result |= api_ublox_msg_message_decode(u8_ubl_message, u32_message_length, &u32_ClassId);
                    /* Check if receiver sent ACK answer */
                    if(ack_state.u8_ack_state != 0U)
                    {   /* Check if this answer was for same Class  and */
                        if(ack_state.ub_id.u8_class == u8_message[2])
                        {   /* Check if this answer was for same ID */
                            if(ack_state.ub_id.u8_id == u8_message[3])
                            {   /* stop reading input stream */
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                u16_i++ ;
            }
        }

        /* timeout counter should stop reading after ACK_TIME_OUT */
        gv_board_sys_tick_delay(1U); /* wait for 1 ms */
        u16_i++ ;
        if(u16_i >= ACK_TIME_OUT)
        {
            be_result |= BOARD_ERR_TIMEOUT;
            break;
        }
    }
    return (be_result);
}

/*
    This function using during initialisation and wait before GPS locate satelites.
    After GPS fixed position, function save current position to homw WP( WP[0])
    If  during GPS_FIX_TIMEOUT timeout GPS can not fix position, function return ERROR.
*/
BOARD_ERROR api_ublox_msg_read_status(void)
{
    BOARD_ERROR be_result       = BOARD_ERR_OK;
    BOARD_ERROR be_home_wp      = BOARD_ERR_BUSY;
    uint32_t    u32_timeout     = 0U;
    UBL_STATE   us_state        = PACKET_SYNC0;
    uint8_t     u8_read_byte    = 0U;
    uint32_t    u32_ClassId     = 0U;
    uint32_t    u32_home_wp_set_flag = 0U;
    uint32_t    u32_gps_fix_set_flag = 0U;
    GPS_RECEIVER_STATE  grs_nav_state;
    GPS_NAVIGATION_DATA gnd_nav_data;
    GPS_POSITION_DATA   gpd_gps_data;



    /* Check GPS status fix on here for GPS_FIX_TIMEOUT. */
    gps_state.u8_flags = 0U;
    u32_home_wp_set_flag = 0U;

    if(be_result == BOARD_ERR_OK)
    {
        while(  (u32_gps_fix_set_flag ==0U ) && (u32_home_wp_set_flag == 0U))
        {
            /* Read received bytes from DMA buffer to UART3 RX buffer */
            be_result = be_board_dma_DMA1_CH3_buffer_copy_to_UART3_buffer();

            /* Read byte-by-byte from UART3 RX buffer */

            while(be_result == BOARD_ERR_OK )
            {
                /* Read byte from UART3 Rx buffer. Remember, after reading size--, tail++. */
                be_result |= be_board_r_buff_USARTx_RX_GET_byte(USART3, &u8_read_byte);
                if(be_result == BOARD_ERR_OK)
                {
                    /* Decode next RX received byte */
                    be_result |= api_ublox_msg_byte_decode(u8_read_byte, u8_ubl_message);
                    be_result |= api_ublox_msg_get_message_state(&us_state);
                    if(us_state == PACKET_RECEIVED)
                    {
                        be_result |= api_ublox_msg_message_decode(u8_ubl_message, u32_message_length, &u32_ClassId);
                        be_result |= api_ublox_msg_get_nav_status(&grs_nav_state);
                        if(grs_nav_state.u8_gpsFix == 0x03U)
                        {
                            u32_gps_fix_set_flag =0U;
                            u32_timeout++ ;
                            /* Here we have GPS on and fixed. So, we can read navigation data for home way point. */

                            if(u32_ClassId == UBL_NAV_VELNED_ID)
                            {
                                /* read fresh navigation data */
                                api_ublox_msg_get_navigation_data(&gnd_nav_data);
                                /* convert UBL nav data to real word float value */
                                api_gps_nav_ubl_float_converter(&gnd_nav_data, &gpd_gps_data);
                                be_home_wp = api_ublox_msg_set_home_wp(gpd_gps_data);
                            }
                            if(be_home_wp == BOARD_ERR_OK)
                            {
                                u32_home_wp_set_flag = 1U;
                                break;
                            }
                        }
                    }
                }
            }
            /* timeout counter should stop reading after ACK_TIME_OUT */
            gv_board_sys_tick_delay(1U); /* wait for 1 ms */
            u32_timeout++ ;
            if(u32_timeout >= GPS_FIX_TIMEOUT)
            {
                be_result |= BOARD_ERR_TIMEOUT;
                break;
            }

        }
    }
    return(be_result);
}

/* Function decode messages during runtime. */
/* TODO: check it. */
BOARD_ERROR api_ublox_msg_input_decode(USART_TypeDef*  USARTx)
{
    BOARD_ERROR be_result    = BOARD_ERR_OK;
    UBL_STATE   us_state     = PACKET_SYNC0;
    uint8_t     u8_read_byte = 0U;
    uint32_t   u32_i         = 0U;
    uint32_t   u32_ClassId   = 0U;
    GPS_RECEIVER_STATE       grs_nav_state;
    uint8_t u8_flag   = 1U;

    /* Read buffer byte-by-byte. */
    while( u8_flag == 1U )
    {
        /* Read byte from UARTx Rx buffer. Remember, after reading size--, tail++. */
        be_result = be_board_r_buff_USARTx_RX_GET_byte(USARTx, &u8_read_byte);

        /* If read OK, try to decode byte from input buffer. */
        if(be_result != BOARD_ERR_OK)
        {
            u8_flag = 0U;
        }

        if(u8_flag == 1U)
        {
            /* Decode next RX received byte. */
            be_result |= api_ublox_msg_byte_decode(u8_read_byte, u8_ubl_message);

            /* Check message decoding status. */
            be_result |= api_ublox_msg_get_message_state(&us_state);

            /* If message extracting is ready, try to parsing it. */
            if(us_state == PACKET_RECEIVED)
            {


                /* Message parsing.*/
                be_result = api_ublox_msg_message_decode(u8_ubl_message, u32_message_length, &u32_ClassId);



                /* Calculation of GPS influences. */
                api_ublox_msg_get_nav_status(&grs_nav_state);

                if(grs_nav_state.u8_gpsFix == 0x03U)
                {
                    /* If decoded packet with heading, start calculation of GPS influences */
                    if(u32_ClassId == UBL_NAV_VELNED_ID)
                    {
                        /* Set GPS FRESH DATA READY flag. */
                        api_ublox_msg_set_gps_data_status(BOARD_DATA_READY);
                    }
                }

                if(be_result != BOARD_ERR_OK)
                {
                    u32_i++ ;
                }
            }
        }
    }
    return (be_result);
}

/* Write input position to home WP. */
static BOARD_ERROR api_ublox_msg_set_home_wp(GPS_POSITION_DATA gpd_gps_data)
{
      static uint32_t u32_counter = 0U;

      BOARD_ERROR be_result = BOARD_ERR_OK;

      /* Because we set home WP during initialisation, we will take correct data after HOME_WP_INITIALISATION_TIMEOUT reading after GPS FIX flag ON. */
      if(u32_counter < HOME_WP_INITIALISATION_TIMEOUT) /* 10 time */
      {
          be_result = api_gps_nav_set_wp(gpd_gps_data, 0U);
          u32_counter++;
          be_result = BOARD_ERR_BUSY;
      }
      else
      {
          be_result = BOARD_ERR_OK;
      }

      return(be_result);
}























