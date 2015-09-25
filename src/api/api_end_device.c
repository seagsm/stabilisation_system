
#include "api_end_device.h"

/* Value of current HEAD HOLD direction. This value is using in PDF YAW filter. */
static int32_t i32_head_hold_device = 0;

/*This function calculate value of PPM (PWM) for all end device. (Motors, servos for tricopter configuration.) */

 
void api_end_device_update(void)
{

#if API_END_DEVICE_TRICOPTER
    api_end_tricopter_update();
    i32_head_hold_device = api_end_tricopter_get_head_hold();
#endif

#if API_END_DEVICE_FLIGHT_SAURCER
    api_end_saucer_update();
    i32_head_hold_device = api_end_saucer_get_head_hold();
#endif

#if API_END_DEVICE_AERO_0
    api_end_aero_update();
    i32_head_hold_device = api_end_aero_get_head_hold();
#endif    
}

int32_t api_end_device_get_head_hold(void)
{
    return(i32_head_hold_device);
}
