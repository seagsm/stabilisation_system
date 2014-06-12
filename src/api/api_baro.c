


#include "api_baro.h"

BARO_PID  bp_baro_pid;

void api_baro_PID_init(void)
{
    bp_baro_pid.i32_p_gain = 16;
    bp_baro_pid.i32_i_gain = 15;
    bp_baro_pid.i32_d_gain = 7;

    bp_baro_pid.i32_BaroPID = 0;
    bp_baro_pid.i32_errorAltitudeI = 0;
    bp_baro_pid.i32_i_max = 30000;
    bp_baro_pid.i32_i_min = -30000;

    bp_baro_pid.i32_EstAlt = 0;
    bp_baro_pid.i32_AltHold = 0;
}

void api_baro_altitude_estimation(void)
{
    static uint8_t BaroHistIdx = 0U;
    static int32_t BaroHistTab[BARO_TAB_SIZE] = {0};
    static int32_t BaroHigh = 0;
    static int32_t BaroLow  = 0;
    int32_t last;
    int32_t temp32;
    uint8_t index;
    int32_t  BaroAlt = i32_board_baro_get_altitude();

    last = BaroHistTab[BaroHistIdx];

    BaroHistTab[BaroHistIdx] = BaroAlt/10;

    BaroHigh = BaroHigh + BaroHistTab[BaroHistIdx];

    /* 20 = (0 + 40/2)% 40  */
    /* 21 = (1 + 40/2)% 40  */
    /* ...                  */
    /* 39 = (19 + 40/2)% 40 */
    /*  0 = (20 + 40/2)% 40 */
    /*  1 = (21 + 40/2)% 40 */
    /*  2 = (22 + 40/2)% 40 */

    index = (BaroHistIdx + (BARO_TAB_SIZE/2U))%BARO_TAB_SIZE;
    BaroHigh -= BaroHistTab[index];
    BaroLow  += BaroHistTab[index];
    BaroLow  -= last;

    BaroHistIdx++;
    if (BaroHistIdx == BARO_TAB_SIZE)
    {
        BaroHistIdx = 0U;
    }

    bp_baro_pid.i32_BaroPID = 0;
    /* D */
    temp32 = bp_baro_pid.i32_d_gain * (BaroHigh - BaroLow) / 40;
    bp_baro_pid.i32_BaroPID = bp_baro_pid.i32_BaroPID - temp32;

    /* Estimation Altitude. */
    bp_baro_pid.i32_EstAlt = BaroHigh * 10 / ((int32_t)BARO_TAB_SIZE / 2);
    temp32 = bp_baro_pid.i32_AltHold - bp_baro_pid.i32_EstAlt;
    if (abs_t(temp32) < 10)
    {
        /* remove small D parametr to reduce noise near zero position */
        if(abs_t(bp_baro_pid.i32_BaroPID) < 10)
        {
            bp_baro_pid.i32_BaroPID = 0;
        }
    }

    /* P */
    bp_baro_pid.i32_BaroPID = bp_baro_pid.i32_BaroPID +  bp_baro_pid.i32_p_gain * constrain_i32(temp32,(-2) * bp_baro_pid.i32_p_gain, 2 * bp_baro_pid.i32_p_gain) / 100;

    /* sum of P and D should be in range 150 */
    bp_baro_pid.i32_BaroPID = constrain_i32(bp_baro_pid.i32_BaroPID, -150, +150);

   /* I */
    bp_baro_pid.i32_errorAltitudeI = bp_baro_pid.i32_errorAltitudeI + temp32 * bp_baro_pid.i32_i_gain / 50;
    bp_baro_pid.i32_errorAltitudeI = constrain_i32(bp_baro_pid.i32_errorAltitudeI, -30000, 30000);
    /* I in range +/-60 */
    temp32 = bp_baro_pid.i32_errorAltitudeI / 500; /* I in range +/-60 */
    bp_baro_pid.i32_BaroPID = bp_baro_pid.i32_BaroPID + temp32;

}
