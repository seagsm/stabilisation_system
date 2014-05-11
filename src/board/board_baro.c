


#include "board_baro.h"

/* Function init baro modyle. */
BOARD_ERROR be_board_baro_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = be_board_drv_bmp085_init();
    return (be_result);
}

/* Function get correct pressure value from baro module. */
uint32_t u32_board_baro_get_pressure(void)
{
    /* Test only. */

        be_board_drv_bmp085_raw_temperature_start_read();
        gv_board_sys_tick_delay(5U);
        be_board_drv_bmp085_read_raw_temperature();
        be_board_drv_bmp085_raw_pressure_start_read();
        gv_board_sys_tick_delay(28U);
        be_board_drv_bmp085_read_raw_pressure();

        be_board_drv_bmp085_real_data_calculation();








     return(u32_board_drv_bmp085_get_pressure());
}

/* Function calculate altitude from pressure. */
int32_t i32_board_baro_get_altitude(void)
{
    float float_altitude;
    uint32_t u32_pressure;

    u32_pressure =  u32_board_baro_get_pressure();

    /* altitude in cantimeters*/
    float_altitude = (1.0f - (float)pow((float)u32_pressure/101325.0, 0.190295)) * 4433000.0f;
    return((int32_t)float_altitude);
}










