

#include "board_flash.h"




/*
uint8_t flash_ready(void) {
        return !(FLASH->SR & FLASH_SR_BSY);
}
*/
static uint8_t u8_board_flash_ready(void) 
{
    uint8_t u8_exit_code;
    /* Get flash busy bit. ( 1 is busy) */
    u8_exit_code = (uint8_t)FLASH->SR & FLASH_SR_BSY;
    /* Invert this bit. So now Busy is Zero.*/
    u8_exit_code = (~u8_exit_code)&0x01U; /* MISRA forbid c= !c; */
    return (u8_exit_code);
}

/* Erase page. */
void board_flash_erase_page(uint32_t u32_address) 
{
    FLASH->CR|= FLASH_CR_PER;
    FLASH->AR = u32_address;
    FLASH->CR|= FLASH_CR_STRT;
    while(!u8_board_flash_ready())
    {}  
    FLASH->CR&= ~(uint32_t)FLASH_CR_PER;
}

/* Unlock flash. */
void board_flash_unlock(void) 
{
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}

/* Lock Flash. */
void board_flash_lock(void) 
{
    FLASH->CR |= FLASH_CR_LOCK;
}

/* Write to Flash. */
void board_flash_write(uint32_t u32_address,uint32_t u32_data) 
{
    /* FLASH_CR_PG - flash programming chosen. */
    FLASH->CR |= FLASH_CR_PG;
    while(!u8_board_flash_ready())
    {}
    /* Write LSB 16 bits. */
    *(__IO uint16_t*)u32_address = (uint16_t)u32_data;
    
    while(!u8_board_flash_ready())
    {}
    u32_address += 2U;
    u32_data   >>= 16;
    /* Write MSB 16 bits. */
    *(__IO uint16_t*)u32_address = (uint16_t)u32_data;
    while(!u8_board_flash_ready())
    {}
    /* Flash programming unchosen. */
    FLASH->CR &= ~((uint32_t)FLASH_CR_PG);
}

/* Read from Flash */
uint32_t board_flash_read(uint32_t u32_address) 
{
    return (*(__IO uint32_t*) u32_address);
}

/* This function write four uint8 (byte) of data to flash. */
void board_flash_write_variables(uint8_t u8_var0, uint8_t u8_var1, uint8_t u8_var2, uint8_t u8_var3)
{
    uint32_t u32_wr_data = 0U; 
        
    u32_wr_data =     (uint32_t)u8_var3<<24;
    u32_wr_data = u32_wr_data + u8_var2<<16; 
    u32_wr_data = u32_wr_data + u8_var1<<8;
    u32_wr_data = u32_wr_data + u8_var0;

    /* Calculating start address of wrining to page 127, defined in H file. */
    uint32_t u32_st_address = FLASH_BASE + FLASH_PAGE * 1024U;
    uint16_t u16_tmp;

    board_flash_unlock();
    board_flash_erase_page(u32_st_address);
    board_flash_lock();
    board_flash_unlock();

    for(u16_tmp = 0U; u16_tmp < 1024U; u16_tmp += 4U)
    {
        board_flash_write(u32_st_address + u16_tmp, u32_wr_data);
    }        
    board_flash_lock();
}

/*
int main(void) {

        while (1)
        {
                uint8_t v1,v2,v3,v4;

                flash_write_variables(1,2,3,4);

                uint32_t st_address = FLASH_BASE + FLASH_PAGE * 1024;
                v1=flash_read(st_address);
                v2=flash_read(st_address)>>8;
                v3=flash_read(st_address)>>16;
                v4=flash_read(st_address)>>24;
        }
}
*/









