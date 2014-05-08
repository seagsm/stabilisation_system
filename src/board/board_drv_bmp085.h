#ifndef BOARD_DRV_BMP085_H
#define BOARD_DRV_BMP085_H 1

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_i2c.h"

#define BMP085_ADDRESS 0x77U
#define BMP085_REG_START_ADDRESS 0xAAU
#define BMP085_REG_SIZEOF 0x16U   /* 11 * 2 bytes. */

typedef struct
{
  /* sensor registers from the BOSCH BMP085 datasheet */
  int16_t  i16_ac1;
  int16_t  i16_ac2;
  int16_t  i16_ac3;
  uint16_t u16_ac4;
  uint16_t u16_ac5;
  uint16_t u16_ac6;
  int16_t  i16_b1;
  int16_t  i16_b2;
  int16_t  i16_mb;
  int16_t  i16_mc;
  int16_t  i16_md;
/*
  union {uint16_t val; uint8_t raw[2]; } ut; //uncompensated T
  union {uint32_t val; uint8_t raw[4]; } up; //uncompensated P
*/
  uint8_t  u8_ut[2];
  uint8_t  u8_up[4];
  uint8_t  u8_state;
  uint32_t u32_deadline;
} BMP85_STATE;






BOARD_ERROR board_drv_bmp085_init(void);
int32_t i32_board_drv_bmp085_calculate_pressure(void);
BOARD_ERROR board_drv_bmp085_callibration_read(void);









#endif