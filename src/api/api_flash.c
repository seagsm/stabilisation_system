
#include "api_flash.h"


/* Set point to configuration structure to flash PAGE 127. */
#pragma location = FLASH_BASE + FLASH_PAGE * 1024U 
const API_FLASH_SYS_CONFIG api_flash_configuration;


BOARD_ERROR be_api_flash_write_configuration(void)
{    
    BOARD_ERROR be_result = BOARD_ERR_OK;

    API_FLASH_SYS_CONFIG api_flash_wr_config;
    
    uint32_t u32_addr_mem = 0U;
    uint32_t u32_addr_flash = 0U;
    
    /* Get size of structure in 8bits bytes. */
    uint32_t u32_sizeof = 0U;
    
    /* Calculating start address of wrining to page 127, defined in H file. */
    uint32_t u32_st_address = FLASH_BASE + FLASH_PAGE * 1024U;
    uint16_t u16_tmp;

    board_flash_unlock();
    board_flash_erase_page(u32_st_address);
    board_flash_lock();
    board_flash_unlock();

    u32_sizeof = sizeof(api_flash_configuration);
    
    api_flash_wr_config.pid_data[0]       = pid_api_pid_data[0];
    api_flash_wr_config.pid_data[1]       = pid_api_pid_data[1];
    api_flash_wr_config.pid_data[2]       = pid_api_pid_data[2];
    api_flash_wr_config.u32_data_is_valid = DATA_IS_VALID;
    api_flash_wr_config.u32_data_CRC      = gu32_api_flash_config_CRC32((uint32_t) &api_flash_wr_config);
 
    for(u16_tmp = 0U; u16_tmp < u32_sizeof; u16_tmp += 4U)
    {
        board_flash_write(u32_st_address + u16_tmp, ( ((uint32_t) &api_flash_wr_config) + u16_tmp) );
    }        
    board_flash_lock();

    for(u16_tmp = 0U; u16_tmp < u32_sizeof; u16_tmp += 4U)
    {
        u32_addr_flash = u32_st_address + u16_tmp;
        u32_addr_mem = ((uint32_t) &api_flash_wr_config) + u16_tmp;
        if( *(uint32_t *)u32_addr_flash != *(uint32_t *)u32_addr_mem)
        {
            be_result = BOARD_ERR_ERROR;
            break;
        }  
    }        
    return(be_result);
}


/* Function read configuration setup from flash to ram. */
BOARD_ERROR be_api_flash_read_configuration(void)
{    
    BOARD_ERROR be_result = BOARD_ERR_OK;
    
    return(be_result);
}

uint32_t gu32_api_flash_config_CRC32(uint32_t u32_address)
{
    uint32_t u32_CRC = 0U;
    uint32_t u32_sizeof;
    uint32_t u32_current_address;
    uint32_t u32_end_address;
    
    /* Get size of structure in 8-bits bytes. */
    u32_sizeof = sizeof(api_flash_configuration);
    /* Get point to the end of structure. */
    u32_end_address = u32_sizeof + u32_address;
    /* Get start address.*/
    u32_current_address = u32_address; 
    /* Calc summ of all u32 elements of structure. */
    while(u32_end_address >= u32_address)
    {
        u32_CRC = u32_CRC + *(uint32_t *)u32_current_address;  
        u32_current_address = u32_current_address + 4U;
        u32_end_address = u32_end_address - 4U;  
    }  
    /* Remove u32_data_CRC element from CRC summ. */
    u32_CRC = u32_CRC -  (*(API_FLASH_SYS_CONFIG *)u32_address).u32_data_CRC;
    /* Calc CRC. */
    u32_CRC = 0xFFFFFFFFU - u32_CRC;
    return(u32_CRC);
} 




