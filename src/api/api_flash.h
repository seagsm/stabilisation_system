#ifndef API_FLASH_H
#define API_FLASH_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "api_pid.h"
#include "board_flash.h"

#define DATA_IS_VALID   0x12345678U


typedef struct
{
    PID_element pid_data[3];
    uint32_t u32_data_is_valid;
    uint32_t u32_data_CRC;
} API_FLASH_SYS_CONFIG;

extern const API_FLASH_SYS_CONFIG api_flash_configuration;

BOARD_ERROR be_api_flash_write_configuration(void);
BOARD_ERROR be_api_flash_read_configuration(void);
uint32_t gu32_api_flash_config_CRC32(uint32_t u32_address);





#endif



