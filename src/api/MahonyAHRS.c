

#include "MahonyAHRS.h"
#include "math.h"


/* Before start Update we shoud set Ki and Kp. */
/* Each update we should set SamplePeriod      */

  static float mahony_Ki = 0.0;
  /* float mahony_Kp = 0.1; */ /* it was exelent stable, but -+1 degree stabilisation during 1 60 seconds */
  static float mahony_Kp = 0.1;

  static float eInt[3];

BOARD_ERROR mahony_update_AccGyroMag(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    float Ki = mahony_Ki;
    float Kp = mahony_Kp;
    float SamplePeriod;

    /* short name local variable for readability */
    float q1 = fl_api_body_angle_quaternion[0];
    float q2 = fl_api_body_angle_quaternion[1];
    float q3 = fl_api_body_angle_quaternion[2];
    float q4 = fl_api_body_angle_quaternion[3];

    float norm;
    float hx, hy, bx, bz;
    float vx, vy, vz, wx, wy, wz;
    float ex, ey, ez;
    float pa, pb, pc;
    float  float_tmp_0;
    double double_tmp_0;

    /* Auxiliary variables to avoid repeated arithmetic. */
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

    SamplePeriod = fl_api_body_angle_sample_period;

    /* Normalise accelerometer measurement */
    /* norm = (float)sqrt(ax * ax + ay * ay + az * az); */
    float_tmp_0 = ax * ax + ay * ay + az * az;
    double_tmp_0 = (double)float_tmp_0;
    /* norm = (float)sqrt(double_tmp_0);*/
    double_tmp_0 = sqrt(double_tmp_0);
    if(fabs(double_tmp_0) > 0.0)
    {
        norm = (float)double_tmp_0;
        norm = 1.0f / norm;         /* use reciprocal for division. */
        ax *= norm;
        ay *= norm;
        az *= norm;

        /* Normalise magnetometer measurement */
        /* norm = (float)sqrt(mx * mx + my * my + mz * mz); */
        float_tmp_0 = mx * mx + my * my + mz * mz;
        double_tmp_0 = (double)float_tmp_0;
        double_tmp_0 = sqrt(double_tmp_0);
        if(fabs(double_tmp_0) > 0.0)
        {
            norm = (float)double_tmp_0;
            norm = 1.0f / norm;         /* use reciprocal for division. */
            mx *= norm;
            my *= norm;
            mz *= norm;

            /* Reference direction of Earth's magnetic field. */
            hx = 2.0f * mx * (0.5f - q3q3 - q4q4) + 2.0f * my * (q2q3 - q1q4) + 2.0f * mz * (q2q4 + q1q3);
            hy = 2.0f * mx * (q2q3 + q1q4) + 2.0f * my * (0.5f - q2q2 - q4q4) + 2.0f * mz * (q3q4 - q1q2);
            float_tmp_0 = (hx * hx) + (hy * hy);
            double_tmp_0 = (double)float_tmp_0;
            bx = (float)sqrt(double_tmp_0);
            bz = 2.0f * mx * (q2q4 - q1q3) + 2.0f * my * (q3q4 + q1q2) + 2.0f * mz * (0.5f - q2q2 - q3q3);

            /* Estimated direction of gravity and magnetic field */
            vx = 2.0f * (q2q4 - q1q3);
            vy = 2.0f * (q1q2 + q3q4);
            vz = q1q1 - q2q2 - q3q3 + q4q4;
            wx = 2.0f * bx * (0.5f - q3q3 - q4q4) + 2.0f * bz * (q2q4 - q1q3);
            wy = 2.0f * bx * (q2q3 - q1q4) + 2.0f * bz * (q1q2 + q3q4);
            wz = 2.0f * bx * (q1q3 + q2q4) + 2.0f * bz * (0.5f - q2q2 - q3q3);

            /* Error is cross product between estimated direction and measured direction of gravity */
            ex = (ay * vz - az * vy) + (my * wz - mz * wy);
            ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
            ez = (ax * vy - ay * vx) + (mx * wy - my * wx);
            if (Ki > 0.0)
            {
                eInt[0] += ex;      /* accumulate integral error */
                eInt[1] += ey;
                eInt[2] += ez;
            }
            else
            {
                eInt[0] = 0.0f;     /* prevent integral wind up */
                eInt[1] = 0.0f;
                eInt[2] = 0.0f;
            }

            /* Apply feedback terms */
            gx = gx + Kp * ex + Ki * eInt[0];
            gy = gy + Kp * ey + Ki * eInt[1];
            gz = gz + Kp * ez + Ki * eInt[2];

            /* Integrate rate of change of quaternion */
            pa = q2;
            pb = q3;
            pc = q4;
            q1 = q1 + (-q2 * gx - q3 * gy - q4 * gz) * (0.5f * SamplePeriod);
            q2 = pa + (q1 * gx + pb * gz - pc * gy) * (0.5f * SamplePeriod);
            q3 = pb + (q1 * gy - pa * gz + pc * gx) * (0.5f * SamplePeriod);
            q4 = pc + (q1 * gz + pa * gy - pb * gx) * (0.5f * SamplePeriod);

            /* Normalise quaternion. */
            float_tmp_0 = q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4;
            double_tmp_0 = (double)float_tmp_0;
            double_tmp_0 = sqrt(double_tmp_0);
            if(fabs(double_tmp_0) > 0.0)
            {
                norm = (float)double_tmp_0;
                norm = 1.0f / norm;
                fl_api_body_angle_quaternion[0] = q1 * norm;
                fl_api_body_angle_quaternion[1] = q2 * norm;
                fl_api_body_angle_quaternion[2] = q3 * norm;
                fl_api_body_angle_quaternion[3] = q4 * norm;
            }
            else
            {
                /* handle NaN. */
                 be_result = BOARD_ERR_ERROR;
            }
        }
        else
        {
            /* handle NaN. */
            be_result = BOARD_ERR_ERROR;
        }
    }
    else
    {
        /* handle NaN. */
        be_result = BOARD_ERR_ERROR;
    }
    return(be_result);
}


        /* <summary> */
        /* Algorithm IMU update method. Requires only gyroscope and accelerometer data.*/
        /* </summary>*/
        /* <param name="gx">*/
        /* Gyroscope x axis measurement in radians/s.*/
        /* </param>*/
        /* <param name="gy">*/
        /* Gyroscope y axis measurement in radians/s.*/
        /* </param>*/
        /* <param name="gz">*/
        /* Gyroscope z axis measurement in radians/s.*/
        /* </param>*/
        /* <param name="ax">*/
        /* Accelerometer x axis measurement in any calibrated units.*/
        /* </param>*/
        /* <param name="ay">*/
        /* Accelerometer y axis measurement in any calibrated units.*/
        /* </param>*/
        /* <param name="az">*/
        /* Accelerometer z axis measurement in any calibrated units.*/
        /* </param>*/
BOARD_ERROR mahony_update_AccGyro(float gx, float gy, float gz, float ax, float ay, float az)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    float Ki = mahony_Ki;
    float Kp = mahony_Kp;
    float SamplePeriod;
    /* short name local variable for readability*/
    float q1 = fl_api_body_angle_quaternion[0];
    float q2 = fl_api_body_angle_quaternion[1];
    float q3 = fl_api_body_angle_quaternion[2];
    float q4 = fl_api_body_angle_quaternion[3];

    float norm;
    float vx, vy, vz;
    float ex, ey, ez;
    float pa, pb, pc;

    float  float_tmp_0;
    double double_tmp_0;

    SamplePeriod = fl_api_body_angle_sample_period;

    /* Normalise accelerometer measurement*/
    /* norm = (float)sqrt(ax * ax + ay * ay + az * az); */
    float_tmp_0 = ax * ax + ay * ay + az * az;
    double_tmp_0 = (double)float_tmp_0;
    double_tmp_0 = sqrt(double_tmp_0);
    if(fabs(double_tmp_0) > 0.0)
    {
        norm = (float)double_tmp_0;
        norm = 1.0f / norm;         /* use reciprocal for division. */
        ax *= norm;
        ay *= norm;
        az *= norm;

        /* Estimated direction of gravity. */
        vx = 2.0f * (q2 * q4 - q1 * q3);
        vy = 2.0f * (q1 * q2 + q3 * q4);
        vz = q1 * q1 - q2 * q2 - q3 * q3 + q4 * q4;

        /* Error is cross product between estimated direction and measured direction of gravity. */
        ex = (ay * vz - az * vy);
        ey = (az * vx - ax * vz);
        ez = (ax * vy - ay * vx);
        if (Ki > 0.0)
        {
            eInt[0] += ex;      /* accumulate integral error. */
            eInt[1] += ey;
            eInt[2] += ez;
        }
        else
        {
            eInt[0] = 0.0f;     /* prevent integral wind up. */
            eInt[1] = 0.0f;
            eInt[2] = 0.0f;
        }

        /* Apply feedback terms. */
        gx = gx + Kp * ex + Ki * eInt[0];
        gy = gy + Kp * ey + Ki * eInt[1];
        gz = gz + Kp * ez + Ki * eInt[2];

        /* Integrate rate of change of quaternion. */
        pa = q2;
        pb = q3;
        pc = q4;
        q1 = q1 + (-q2 * gx - q3 * gy - q4 * gz) * (0.5f * SamplePeriod);
        q2 = pa + (q1 * gx + pb * gz - pc * gy) * (0.5f * SamplePeriod);
        q3 = pb + (q1 * gy - pa * gz + pc * gx) * (0.5f * SamplePeriod);
        q4 = pc + (q1 * gz + pa * gy - pb * gx) * (0.5f * SamplePeriod);

        /* Normalise quaternion. */
        /* norm = (float)sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4); */
        float_tmp_0 = q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4;
        double_tmp_0 = (double)float_tmp_0;
        double_tmp_0 = sqrt(double_tmp_0);
        if(fabs(double_tmp_0) > 0.0)
        {
            norm = (float)double_tmp_0;
            norm = 1.0f / norm;
            fl_api_body_angle_quaternion[0] = q1 * norm;
            fl_api_body_angle_quaternion[1] = q2 * norm;
            fl_api_body_angle_quaternion[2] = q3 * norm;
            fl_api_body_angle_quaternion[3] = q4 * norm;
        }
        else
        {
            be_result = BOARD_ERR_ERROR;
        }
    }
    else
    {
        /* handle NaN. */
        be_result = BOARD_ERR_ERROR;
    }
    return(be_result);
}
