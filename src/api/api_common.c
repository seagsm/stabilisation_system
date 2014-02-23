
#include "api_common.h"




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