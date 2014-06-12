
#include "MadgwickAHRS.h"


static float madgwick_Beta = 0.1f;

void madgwick_update_AccGyroMag(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz)
{
    float Beta = madgwick_Beta;

    /* short name local variable for readability*/
    float q1 = fl_api_body_angle_quaternion[0];
    float q2 = fl_api_body_angle_quaternion[1];
    float q3 = fl_api_body_angle_quaternion[2];
    float q4 = fl_api_body_angle_quaternion[3];
    
    float norm;
    float hx, hy, _2bx, _2bz;
    float s1, s2, s3, s4;
    float qDot1, qDot2, qDot3, qDot4;
    /* Auxiliary variables to avoid repeated arithmetic. */
    float _2q1mx;
    float _2q1my;
    float _2q1mz;
    float _2q2mx;
    float _4bx;
    float _4bz;
    float _2q1 = 2.0f * q1;
    float _2q2 = 2.0f * q2;
    float _2q3 = 2.0f * q3;
    float _2q4 = 2.0f * q4;
    float _2q1q3 = 2.0f * q1 * q3;
    float _2q3q4 = 2.0f * q3 * q4;
    float q1q1 = q1 * q1;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q1q4 = q1 * q4;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q2q4 = q2 * q4;
    float q3q3 = q3 * q3;
    float q3q4 = q3 * q4;
    float q4q4 = q4 * q4;

    double doubl_i;
    float  float_i;

    /*  Normalise accelerometer measurement. */
    float_i = ax * ax + ay * ay + az * az;
    doubl_i = (double)float_i;
    norm = (float)sqrt(doubl_i);

    if ( (int32_t)(norm) == 0)
    {
       /* return; */
    }

    norm = 1.0f / norm;        /* use reciprocal for division. */
    ax = ax * norm;
    ay = ay * norm;
    az = az * norm;

    /* Normalise magnetometer measurement. */
    float_i = mx * mx + my * my + mz * mz;
    doubl_i = (double)float_i;
    norm    = (float)sqrt(doubl_i);

    if (fabs((double)norm) < 0.0000001)
    {
        /*   be_result = BOARD_ERR_EMPTY; */
    }
    norm = 1.0f / norm;        /* use reciprocal for division. */
    mx = mx * norm;
    my = my * norm;
    mz = mz *norm;

    /* Reference direction of Earth's magnetic field. */
    _2q1mx = 2.0f * q1 * mx;
    _2q1my = 2.0f * q1 * my;
    _2q1mz = 2.0f * q1 * mz;
    _2q2mx = 2.0f * q2 * mx;
    hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
    hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;

    float_i = hx * hx + hy * hy;
    doubl_i = (double)float_i;
    _2bx = (float)sqrt(doubl_i);
    _2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    /* Gradient decent algorithm corrective step. */
    s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s2 =  _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s4 =  _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);

    float_i = s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4;
    doubl_i = (double)float_i;
    norm = 1.0f / (float)sqrt(doubl_i);   /* normalise step magnitude. */
    s1 = s1 * norm;
    s2 = s2 * norm;
    s3 = s3 * norm;
    s4 = s4 * norm;

    /* Compute rate of change of quaternion. */
    qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - Beta * s1;
    qDot2 = 0.5f * ( q1 * gx + q3 * gz - q4 * gy) - Beta * s2;
    qDot3 = 0.5f * ( q1 * gy - q2 * gz + q4 * gx) - Beta * s3;
    qDot4 = 0.5f * ( q1 * gz + q2 * gy - q3 * gx) - Beta * s4;

    /* Integrate to yield quaternion. */
    q1 = q1 + qDot1 * fl_api_body_angle_sample_period;
    q2 = q2 + qDot2 * fl_api_body_angle_sample_period;
    q3 = q3 + qDot3 * fl_api_body_angle_sample_period;
    q4 = q4 + qDot4 * fl_api_body_angle_sample_period;
    /* norm = 1.0f / (float)sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4); */   /* normalise quaternion. */
    norm = 1.0f / madgwick_4x_float_length(q1,q2,q3,q4);    /* normalise quaternion. */

    fl_api_body_angle_quaternion[0] = q1 * norm;
    fl_api_body_angle_quaternion[1] = q2 * norm;
    fl_api_body_angle_quaternion[2] = q3 * norm;
    fl_api_body_angle_quaternion[3] = q4 * norm;
}


BOARD_ERROR madgwick_AccGyro(float gx, float gy, float gz, float ax, float ay, float az)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    float Beta = madgwick_Beta;
    float q1 = fl_api_body_angle_quaternion[0];
    float q2 = fl_api_body_angle_quaternion[1];
    float q3 = fl_api_body_angle_quaternion[2];
    float q4 = fl_api_body_angle_quaternion[3];  /* short name local variable for readability. */
    float norm;
    float s1, s2, s3, s4;
    float qDot1, qDot2, qDot3, qDot4;

    /* Auxiliary variables to avoid repeated arithmetic. */
    float _2q1 = 2.0f * q1;
    float _2q2 = 2.0f * q2;
    float _2q3 = 2.0f * q3;
    float _2q4 = 2.0f * q4;
    float _4q1 = 4.0f * q1;
    float _4q2 = 4.0f * q2;
    float _4q3 = 4.0f * q3;
    float _8q2 = 8.0f * q2;
    float _8q3 = 8.0f * q3;
    float q1q1 = q1 * q1;
    float q2q2 = q2 * q2;
    float q3q3 = q3 * q3;
    float q4q4 = q4 * q4;

    /* Normalise accelerometer measurement. */
    /* norm = (float)sqrt(ax * ax + ay * ay + az * az); */

    norm = madgwick_3x_float_length(ax, ay, az);

    if ((int32_t)norm == 0)
    {
       be_result = BOARD_ERR_EMPTY;
    }
    else
    {
        norm = 1.0f / norm;        /* use reciprocal for division. */
        ax *= norm;
        ay *= norm;
        az *= norm;

        /* Gradient decent algorithm corrective step. */
        s1 = _4q1 * q3q3 + _2q3 * ax + _4q1 * q2q2 - _2q2 * ay;
        s2 = _4q2 * q4q4 - _2q4 * ax + 4.0f * q1q1 * q2 - _2q1 * ay - _4q2 + _8q2 * q2q2 + _8q2 * q3q3 + _4q2 * az;
        s3 = 4.0f * q1q1 * q3 + _2q1 * ax + _4q3 * q4q4 - _2q4 * ay - _4q3 + _8q3 * q2q2 + _8q3 * q3q3 + _4q3 * az;
        s4 = 4.0f * q2q2 * q4 - _2q2 * ax + 4.0f * q3q3 * q4 - _2q3 * ay;

        /*norm = 1.0f / (float)sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);*/    /* normalise step magnitude. */
        norm = madgwick_4x_float_length(s1,s2,s3,s4);
        if (fabs((double)norm) < 0.0000001)
        {
            be_result = BOARD_ERR_EMPTY;
        }
        else
        {
            norm = 1.0f / norm;    /* normalise step magnitude. */
            s1 *= norm;
            s2 *= norm;
            s3 *= norm;
            s4 *= norm;

            /* Compute rate of change of quaternion. */
            qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - Beta * s1;
            qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - Beta * s2;
            qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - Beta * s3;
            qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - Beta * s4;

            /* Integrate to yield quaternion. */
            q1 += qDot1 * fl_api_body_angle_sample_period;
            q2 += qDot2 * fl_api_body_angle_sample_period;
            q3 += qDot3 * fl_api_body_angle_sample_period;
            q4 += qDot4 * fl_api_body_angle_sample_period;

            /* norm = 1.0f / (float)sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4); */    /* normalise quaternion. */
            norm = madgwick_4x_float_length(q1,q2,q3,q4);
            if (fabs((double)norm) < 0.0000001)
            {
                be_result = BOARD_ERR_EMPTY;
            }
            else
            {
                norm = 1.0f / norm;    /* normalise quaternion. */
                fl_api_body_angle_quaternion[0] = q1 * norm;
                fl_api_body_angle_quaternion[1] = q2 * norm;
                fl_api_body_angle_quaternion[2] = q3 * norm;
                fl_api_body_angle_quaternion[3] = q4 * norm;
            }
        }
    }
    return(be_result);
}


static float madgwick_3x_float_length(float x, float y, float z)
{
    double doubl_i;
    float  float_i;
    float_i = x * x + y * y + z * z;
    doubl_i = (double)float_i;
    float_i = (float)sqrt(doubl_i);
    return(float_i);
}

static float madgwick_4x_float_length(float f_a, float f_b, float f_c, float f_d)
{
    double doubl_i;
    float  float_i;
    float_i = f_a * f_a + f_b * f_b + f_c * f_c + f_d * f_d;
    doubl_i = (double)float_i;
    float_i = (float)sqrt(doubl_i);
    return(float_i);
}
