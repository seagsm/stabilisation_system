


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
    int32_t i32_temp32;
    uint8_t index;

#if BOARD_BARO_BMP085
    int32_t  BaroAlt = i32_board_baro_bmp085_get_altitude();
#elif BOARD_BARO_MS5611
    int32_t  BaroAlt = i32_board_baro_ms5611_get_altitude();
#endif
    /*
        Input filter.
    */
    last = BaroHistTab[BaroHistIdx];

    /* Devide by 10 to remove noise. */
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
    /* Summ of newest BARO_TAB_SIZE/2 table elements. */
    BaroHigh -= BaroHistTab[index];
    /* Summ of oldest BARO_TAB_SIZE/2 table elements. */
    BaroLow  += BaroHistTab[index];
    /* Remowe oldest table element. */
    BaroLow  -= last;

    /* Increase and round index. */
    BaroHistIdx++;
    if (BaroHistIdx == BARO_TAB_SIZE)
    {
        BaroHistIdx = 0U;
    }

    /*
        Estimation Altitude.
        It is MA of newest components of table multiplied by 10 to return to same factor.
    */
    /* bp_baro_pid.i32_EstAlt = BaroHigh * 10 / ((int32_t)BARO_TAB_SIZE / 2); */
    i32_temp32 = BaroHigh * 10 / ((int32_t)BARO_TAB_SIZE / 2);
    api_baro_set_altitude_estimation(i32_temp32);

    /* Pid controller. */
    bp_baro_pid.i32_BaroPID = 0;

    /* D */
    /* Calculation of differences between MA of newest and oldest elements of table. */
    i32_temp32 = (BaroHigh / (int)BARO_TAB_SIZE) - (BaroLow / (int)BARO_TAB_SIZE);

    /* Calculation of D components. */
    i32_temp32 = bp_baro_pid.i32_d_gain * i32_temp32;

    bp_baro_pid.i32_BaroPID = bp_baro_pid.i32_BaroPID - i32_temp32;

    /* Calculation of altitude error. */
    i32_temp32 = bp_baro_pid.i32_AltHold - bp_baro_pid.i32_EstAlt;

    /*
        Remove small D parametr to reduce noise near zero position.
        10 is magic number, depend on factors.
    */
    if (abs_t(i32_temp32) < 10)
    {

        if(abs_t(bp_baro_pid.i32_BaroPID) < 10)
        {
            bp_baro_pid.i32_BaroPID = 0;
        }
    }

    /* P */
    bp_baro_pid.i32_BaroPID = bp_baro_pid.i32_BaroPID +  bp_baro_pid.i32_p_gain * constrain_i32(i32_temp32,(-2) * bp_baro_pid.i32_p_gain, 2 * bp_baro_pid.i32_p_gain) / 100;

    /* Summa of P and D should be in range 150. */
    bp_baro_pid.i32_BaroPID = constrain_i32(bp_baro_pid.i32_BaroPID, -150, +150);

   /* I */
    bp_baro_pid.i32_errorAltitudeI = bp_baro_pid.i32_errorAltitudeI + i32_temp32 * bp_baro_pid.i32_i_gain / 50;
    bp_baro_pid.i32_errorAltitudeI = constrain_i32(bp_baro_pid.i32_errorAltitudeI, -30000, 30000);
    /* I in range +/-60 */
    i32_temp32 = bp_baro_pid.i32_errorAltitudeI / 500; /* I in range +/-60 */
    bp_baro_pid.i32_BaroPID = bp_baro_pid.i32_BaroPID + i32_temp32;
}

int32_t api_baro_get_altitude_estimation(void)
{
     return (bp_baro_pid.i32_EstAlt);
}

static void api_baro_set_altitude_estimation(int32_t i32_value)
{
    bp_baro_pid.i32_EstAlt = i32_value;
}




