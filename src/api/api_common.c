
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