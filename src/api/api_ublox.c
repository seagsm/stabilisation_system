
#include "api_ublox.h"

/* Set the navigation mode (Airborne, 1G) */
static uint8_t Nav_param[] = {0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xDC};

/* Switch off GLL */
static uint8_t GLL_off[] = { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x2B };

/* Switch off GSA */
static uint8_t GSA_off[] = { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x32 };

/* Switch off GSV */
static uint8_t GSV_off[] = { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x39 };

/* Switch off RMC */
static uint8_t RMC_off[] = { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x40 };

static uint8_t NAV_POSLLH_on[] = {0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x02, 0x01, 0x0E, 0x47};
static uint8_t NAV_STATUS_on[] = {0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x03, 0x01, 0x0F, 0x49};
static uint8_t NAV_VELNED_on[] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x12, 0x01, 0x1E, 0x67};


BOARD_ERROR api_ublox_gps_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    GPS_NAVIGATION_DATA nav_data;

    be_result = api_ublox_msg_parcer_init();

    nav_data.i32_longitude   = 0;
    nav_data.i32_latitude    = 0;
    nav_data.i32_altitude    = 0;
    nav_data.u32_speed       = 0U;
    nav_data.i32_heading     = 0;

    be_result = api_ublox_msg_set_navigation_data(nav_data);

    be_result = api_ublox_gps_parameters_init();

    return (be_result);
}

/* Set start-up parameters of GPS. */
static BOARD_ERROR api_ublox_gps_parameters_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint16_t u16_size       = 0U;

/*******
    TODO: Rewrite using switch.
*******/
    /* set UART speed */
    /* u16_size = sizeof(UBLOX_9600_speed)/sizeof(*UBLOX_9600_speed); */
    /* be_result |= api_ublox_msg_send(UBLOX_9600_speed, u16_size); */
    be_result |= api_ublox_msg_send_speed(UART_115200);

    /* Nav_param */
    u16_size = sizeof(Nav_param)/sizeof(*Nav_param);
    be_result |= api_ublox_msg_send(Nav_param, u16_size);

    /* GLL_off */
    u16_size = sizeof(GLL_off)/sizeof(*GLL_off);
    be_result |= api_ublox_msg_send(GLL_off, u16_size);

    /* GSA_off */
    u16_size = sizeof(GSA_off)/sizeof(*GSA_off);
    be_result |= api_ublox_msg_send(GSA_off, u16_size);

    /* GSV_off */
    u16_size = sizeof(GSV_off)/sizeof(*GSV_off);
    be_result |= api_ublox_msg_send(GSV_off, u16_size);

    /* RMC_off */
    u16_size = sizeof(RMC_off)/sizeof(*RMC_off);
    be_result |= api_ublox_msg_send(RMC_off, u16_size);

    /* NAV_POSLLH_on */
    u16_size = sizeof(NAV_POSLLH_on)/sizeof(*NAV_POSLLH_on);
    be_result |= api_ublox_msg_send(NAV_POSLLH_on, u16_size);

    /* NAV_VELNED_on */
    u16_size = sizeof(NAV_VELNED_on)/sizeof(*NAV_VELNED_on);
    be_result |= api_ublox_msg_send(NAV_VELNED_on, u16_size);

    /* NAV_STATUS_on */
    u16_size = sizeof(NAV_STATUS_on)/sizeof(*NAV_STATUS_on);
    be_result |= api_ublox_msg_send(NAV_STATUS_on, u16_size);

    /* Wait for GPS_FIX_TIMEOUT for GPS satelite fix */
    api_led_on();
#if GPS_EMULATION_OFF
    be_result |= api_ublox_msg_read_status();
#else
    be_result = api_ublox_msg_read_status();
#endif
    api_led_off();

    return (be_result);
}