#ifndef BOARD_FLASH_H
#define BOARD_FLASH_H 1

#include "stm32f10x.h"
/* #include "stm32f10x_gpio.h" */
/* #include "stm32f10x_rcc.h"  */
#include "board_system_type.h"

#define FLASH_KEY1      ((uint32_t)0x45670123)
#define FLASH_KEY2      ((uint32_t)0xCDEF89AB)
#define FLASH_PAGE      ((uint8_t)0x7F)

static uint8_t u8_board_flash_ready(void);
void board_flash_erase_page(uint32_t u32_address);
void board_flash_unlock(void);
void board_flash_lock(void); 
void board_flash_write(uint32_t u32_address,uint32_t u32_data); 
uint32_t board_flash_read(uint32_t u32_address); 
void board_flash_write_variables(uint8_t u8_var0, uint8_t u8_var1, uint8_t u8_var2, uint8_t u8_var3);



#endif