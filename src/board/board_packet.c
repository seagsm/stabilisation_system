/*
GPS data
 [0x0080]  GPS current Latitude. (read only)
 [0x0081]  GPS current Longitude. (read only)
 [0x0082]  GPS current Heading. (read only)
 [0x0083]  GPS current Speed. (read only)
 [0x0084]  GPS current Altitude (read only)
*/

#include "board_packet.h"



/* This function add HEAD to u8_tx_data_packet array and increment index. */
static void board_packet_add_head_of_tx_packet(uint16_t *pu16_i)
{
    uint16_t u16_index;

    /* First index of packet always is zero. */
    *pu16_i = 0U;
    u16_index = *pu16_i;
    /* Create head structure. 0x73 is header. */
    /* u8_tx_UART1_data_packet[u16_index] = 0x73U; */
    be_board_dma_set_UARTx_packet(BOARD_UART1, 0x73U, u16_index);

    u16_index++;
    *pu16_i = u16_index;
}

/* Function send WRITE_OK.*/
void board_packet_send_WRITE_OK(void)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_packet_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    /* u8_tx_UART1_data_packet[u16_i] = WRITE_OK;*/ /* write_ok */
    be_board_dma_set_UARTx_packet(BOARD_UART1, WRITE_OK, u16_i);
    u16_i++;/* index of next element */

/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    /* u8_tx_UART1_data_packet[u16_i] = u8_CRC;*/ /* 1 bytes. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_CRC, u16_i);
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
    /* u8_tx_UART1_data_packet[0x01U] = u8_size; */ /* 1 bytes. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_size, 0x01U);

    /* Send packet. */
    v_board_dma_send_packet(u16_i);
}

/* Function send ERROR.*/
void board_packet_send_ERROR(void)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_packet_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    /* u8_tx_UART1_data_packet[u16_i] = ERROR;*/ /* write_ok */
    be_board_dma_set_UARTx_packet(BOARD_UART1, ERROR, u16_i);
    u16_i++;/* index of next element */

/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    /* u8_tx_UART1_data_packet[u16_i] = u8_CRC; */ /* 1 bytes. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_CRC, u16_i);
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
    /* u8_tx_UART1_data_packet[0x01U] = u8_size; */ /* 1 bytes. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_size, 0x01U);

    /* Send packet. */
    v_board_dma_send_packet(u16_i);
}

/* Function send answer float*/
void board_packet_send_answ_float(uint16_t u16_data_id, float float_data)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_packet_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    /* u8_tx_UART1_data_packet[u16_i] = ANSWER; */ /* answer */
    be_board_dma_set_UARTx_packet(BOARD_UART1, ANSWER, u16_i);
    u16_i++;/* index of next element */

/* Add of ID of parameters. */
    board_packet_add_u16_to_packet(&u16_i, u16_data_id);

/* Add parameters value. */
    board_packet_add_float_to_packet(&u16_i, float_data);

/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    /* u8_tx_UART1_data_packet[u16_i] = u8_CRC; */ /* 1 bytes. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_CRC, u16_i);
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
    /* u8_tx_UART1_data_packet[0x01U] = u8_size; */ /* 1 bytes. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_size, 0x01U);

    /* Send packet. */
    v_board_dma_send_packet(u16_i);
}

void board_packet_send_answ_int32(uint16_t u16_data_id, int32_t i32_data)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_packet_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    /* u8_tx_UART1_data_packet[u16_i] = 0x02U;*/ /* answer */
    be_board_dma_set_UARTx_packet(BOARD_UART1, 0x02U, u16_i);
    u16_i++;/* index of next element */
/* Add of ID of parameters. */
    board_packet_add_u16_to_packet(&u16_i, u16_data_id);

/* Add parameters value. */
    board_packet_add_i32_to_packet(&u16_i, i32_data);
/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_CRC, u16_i); /* 1 bytes. */
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_size, 0x01U); /* 1 bytes */

    /* Send packet. */
    v_board_dma_send_packet(u16_i);
}

/* Function send u64 answer. */
void board_packet_send_answ_uint64(uint16_t u16_data_id, uint64_t u64_data)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_packet_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    /* u8_tx_UART1_data_packet[u16_i] = 0x02U; */ /* answer */
    be_board_dma_set_UARTx_packet(BOARD_UART1, 0x02U, u16_i); /* answer */
    u16_i++;/* index of next element */

/* Add of ID of parameters. */
    board_packet_add_u16_to_packet(&u16_i, u16_data_id);

/* Add parameters value. */
    board_packet_add_u64_to_packet(&u16_i, u64_data);
/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_CRC, u16_i); /* 1 bytes. */
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
        be_board_dma_set_UARTx_packet(BOARD_UART1, u8_size, 0x01U); /* 1 bytes */

    /* Send packet. */
    v_board_dma_send_packet(u16_i);
}

void board_packet_send_answ_quaternion(uint16_t u16_data_id, BOARD_QUAT  bq_data)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_packet_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, 0x02U, u16_i); /* answer */
    u16_i++;/* index of next element */

/* Add of ID of parameters. */
    board_packet_add_u16_to_packet(&u16_i, u16_data_id);

/* Add parameters value. */
    board_packet_add_float_to_packet(&u16_i, bq_data.fl_q0);
    board_packet_add_float_to_packet(&u16_i, bq_data.fl_q1);
    board_packet_add_float_to_packet(&u16_i, bq_data.fl_q2);
    board_packet_add_float_to_packet(&u16_i, bq_data.fl_q3);

/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_CRC, u16_i); /* 1 bytes. */
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
        be_board_dma_set_UARTx_packet(BOARD_UART1, u8_size, 0x01U); /* 1 bytes */

    /* Send packet. */
    v_board_dma_send_packet(u16_i);
}

void board_packet_send_answ_float_vector3d(uint16_t u16_data_id, BOARD_FLOAT_3X_DATA fv3d_data)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_packet_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, 0x02U, u16_i); /* answer */
    u16_i++;/* index of next element */
/* Add of ID of parameters. */
    board_packet_add_u16_to_packet(&u16_i, u16_data_id);

/* Add parameters value. */
    board_packet_add_float_to_packet(&u16_i, fv3d_data.fl_X);
    board_packet_add_float_to_packet(&u16_i, fv3d_data.fl_Y);
    board_packet_add_float_to_packet(&u16_i, fv3d_data.fl_Z);

/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_CRC, u16_i); /* 1 bytes. */
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_size, 0x01U); /* 1 bytes */

    /* Send packet. */
    v_board_dma_send_packet(u16_i);
}

/* Function send float navigation data. */
void board_packet_send_answ_float_nav_data(uint16_t u16_data_id, GPS_POSITION_DATA gpd_gps_data)
{
    uint8_t u8_CRC = 0U;
    uint8_t u8_size = 0U;
    uint16_t u16_i;

/* HEAD of TX packet. */
    board_packet_add_head_of_tx_packet(&u16_i);/* 1 bytes. */
/* Add index of SIZE position in of TX packet. */
    u16_i++;/* index of size */

/* Add command ID. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, 0x02U, u16_i); /* answer */
    u16_i++;/* index of next element */
/* Add of ID of parameters. */
    board_packet_add_u16_to_packet(&u16_i, u16_data_id);

/* Add parameters value. */
    board_packet_add_float_to_packet(&u16_i, gpd_gps_data.fl_latitude);
    board_packet_add_float_to_packet(&u16_i, gpd_gps_data.fl_longitude);
    board_packet_add_float_to_packet(&u16_i, gpd_gps_data.fl_heading);
    board_packet_add_float_to_packet(&u16_i, gpd_gps_data.fl_altitude);
    board_packet_add_float_to_packet(&u16_i, gpd_gps_data.fl_speed);
/* CRC calculation of all array from 0+1 (size of head) to current u16_i.*/
    u8_CRC = gu8_api_CRC8(2U, u16_i);

/* CRC. */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_CRC, u16_i); /* 1 bytes. */
    u16_i++;

/* Add SIZE. */
    u8_size = (uint8_t)u16_i;
    u8_size = u8_size - 3U;/* header, size, crc  */
    be_board_dma_set_UARTx_packet(BOARD_UART1, u8_size, 0x01U); /* 1 bytes */

    /* Send packet. */
    v_board_dma_send_packet(u16_i);
}




/* This function add to u8_tx_data_packet array u16 value and increment index. */
static void board_packet_add_u16_to_packet(uint16_t *pu16_i, uint16_t u16_value)
{
    uint16_t u16_index;

    u16_index = *pu16_i;
    u8_tx_UART1_data_packet[u16_index] = (uint8_t)(  u16_value & 0xFFU);
    u16_index++;
    u8_tx_UART1_data_packet[u16_index] = (uint8_t)( (u16_value >> 8) & 0xFFU);
    u16_index++;

    *pu16_i = u16_index;
}


/* This function add to u8_tx_data_packet array a float value and increment index. */
static void board_packet_add_float_to_packet(uint16_t *pu16_i, float f_value)
{
    uint32_t u32_i;
    u32_i = *(uint32_t*)((void*)&f_value);
    board_packet_add_u16_to_packet(pu16_i, (uint16_t)u32_i);
    board_packet_add_u16_to_packet(pu16_i, (uint16_t)(u32_i >> 16));
}

/* This function add to u8_tx_data_packet array a float value and increment index. */
static void board_packet_add_i32_to_packet(uint16_t *pu16_i, int32_t i32_value)
{
    uint32_t u32_i;
    u32_i = *(uint32_t*)((void*)&i32_value);
    board_packet_add_u16_to_packet(pu16_i, (uint16_t)u32_i);
    board_packet_add_u16_to_packet(pu16_i, (uint16_t)(u32_i >> 16));
}


/* This function add u64 system time to tx packet. */
static void board_packet_add_u64_to_packet(uint16_t *pu16_i, uint64_t u64_data)
{
    uint16_t u16_index;
    u16_index = *pu16_i;

    /* Add system time to tx packet. */
       /* u8_tx_UART1_data_packet[u16_index] = (uint8_t)( u64_data&0xFFU); */
    be_board_dma_set_UARTx_packet(BOARD_UART1, (uint8_t)( u64_data&0xFFU), u16_index);
    u16_index++;

       /* u8_tx_UART1_data_packet[u16_index] = (uint8_t)((u64_data >> 8)&0xFFU); */
    be_board_dma_set_UARTx_packet(BOARD_UART1, (uint8_t)((u64_data >> 8)&0xFFU), u16_index);
    u16_index++;

       /* u8_tx_UART1_data_packet[u16_index] = (uint8_t)((u64_data >> 16)&0xFFU); */
    be_board_dma_set_UARTx_packet(BOARD_UART1, (uint8_t)((u64_data >> 16)&0xFFU), u16_index);
    u16_index++;

       /* u8_tx_UART1_data_packet[u16_index] = (uint8_t)((u64_data >> 24)&0xFFU); */
    be_board_dma_set_UARTx_packet(BOARD_UART1, (uint8_t)((u64_data >> 24)&0xFFU), u16_index);
    u16_index++;

       /* u8_tx_UART1_data_packet[u16_index] = (uint8_t)((u64_data >> 32)&0xFFU); */
    be_board_dma_set_UARTx_packet(BOARD_UART1, (uint8_t)((u64_data >> 32)&0xFFU), u16_index);
    u16_index++;

       /* u8_tx_UART1_data_packet[u16_index] = (uint8_t)((u64_data >> 40)&0xFFU); */
    be_board_dma_set_UARTx_packet(BOARD_UART1, (uint8_t)((u64_data >> 40)&0xFFU), u16_index);
    u16_index++;

       /* u8_tx_UART1_data_packet[u16_index] = (uint8_t)((u64_data >> 48)&0xFFU); */
    be_board_dma_set_UARTx_packet(BOARD_UART1, (uint8_t)((u64_data >> 48)&0xFFU), u16_index);
    u16_index++;

       /* u8_tx_UART1_data_packet[u16_index] = (uint8_t)((u64_data >> 56)&0xFFU); */
    be_board_dma_set_UARTx_packet(BOARD_UART1, (uint8_t)((u64_data >> 56)&0xFFU), u16_index);
    u16_index++;

    *pu16_i = u16_index;
}











