
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


double dbl_deg2rad(double degrees)
{
    return (double)(_M_PI / 180.0) * degrees;
}

/* Function convert degree to radians*/
double dbl_rad2deg(double rad)
{
    return (double)(180.0/_M_PI) * rad;
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
    else
    {

    }
    return(i32_x);
}

/* Return float x between float y and float z.*/
float fl_constrain(float fl_x, float fl_y, float fl_z)
{
    if(fl_x < fl_y)
    {
        fl_x = fl_y;
    }
    else if(fl_x > fl_z)
    {
        fl_x = fl_z;
    }
    else
    {

    }
    return(fl_x);
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

float f_sin(float f_value)
{
  return ((float)sin((double)f_value));
}

float f_cos(float f_value)
{
  return ((float)cos((double)f_value));
}

float f_pow(float f_value, float fl_i)
{
    return((float)pow((double)f_value, (double)fl_i));
}

float f_sqrt(float fl_value)
{
    return((float)sqrt((double)fl_value));
}

