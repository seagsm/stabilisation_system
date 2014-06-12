
#include "api_rx_channels.h"

/* Function calculate +- deviation from zero value of RC channel.*/
/* value shoul be in microSecond. Some part of MultiWii code. */
int32_t api_rx_channels_approximation(int32_t i32_ch_value, int32_t i32_ch_zero)
{
    int32_t i32_delta;
    int32_t i32_exp_index;
    int32_t i32_tmp_0;
    int32_t i32_tmp_1;
    int32_t i32_result;

    /* Calculation deviation from ZERO point. */
    i32_delta = min(abs_t(i32_ch_value-i32_ch_zero),500);

    /* Deviation maximum is 500, so max value of index is 5 */
    i32_exp_index = i32_delta/100;

    /* Calc aproximation points. */
    /* let delta = 234, so exp_index=delta/100 =(int)2, so delta - exp_index*100 = 234-200 = 34. */
    i32_tmp_0 = (2500+RC_EXP_COEFF*(i32_exp_index*i32_exp_index-25))*i32_exp_index*RC_RC_RATE/1250;
    i32_exp_index = i32_exp_index+1;
    i32_tmp_1 = (2500+RC_EXP_COEFF*(i32_exp_index*i32_exp_index-25))*i32_exp_index*RC_RC_RATE/1250;

    i32_result = i32_tmp_0 + (i32_delta-(i32_exp_index-1)*100) * (i32_tmp_1-i32_tmp_0) / 100;
    if(i32_ch_value< i32_ch_zero)
    {
        i32_result = -i32_result;
    }
    return(i32_result);
}

