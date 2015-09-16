
#include "api_led.h"

static uint32_t u32_period;
static uint64_t u64_period;


/* Set flash period for fast delay of led flashing */
BOARD_ERROR api_led_flash_set_fast_period(uint32_t u32_value)
{
    BOARD_ERROR be_result = BOARD_ERR_OK; 

    u32_period = u32_value / 2U;
    
    return(be_result);

}

/* Set flash period for system delay of led flashing */
BOARD_ERROR api_led_flash_set_sys_period(uint64_t u64_value)
{
    BOARD_ERROR be_result = BOARD_ERR_OK; 

    u64_period = u64_value / 2U;
    
    return(be_result);

}

/* Led flashing use fast delay function */
BOARD_ERROR api_led_flash_fast(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK; 

    be_result = api_led_flash_fast_delay(u32_period);
    return (be_result);
}               

/* Led flashing use system delay function */
BOARD_ERROR api_led_flash_sys(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK; 

    be_result = api_led_flash_sys_delay(u64_period);
    return (be_result);
}     

/* Led flashing with input parameters use fast delay function */
static BOARD_ERROR api_led_flash_fast_delay(uint32_t u32_timeout)
{
    BOARD_ERROR be_result = BOARD_ERR_OK; 

    GPIO_SetBits( GPIOB, GPIO_Pin_1);
    gv_board_sys_tick_fast_delay(u32_timeout);
    
    GPIO_ResetBits( GPIOB, GPIO_Pin_1);
    gv_board_sys_tick_fast_delay(u32_timeout);

    return (be_result);
}               

/* Led flashing with input parameters use system delay function */
BOARD_ERROR api_led_flash_sys_delay(uint64_t u64_timeout)
{
    BOARD_ERROR be_result = BOARD_ERR_OK; 

    GPIO_SetBits( GPIOB, GPIO_Pin_1);
    gv_board_sys_tick_delay(u64_timeout);
    
    GPIO_ResetBits( GPIOB, GPIO_Pin_1);
    gv_board_sys_tick_delay(u64_timeout);

    return (be_result);
}               













