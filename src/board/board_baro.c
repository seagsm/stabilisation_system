


#include "board_baro.h"

/* Function init baro modyle. */
BOARD_ERROR be_board_baro_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = board_drv_bmp085_init();

    return (be_result);
}

/* Function get correct pressure value from baro module. */
int32_t i32_board_baro_get_pressure(void)
{
    int32_t i32_pressure;

    i32_pressure = i32_board_drv_bmp085_calculate_pressure();

    return(i32_pressure);
}

/* Function calculate altitude from pressure. */
int32_t i32_board_baro_get_altitude(int32_t i32_pressure)
{
    float float_altitude;

    /* altitude in cantimeters*/
    float_altitude = (1.0f - (float)pow((float)i32_pressure/101325.0, 0.190295)) * 4433000.0f;
    return((int32_t)float_altitude);
}










