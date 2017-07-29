#include "board_gps.h"

static BOARD_DEV_STATE bds_flag_GPS_on = BOARD_DEV_OFF;

/* Set GPS device state. Use this function if you need set GPS state. */
static BOARD_ERROR be_board_gps_set_gps_dev_state(BOARD_DEV_STATE bds_value)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    bds_flag_GPS_on = bds_value;

    return (be_result);
}

/* Get GPS davice state. Use this function if you need get GPS state. */
BOARD_ERROR be_board_gps_get_gps_dev_state(BOARD_DEV_STATE *bds_value)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    *bds_value = bds_flag_GPS_on;

    return (be_result);
}

/* Init GPS module. */
BOARD_ERROR be_board_gps_init(void)
{
    BOARD_ERROR     be_result = BOARD_ERR_OK;
    BOARD_DEV_STATE bds_value = BOARD_DEV_OFF;

    /* Initialisation of navigation subsystem. */
    be_result = api_gps_nav_initialisation();

    /* Start GPS UBLOX init function */
    be_result = be_board_UBLOX_gps_init();

    /* Set GPS device state. */
    if(be_result == BOARD_ERR_OK)
    {
        bds_value = BOARD_DEV_ON;
    }
    be_board_gps_set_gps_dev_state(bds_value);

    return(be_result);
}

/* Init UBLOX GPS module. */
static BOARD_ERROR be_board_UBLOX_gps_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

#if GPS_EMULATION_OFF
    be_result |= be_board_UBLOX_gps_check();
#else

#endif

    if(be_result == BOARD_ERR_OK)
    {
        be_result |= api_ublox_gps_init();
    }
    return(be_result);
}

/* This function check if GPS UBLOX module connected to UART. */
static BOARD_ERROR be_board_UBLOX_gps_check(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    uint32_t u32_timeout        = 0U;
    char     c_value            = 0U;

    /* Clear GPRMC state flag to undefined state (0x00) */
    v_api_nmea_clear_gprmc_status();

    /* Wait timeout for GPRMC packets */
    while(u32_timeout < GPS_TIMEOUT)
    {
        /* read RX data from DMA buffer to UART buffer */
        be_board_dma_DMA1_CH3_buffer_copy_to_UART3_buffer();
        /* Decode current part of data. */
        api_nmea_decode(USART3);
        /* Delay 1 milliSec */
        gv_board_sys_tick_delay(1U);
        u32_timeout++;
        /* Read GPRMC starus flag. */
        c_value = c_api_nmea_gprmc_status();

        /* Check status field from GPS GPRMC packet. GPRMC packet should be generated by UBLOX modyle by default. */
        if( c_value != 0U)
        {
            if( (c_value == 'A') || (c_value == 'V'))
            {
                /* Set flag to 0x01 if GPS module connected. */
                /* u32_flag_GPS_on = 1U; */
                be_board_gps_set_gps_dev_state(BOARD_DEV_ON);
            }
        }
    }
    return(be_result);
}





