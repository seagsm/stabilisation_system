
#include "api_common.h"


/* Common use variables.*/
BOARD_FLOAT_3X_DATA b_float3d_api_common_out_acce_data;
float float_api_common_variables[3];
float float_api_common_motors[3];




/*
 * Function convert degree to radians.
 * It is neccessary for Update algoritm.
 */
float deg2rad(float degrees)
{
    return (float)(_M_PI / 180.0f) * degrees;
}

/* Function convert degree to radians*/
float rad2deg(float rad)
{
    return (float)(180.0f/_M_PI) * rad;
}

/* Return x between y and z.*/
int32_t constrain_i32(int32_t i32_x,int32_t i32_y,int32_t i32_z)
{
    if(i32_x < i32_y)
    {
        i32_x = i32_y;
    }
    else if(i32_x > i32_z)
    {
        i32_x = i32_z;
    }
    return(i32_x);
}

/* Function return minimum value of two int32. */
int32_t min(int32_t i32_x,int32_t i32_y)
{
    if(i32_x > i32_y)
    {
        i32_x = i32_y;
    }
    return(i32_x);
}

int32_t abs_t(int32_t i32_x)
{
    if(i32_x < 0)
    {
        i32_x = -i32_x;
    }
    return(i32_x);
}



