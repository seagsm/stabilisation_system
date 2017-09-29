

#include "board_baro_ms5611.h"

static BOARD_DEV_STATE bds_baro_on = BOARD_DEV_OFF;


/* Set Baro device state. Use this function if you need set Baro state. */
static BOARD_ERROR be_board_baro_ms5611_set_baro_dev_state(BOARD_DEV_STATE bds_value)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    bds_baro_on = bds_value;

    return (be_result);
}

/* Get Baro davice state. Use this function if you need get Baro state. */
BOARD_ERROR be_board_baro_ms5611_get_baro_dev_state(BOARD_DEV_STATE *bds_value)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    *bds_value = bds_baro_on;

    return (be_result);
}

/* Function init baro modyle. */
BOARD_ERROR be_board_baro_ms5611_init(void)
{
    BOARD_ERROR     be_result = BOARD_ERR_OK;
    BOARD_DEV_STATE bds_value = BOARD_DEV_OFF;
    baro_t *pb_baro;

/* test */
    int32_t baroPressure;
    int32_t baroTemperature;
    uint32_t u32_counter = 0U;
static    float float_altitude;
/**/

    be_result = board_drv_ms5611_reset();
    be_result = board_drv_get_baro(&pb_baro);
    be_result |= board_drv_ms5611Detect(pb_baro);

    if(be_result == BOARD_ERR_OK)
    {
        bds_value = BOARD_DEV_ON;
    }
    be_board_baro_ms5611_set_baro_dev_state(bds_value);

#if 0
/* test */
    while(u32_counter < 10000U)
    {
        be_result = pb_baro->start_ut();
        gv_board_sys_tick_delay(10U);
        be_result = pb_baro->get_ut();
        gv_board_sys_tick_delay(10U);
        be_result = pb_baro->start_up();
        gv_board_sys_tick_delay(10U);
        be_result = pb_baro->get_up();
        pb_baro->calculate(&baroPressure, &baroTemperature);

        /* Pressure in pascals, 1mBar = 100 pascals, here 288m /sea-level: */
        /* altitude in centimeters*/
        float_altitude = (1.0f - (float)pow((float)baroPressure/101325.0, 0.190295)) * 4433000.0f;

        gv_board_sys_tick_delay(1000U);
        u32_counter++;
    }
/**/
#endif

    return (be_result);
}

/* Function get correct temperature value from baro module. */
int16_t i16_board_baro_ms5611_get_temperature(void)
{
     return ((int16_t)i32_board_drv_ms5611_get_temperature());
    /* return(i16_board_drv_ms5611_get_temperature()); */
}

/* Function get correct pressure value from baro module. */
uint32_t u32_board_baro_ms5611_get_pressure(void)
{
    /* return(u32_board_drv_ms5611_get_pressure()); */
    return((uint32_t) i32_board_drv_ms5611_get_pressure());
}

/* Function get filtered pressure value from baro module. */
uint32_t u32_board_baro_ms5611_get_filtered_pressure(void)
{
    return(u32_board_drv_ms5611_get_filtered_pressure());
}

/* Function set filtered pressure value. */
void v_board_baro_ms5611_set_filtered_pressure(uint32_t u32_filtered_pressure)
{
    v_board_drv_ms5611_set_filtered_pressure(u32_filtered_pressure);
}

/* Function calculate altitude from pressure. */
int32_t i32_board_baro_ms5611_get_altitude(void)
{
    float float_altitude;
    uint32_t u32_pressure;

    u32_pressure =  u32_board_baro_ms5611_get_pressure();

    /* altitude in centimeters*/
    float_altitude = (1.0f - (float)pow((float)u32_pressure/101325.0, 0.190295)) * 4433000.0f;
    return((int32_t)float_altitude);
}

BOARD_ERROR be_board_baro_ms5611_set_conversion_state(BARO_STATE_CONDITION bsc_state)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    switch (bsc_state)
    {
        case START_CONVERSION:
            be_board_drv_ms5611_set_conversion_state(MS5611_START_PRESS_CONVERSION);
            break;
        case CONVERSION_DONE:
            be_board_drv_ms5611_set_conversion_state(MS5611_CALCULATION);
            break;
        default:
            be_result = BOARD_ERR_STATE;
        break;
    }
    return (be_result);
}

BOARD_ERROR be_board_baro_ms5611_get_conversion_state(BARO_STATE_CONDITION *pbsc_state)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    MS5611_STATE_CONDITION msc_state;

    be_board_drv_ms5611_get_conversion_state(&msc_state);
    switch (msc_state)
    {
        case MS5611_START_TEMP_CONVERSION:
            *pbsc_state = START_CONVERSION;
            break;
        case MS5611_START_READ_UNCOMP_TEMP:
        case MS5611_READ_UNCOMPENSATED_TEMP:
        case MS5611_WAIT_FOR_TEMP_DATA_READY:
        case MS5611_START_PRESS_CONVERSION:
        case MS5611_START_READ_UNCOMP_PRESS:
        case MS5611_READ_UNCOMPENSATED_PRESS:
        case MS5611_WAIT_FOR_PRESS_DATA_READY:
            *pbsc_state = CONVERSION_IN_PROGRESS;
            break;
        case MS5611_CALCULATION:
            *pbsc_state = CONVERSION_DONE;
            break;
        default:
            *pbsc_state = UNDEFINED_STATE;
            break;
    }
    return (be_result);
}

