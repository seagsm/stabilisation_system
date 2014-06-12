
#include "api_CRC.h"


/* This function calc CRC summ for u8_tx_data_packet from board_dma.h */
uint16_t gu16_api_CRC16_alg(uint16_t u16_start, uint16_t length)
{
    const uint16_t u16_polinom = 0xa001U;
          uint16_t u16_code = 0xffffU;
          uint8_t u8_ml;
          uint16_t i;
          uint16_t j;

          for (i = u16_start; i < length; i++)
          {
              /* For each byte from array */

              /* Put LSB of 16 bit code(in the future a ÑRÑ16) to ml. */
                u8_ml = (uint8_t)(u16_code);
                /* Do m1 XOR msg[i] and put result to m1. */
                u8_ml ^= u8_tx_data_packet[i];
                /* Set LSB of code to zero. */
                u16_code &= 0xff00U;
                u16_code += u8_ml;
                for (j = 0U; j < 8U; j++)
                {
                    /* Check LSB bit of code. */
                    if ((u16_code & 0x0001U) == 1U)
                    {   /* LSB bit is 1. */
                        u16_code >>= 1U;
                        /* Do code XOR polinom and put result to code. */
                        u16_code ^= u16_polinom;
                    }
                    else
                    {   /* LSB bit is 0*/
                        u16_code >>= 1U;
                    }
                }
            }
            return u16_code;
}

/* This function calc CRC summ for u8_tx_data_packet from board_dma.h */
uint8_t gu8_api_CRC8(uint16_t u16_start, uint16_t length)
{
    uint8_t u8_crc = 0U;
    uint16_t u16_i = 0U;

    for (u16_i = u16_start; u16_i < length; u16_i++)
    {
        u8_crc = u8_crc + u8_tx_data_packet[u16_i];
    }
    u8_crc = 0xFFU - u8_crc;
    return(u8_crc);
}