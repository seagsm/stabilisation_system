#ifndef API_BARO_H
#define API_BARO_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_baro.h"
#include "api_common.h"

#define BARO_TAB_SIZE   40U
#define API_BARO_MODE_ON 1501U
#define ALT_BARO_HOLD_THROTTLE_NEUTRAL_ZONE 20
typedef struct
{
    /* P */
    int32_t i32_p_gain;         /* proportional gain to keep angle level. */
    /* I */
    int32_t i32_i_gain;         /* integral gain to keep angle level. */
    int32_t i32_i_max;	        /* Maximum allowable integrator state. */
    int32_t i32_i_min;          /* Minimum allowable integrator state. */
    int32_t i32_errorAltitudeI;
    /* D */
    int32_t i32_d_gain;         /* derivative gain to keep angle level. */
    /* Results */  
    int32_t  i32_BaroPID;       /* Filter output. */
    int32_t  i32_EstAlt;        /* Estimated Altitude, output. */  
    int32_t  i32_AltHold;       /* Hold Altitude, input. */
    int32_t  i32_ThrHold;       /* Hold Throttle, input. */
} BARO_PID;

extern BARO_PID  bp_baro_pid;

void api_baro_PID_init(void); 
void api_baro_altitude_estimation(void);



#endif