

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

    be_result = ms5611_reset();
 /*   be_result = be_board_drv_ms5611_init(); */
    be_result = board_drv_get_baro(&pb_baro);
    be_result |= ms5611Detect(pb_baro);

    if(be_result == BOARD_ERR_OK)
    {
        bds_value = BOARD_DEV_ON;
    }
    be_board_baro_ms5611_set_baro_dev_state(bds_value);
    return (be_result);
}

/* Function get correct temperature value from baro module. */
int16_t i16_board_baro_ms5611_get_temperature(void)
{
  /*  return(i16_board_drv_ms5611_get_temperature());*/
    return (0);
}

/* Function get correct pressure value from baro module. */
uint32_t u32_board_baro_ms5611_get_pressure(void)
{
    /* return(u32_board_drv_ms5611_get_pressure()); */
    return (0U);
}

/* Function get filtered pressure value from baro module. */
uint32_t u32_board_baro_ms5611_get_filtered_pressure(void)
{
    /* return(u32_board_drv_ms5611_get_filtered_pressure()); */
    return (0U);
}

/* Function set filtered pressure value. */
void u32_board_baro_ms5611_set_filtered_pressure(uint32_t u32_filtered_pressure)
{
    /* v_board_drv_ms5611_set_filtered_pressure(u32_filtered_pressure); */
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



BOARD_ERROR be_board_baro_ms5611_set_state(BARO_STATE_CONDITION bsc_state)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
#if 0
    switch (bsc_state)
    {
        case START_CONVERSION:
            v_board_drv_ms5611_set_state(START_TEMP_CONVERSION);
            break;
        case CONVERSION_DONE:
            v_board_drv_ms5611_set_state(CALCULATION);
            break;
        default:
            be_result = BOARD_ERR_STATE;
        break;
    }
#endif
    return (be_result);
}

BARO_STATE_CONDITION bsc_board_baro_ms5611_get_state(void)
{
    BARO_STATE_CONDITION bsc_state = UNDEFINED_STATE;
#if 0
    switch (b85sc_board_drv_ms5611_get_state())
    {
        case START_TEMP_CONVERSION:
            bsc_state = START_CONVERSION;
            break;
        case START_READING_UNCOMPENSATED_TEMP:
        case READ_UNCOMPENSATED_TEMP:
        case WAIT_FOR_TEMP_DATA_READY:
        case START_PRESS_CONVERSION:
        case START_READING_UNCOMPENSATED_PRESS:
        case READ_UNCOMPENSATED_PRESS:
        case WAIT_FOR_PRESS_DATA_READY:
            bsc_state = CONVERSION_IN_PROGRESS;
            break;
        case CALCULATION:
            bsc_state = CONVERSION_DONE;
            break;
        default:
           break;
    }
#endif

    return (bsc_state);
}

void v_board_baro_ms5611_data_compensation(void)
{
    /* be_board_drv_ms5611_real_data_calculation(); */
}





