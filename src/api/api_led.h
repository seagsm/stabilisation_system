#ifndef API_LED_H
#define API_LED_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_gpio.h"


       BOARD_ERROR api_led_flash_set_fast_period(uint32_t u32_value);
       BOARD_ERROR api_led_flash_set_sys_period(uint64_t u64_value);
       BOARD_ERROR api_led_flash_fast(void);
       BOARD_ERROR api_led_flash_sys(void);
       BOARD_ERROR api_led_on(void);
       BOARD_ERROR api_led_off(void);

       BOARD_ERROR api_led_flash_sys_delay(uint64_t u64_timeout);
static BOARD_ERROR api_led_flash_fast_delay(uint32_t u32_timeout);





#endif