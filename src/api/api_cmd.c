#include "api_cmd.h"

/* Linear buffer. */
static  uint8_t  u8_value_buffer[API_CMD_MAX_SIZE_OF_LINE_BUFFER];

void api_cmd_reading_packet(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint16_t u16_save_tail_index;
    uint16_t u16_save_size;
    uint8_t  u8_read_byte;
    uint8_t  u8_CRC = 0U;
    uint8_t  u8_flag = 0U;
    uint8_t  u8_i    = 0U;
    uint8_t  u8_packet_size = 0U;


    /* Try to found packet header. */
    while(be_result == BOARD_ERR_OK )
    {
        /* Save current buffer tail index pointed to naxt byte after header of packet. */
        u16_save_tail_index = u16_board_r_buff_USART1_RX_tail_get_buffer();

        /* Save current buffer size. */
        u16_save_size = u16_board_r_buff_USART1_RX_size_buffer_get();

        /* Read byte from UART1 Rx buffer. Remember, after reading size--, tail++. */
        be_result = be_board_r_buff_USART1_RX_Get_byte(&u8_read_byte);

        /* Check for packet header. */
        if(u8_read_byte == HEAD_OF_PACKET)
        {
            /* Flag header found. */
            u8_flag = 1U;
            break;
        }
    }

   	/* Start read packet data if buffer is not empty and pointed to head of packet. */
	while(  ( (u16_board_r_buff_USART1_RX_size_buffer_get() > 0U) || (u8_flag == 4U) )  && (u8_flag != 0U))
	{
		switch (u8_flag)
		{
            case 1: /* Read and check size of data in packet. */

                /* Read size of data in packet. */
                be_result = be_board_r_buff_USART1_RX_Get_byte(&u8_read_byte);
                /* First byte placed after header is size of packet. */
                u8_packet_size  = u8_read_byte;

                /* Check size value. */
                if(u8_packet_size >= API_CMD_MAX_SIZE_OF_LINE_BUFFER)
                {
                    /* It is wrong size. Exit from while. */
                    /* Buffer still pointed to SIZE byte. It can be head of  next packet. */

                    /* Disable state "header found". */
                    u8_flag = 0U;

                    /* Restore tail index in RX UART1 buffer. */
                    v_board_r_buff_USART1_RX_tail_buffer_set(u16_save_tail_index);

                    /* Restore size of data in RX UART1 buffer. */
                    v_board_r_buff_USART1_RX_size_buffer_set(u16_save_size);

                    /* Remove header from UART1 buffer. */
                    /* Read byte from UART1 RX buffer. Remember, after reading size--, tail++. */
                    be_result = be_board_r_buff_USART1_RX_Get_byte(&u8_read_byte);

                    be_result = BOARD_ERR_RANGE;
                }
                else
                {
                    /*Go to read data from packet. */
                    u8_flag = 2U;
                    /*Set line buffer index to start. */
                    u8_i = 0U;

                    if(u16_board_r_buff_USART1_RX_size_buffer_get() == 0U)
                    {
                        be_result = BOARD_ERR_EMPTY;
                        /* TODO: check if empty, what we should do??? */
                    }
			}
			break;

		case 2: /* Read data to line buffer.*/
			be_result = be_board_r_buff_USART1_RX_Get_byte(&u8_read_byte);

			u8_value_buffer[u8_i] = u8_read_byte;
			u8_i++;
            if(u8_i >= u8_packet_size)
            {
            	u8_flag = 3U;
            }

			if(u16_board_r_buff_USART1_RX_size_buffer_get() == 0U)
			{
				be_result = BOARD_ERR_EMPTY; /* lets wait for next parth of packet. */
			}
            break;

        case 3:
            /* Read CRC byte.*/
			be_result = be_board_r_buff_USART1_RX_Get_byte(&u8_read_byte);
			/* Here u8_i = u8_packet_size, so ,it pointed to CRC position. */
			u8_value_buffer[u8_i] = u8_read_byte; /* Save CRC */
            /* Calculating of CRC. */
            u8_CRC = su8_api_CMD_CRC8(0U, u8_packet_size);
            /* Check CRC.*/
            if(u8_value_buffer[u8_i] == u8_CRC)
            {
                be_result = BOARD_ERR_OK ;
                u8_flag = 4U;
            }
            else
            {
				/* It is wrong CRC. *//* Exit from while. *//* Buffer still pointed to SIZE byte. It can be head of  next packet. */
				u8_flag = 0U;

                /* Restore tail index in RX UART1 buffer. */
                v_board_r_buff_USART1_RX_tail_buffer_set(u16_save_tail_index);
                /* Restore size of data in RX UART1 buffer. */
                v_board_r_buff_USART1_RX_size_buffer_set(u16_save_size);
                /* Remove header from UART1 buffer. */
                /* Read byte from UART1 RX buffer. Remember, after reading size--, tail++. */
                be_result = be_board_r_buff_USART1_RX_Get_byte(&u8_read_byte);
				be_result = BOARD_ERR_RANGE;
            }
            break;
		case 4:
            /* Decoding data from packet */
            /* TODO: In line_buffer we have now packet information load. We should decode it here. */
            be_result = be_api_CMD_decoding_packet();
            if(be_result == BOARD_ERR_OK)
            {
                /* Start looking for next packet. */
                u8_flag = 0U;
            }
            else
            {
                u8_flag = 0U;
                /* Restore tail index in RX UART1 buffer. */
                v_board_r_buff_USART1_RX_tail_buffer_set(u16_save_tail_index);
                /* Restore size of data in RX UART1 buffer. */
                v_board_r_buff_USART1_RX_size_buffer_set(u16_save_size);
                /* Remove header from UART1 buffer. */
                /* Read byte from UART1 RX buffer to remove header byte. Remember, after reading size--, tail++. */
                be_board_r_buff_USART1_RX_Get_byte(&u8_read_byte);
				be_result = BOARD_ERR_RANGE;
            }
            break;

		default:
			/* Just exit with error. */
			u8_flag = 0U;
			be_result = BOARD_ERR_RANGE;
			break;
		}
	}
}

/* Function decoding packet and call suitable process function. */
static BOARD_ERROR be_api_CMD_decoding_packet(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint16_t u16_data_id  = 0U;
    uint16_t u16_a  = 0U;
    uint16_t u16_b  = 0U;
    uint8_t u8_cmd_id     = 0U;
    int32_t i32_data_load = 0;

    u8_cmd_id = u8_value_buffer[0];
    u16_a = (uint16_t)u8_value_buffer[1];
    u16_b = (uint16_t)u8_value_buffer[2];
    u16_b = u16_b << 8;
    u16_data_id = u16_a + u16_b;

    switch (u8_cmd_id)
    {
      case READ32:
        be_result = be_api_CMD_data_answer_i32(u16_data_id);
        break;
      case READ64:
        be_result = be_api_CMD_data_answer_u64(u16_data_id);
        break;
      case READQUAT:
        be_result = be_api_CMD_data_answer_quaternion(u16_data_id);
        break;
      case READVECTOR3D:
        be_result = be_api_CMD_data_answer_float_vector3d(u16_data_id);
        break;


      case WRITE32:
        i32_data_load = *(int32_t*)((void*)&u8_value_buffer[3]);

        be_result = be_api_CMD_data_write_i32(u16_data_id, i32_data_load );
        break;
      default:
        be_result = BOARD_ERR_ID;
        break;
    }
    return(be_result);
}

static BOARD_ERROR be_api_CMD_data_answer_float_vector3d(uint16_t u16_data_id)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    BOARD_FLOAT_3X_DATA fv3d_data;
    switch(u16_data_id)
    {
            /* Send body wing angles. */
        case 0x0140U:
            fv3d_data.fl_X = fl_api_body_angle_wind_angles[0];
            fv3d_data.fl_Y = fl_api_body_angle_wind_angles[1];
            fv3d_data.fl_Z = fl_api_body_angle_wind_angles[2];

            board_dma_send_answer_float_vector3d(u16_data_id, fv3d_data);
            break;
        default:
            be_result = BOARD_ERR_ID;
            break;
    }
    return(be_result);
}

static BOARD_ERROR be_api_CMD_data_answer_quaternion(uint16_t u16_data_id)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    BOARD_QUAT  bq_data;
    switch(u16_data_id)
    {
            /* Send quaternion. */
        case 0x0120U:
            bq_data.fl_q0 = fl_api_body_angle_quaternion[0];
            bq_data.fl_q1 = fl_api_body_angle_quaternion[1];
            bq_data.fl_q2 = fl_api_body_angle_quaternion[2];
            bq_data.fl_q3 = fl_api_body_angle_quaternion[3];

            board_dma_send_answer_quaternion(u16_data_id, bq_data);
            break;
        default:
            be_result = BOARD_ERR_ID;
            break;
    }
    return(be_result);
}



static BOARD_ERROR be_api_CMD_data_answer_u64(uint16_t u16_data_id)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    switch(u16_data_id)
    {
            /* Get system time. */
        case 0x00FAU:
            board_dma_send_answer_uint64(u16_data_id, gu64_read_system_time());
            break;
        default:
            be_result = BOARD_ERR_ID;
            break;
    }
    return(be_result);
}

/* Function decode right data ID and write receivet value to right place. */
static BOARD_ERROR be_api_CMD_data_write_i32(uint16_t u16_data_id, int32_t i32_data_load )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

     switch(u16_data_id)
    {
        /* Pitch. */
        case 0x0010U:
            pid_api_pid_data[Pitch].i32_p_gain     = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0011U:
            pid_api_pid_data[Pitch].i32_p_dyn_gain = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0012U:
            pid_api_pid_data[Pitch].i32_i_gain     = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0013U:
            pid_api_pid_data[Pitch].i32_d_gain     = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0014U:
            pid_api_pid_data[Pitch].i32_i_min      = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0015U:
            pid_api_pid_data[Pitch].i32_i_max      = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        /* Roll. */
        case 0x0020U:
            pid_api_pid_data[Roll].i32_p_gain     = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0021U:
            pid_api_pid_data[Roll].i32_p_dyn_gain = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0022U:
            pid_api_pid_data[Roll].i32_i_gain     = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0023U:
            pid_api_pid_data[Roll].i32_d_gain     = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0024U:
            pid_api_pid_data[Roll].i32_i_min      = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0025U:
            pid_api_pid_data[Roll].i32_i_max      = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        /* Yaw. */
        case 0x0030U:
            pid_api_pid_data[Yaw].i32_p_gain     = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0031U:
            pid_api_pid_data[Yaw].i32_p_dyn_gain = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0032U:
            pid_api_pid_data[Yaw].i32_i_gain     = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0033U:
            pid_api_pid_data[Yaw].i32_d_gain     = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0034U:
            pid_api_pid_data[Yaw].i32_i_min      = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        case 0x0035U:
            pid_api_pid_data[Yaw].i32_i_max      = i32_data_load;
            board_dma_send_WRITE_OK();
            break;
        /* BARO */           
        case 0x0065U:
            bp_baro_pid.i32_p_gain              = i32_data_load;
            board_dma_send_WRITE_OK();
            break;  
        case 0x0066U:
            bp_baro_pid.i32_i_gain              = i32_data_load;
            board_dma_send_WRITE_OK();
            break;  
        case 0x0067U:
            bp_baro_pid.i32_i_max               = i32_data_load;
            board_dma_send_WRITE_OK();
            break;  
        case 0x0068U:
            bp_baro_pid.i32_i_min               = i32_data_load;
            board_dma_send_WRITE_OK();
            break;  
        case 0x0069U:
            bp_baro_pid.i32_d_gain              = i32_data_load;
            board_dma_send_WRITE_OK();
            break;  
        default:
            be_result = BOARD_ERR_ID;
            break;
    }
    return(be_result);
}

/* Function decode right data ID and send right value to PC. */
static BOARD_ERROR be_api_CMD_data_answer_i32(uint16_t u16_data_id)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    switch(u16_data_id)
    {
        /* pid_api_pid_data[Pitch] */
        case 0x0010U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Pitch].i32_p_gain);
            break;
        case 0x0011U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Pitch].i32_p_dyn_gain);
            break;
        case 0x0012U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Pitch].i32_i_gain);
            break;
        case 0x0013U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Pitch].i32_d_gain);
            break;
        case 0x0014U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Pitch].i32_i_min);
            break;
        case 0x0015U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Pitch].i32_i_max);
            break;

        /* pid_api_pid_data[Roll] */
        case 0x0020U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Roll].i32_p_gain);
            break;
        case 0x0021U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Roll].i32_p_dyn_gain);
            break;
        case 0x0022U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Roll].i32_i_gain);
            break;
        case 0x0023U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Roll].i32_d_gain);
            break;
        case 0x0024U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Roll].i32_i_min);
            break;
        case 0x0025U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Roll].i32_i_max);
            break;

        /* pid_api_pid_data[Yaw] */
        case 0x0030U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Yaw].i32_p_gain);
            break;
        case 0x0031U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Yaw].i32_p_dyn_gain);
            break;
        case 0x0032U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Yaw].i32_i_gain);
            break;
        case 0x0033U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Yaw].i32_d_gain);
            break;
        case 0x0034U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Yaw].i32_i_min);
            break;
        case 0x0035U:
            board_dma_send_answer_int32(u16_data_id, pid_api_pid_data[Yaw].i32_i_max);
            break;

        /* Gyro sensor. */
        case 0x0043U:
            board_dma_send_answer_int32(u16_data_id, (int32_t)bi163d_api_data_prepr_out_gyro_raw_data.i16_X);
            break;
        case 0x0044U:
            board_dma_send_answer_int32(u16_data_id, (int32_t)bi163d_api_data_prepr_out_gyro_raw_data.i16_Y);
            break;
        case 0x0045U:
            board_dma_send_answer_int32(u16_data_id, (int32_t)bi163d_api_data_prepr_out_gyro_raw_data.i16_Z);
            break;
        case 0x0046U:
            board_dma_send_answer_float(u16_data_id, b_float3d_api_data_norm_gyro_data.fl_X);
            break;
        case 0x0047U:
            board_dma_send_answer_float(u16_data_id, b_float3d_api_data_norm_gyro_data.fl_Y);
            break;
        case 0x0048U:
            board_dma_send_answer_float(u16_data_id, b_float3d_api_data_norm_gyro_data.fl_Z);
            break;

        /* Acceleration sensor. */
        case 0x0053U:
            board_dma_send_answer_int32(u16_data_id, (int32_t)bi163d_api_data_prepr_acce_raw_data.i16_X);
            break;
        case 0x0054U:
            board_dma_send_answer_int32(u16_data_id, (int32_t)bi163d_api_data_prepr_acce_raw_data.i16_Y);
            break;
        case 0x0055U:
            board_dma_send_answer_int32(u16_data_id, (int32_t)bi163d_api_data_prepr_acce_raw_data.i16_Z);
            break;
        case 0x0056U:
            /* board_dma_send_answer_float(u16_data_id, b_float3d_api_data_norm_acce_data.fl_X);*/
            board_dma_send_answer_float(u16_data_id, b_float3d_api_common_out_acce_data.fl_X);
            break;
        case 0x0057U:
            board_dma_send_answer_float(u16_data_id, b_float3d_api_common_out_acce_data.fl_Y);
            break;
        case 0x0058U:
            board_dma_send_answer_float(u16_data_id, b_float3d_api_common_out_acce_data.fl_Z);
            break;

        /* Baro sensor. */
        case 0x0060U:
            board_dma_send_answer_int32(u16_data_id, (int32_t)i16_board_baro_get_temperature());
            break;
        case 0x0061U:
            board_dma_send_answer_int32(u16_data_id, (int32_t)u32_board_baro_get_pressure());
            break;
        case 0x0062U:
            board_dma_send_answer_int32(u16_data_id, i32_board_baro_get_altitude());
            break;
        case 0x0063U:
            board_dma_send_answer_int32(u16_data_id, (int32_t)u32_board_baro_get_filtered_pressure());
            break;
        case 0x0064U:
            board_dma_send_answer_int32(u16_data_id, bp_baro_pid.i32_EstAlt);
            break;            
        case 0x0065U:
            board_dma_send_answer_int32(u16_data_id, bp_baro_pid.i32_p_gain);
            break;  
        case 0x0066U:
            board_dma_send_answer_int32(u16_data_id, bp_baro_pid.i32_i_gain);
            break;  
        case 0x0067U:
            board_dma_send_answer_int32(u16_data_id, bp_baro_pid.i32_i_max);
            break;  
        case 0x0068U:
            board_dma_send_answer_int32(u16_data_id, bp_baro_pid.i32_i_min);
            break;  
        case 0x0069U:
            board_dma_send_answer_int32(u16_data_id, bp_baro_pid.i32_d_gain);
            break;  

            
            /* Motors output. */
        case 0x0161U:
            board_dma_send_answer_float(u16_data_id, float_api_common_motors[0]);
            break;
        case 0x0162U:
            board_dma_send_answer_float(u16_data_id, float_api_common_motors[1]);
            break;
        case 0x0163U:
            board_dma_send_answer_float(u16_data_id, float_api_common_motors[2]);
            break;
        /* Test output. */
        case 0x0171U:
            board_dma_send_answer_float(u16_data_id, float_api_common_variables[0]);
            break;
        case 0x0172U:
            board_dma_send_answer_float(u16_data_id, float_api_common_variables[1]);
            break;
        case 0x0173U:
            board_dma_send_answer_float(u16_data_id, float_api_common_variables[2]);
            break;
        default:
            be_result = BOARD_ERR_ID;
            break;
    }
    return(be_result);
}

/* This function calc CRC summ for linear buffer from board_dma.h */
static uint8_t su8_api_CMD_CRC8(uint8_t u8_start, uint8_t u8_length)
{
    uint8_t u8_crc = 0U;
    uint8_t u8_i = 0U;

    for (u8_i = u8_start; u8_i < u8_length; u8_i++)
    {
        u8_crc = u8_crc + u8_value_buffer[u8_i];
    }
    u8_crc = 0xFFU - u8_crc;
    return(u8_crc);
}









