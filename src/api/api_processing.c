/*
 * It is main interupt. Here We are reading datas from sensors
 * and do all processing, like filtering, kalman, calculation
 * PID, PWM and so on.
 *
 *
 */

#include "user_processing.h"
#include "main.h"

#define MSG_SIZE SENSOR_RAW_DATA_SIZE

float Quaternion[4] = {1,0,0,0};
float SamplePeriod = 1;
//    static uint8_t msg[MSG_SIZE];

// this function called from timer interrupt for reading and update all
// sensors states.
void start_processing(control_pid_est_t *board,sensor_t *gyro)
{
    int16_t  gyro_data[3] = {0,0,0};

    /* Read 3D accelerometer. */
    readAccel();
    //board->Acceleration_raw_X = rawAccel[XAXIS].value;
    //board->Acceleration_raw_Y = rawAccel[YAXIS].value;
    //board->Acceleration_raw_Z = rawAccel[ZAXIS].value;
    /* Read 3D Gyro sensor. */
    gyro->read(gyro_data);
    //board->Gyro_raw_XY = gyro_data[0];
    //board->Gyro_raw_YZ = gyro_data[1];
    //board->Gyro_raw_XZ = gyro_data[2];

    /* Read 3D Magnetometer sensor. */
    readMag();
    //board->Magnetometer_raw_X = rawMag[XAXIS].value;
    //board->Magnetometer_raw_Y = rawMag[YAXIS].value;
    //board->Magnetometer_raw_Z = rawMag[ZAXIS].value;

    /* Read Baro sensor. */
    /* Direct reading is too long. */
    //read_altitude();
    //board->Barometer_Altitude_raw = pressureAlt;
    //board->Barometer_Temperature  = (b5+8)/16;
}
//Processing function. It read data from sensors and call Update function.
void start_processing_sensors(sensor_raw_data_t *board,sensor_t *gyro)
{
    int16_t  gyro_data[3] = {0,0,0};
    sensor_float_data_t s_Data;
    euler_angle_t euler_angle; // here we save euler angles calculated from quaternions.

   /* int32_t roll_output, counter_plus,counter_minus; */
   /* static uint32_t current_counter_plus=0,current_counter_minus=0; */

    /* Motor control. */
    int32_t pitch_sensor_current_position;
    int32_t roll_sensor_current_position;
    int32_t yaw_sensor_current_position;

    static int32_t pitch_motor_current_position=0;
    static int32_t roll_motor_current_position=0;
    static int32_t yaw_motor_current_position=0;

    static int32_t init_position_counter=0;
    static int32_t init_position_flag=0;



    static uint32_t old_time=0, delta_time,time_counter = 0;

    time_counter++;
    if(time_counter >= 10)
    {
      time_counter = 0;
    }

    if(time_counter == 0)
    {
      /* Read 3D accelerometer. */
      readAccel();
      board->Acceleration_raw_X = rawAccel[XAXIS].value;
      board->Acceleration_raw_Y = rawAccel[YAXIS].value;
      board->Acceleration_raw_Z = rawAccel[ZAXIS].value;
    }
    /* Read 3D Gyro sensor. */
    gyro->read(gyro_data);
    board->Gyro_raw_XY = gyro_data[0];
    board->Gyro_raw_XZ = gyro_data[1];
    board->Gyro_raw_YZ = gyro_data[2];

    if(time_counter == 0)
    {
      /* Read 3D Magnetometer sensor. */
      readMag();
      board->Magnetometer_raw_X = rawMag[XAXIS].value;
      board->Magnetometer_raw_Y = rawMag[YAXIS].value;
      board->Magnetometer_raw_Z = rawMag[ZAXIS].value;
    }
    //Read current board time.
    board->BoardSystemTime = (uint32_t) read_system_time();

    /* Align of axis. */
    s_Data.Gyro_X = deg2rad(-(float)board->Gyro_raw_XY * GyroCoef);
    s_Data.Gyro_Y = deg2rad(-(float)board->Gyro_raw_XZ * GyroCoef);
    s_Data.Gyro_Z = deg2rad( (float)board->Gyro_raw_YZ * GyroCoef);
    s_Data.Acceleration_X = -(float)board->Acceleration_raw_Y;
    s_Data.Acceleration_Y = -(float)board->Acceleration_raw_X;
    s_Data.Acceleration_Z =  (float)board->Acceleration_raw_Z;
    s_Data.Magnetometer_X =  (float)board->Magnetometer_raw_X;
    s_Data.Magnetometer_Y =  (float)board->Magnetometer_raw_Y;
    s_Data.Magnetometer_Z = -(float)board->Magnetometer_raw_Z;

    /* Set coef for Update function. */
    mahony_Ki =0;
    mahony_Kp =0.5;
    madgwick_Beta  = 0.1;

    /* Calc delta time for update function. */
    /* Possible use fixed value of interrupt time TIMET 1. */
    delta_time = board->BoardSystemTime - old_time;
    old_time = board->BoardSystemTime;
    SamplePeriod = (float)delta_time/1000.0;

    GPIO_SetBits( GPIOB, GPIO_Pin_0);
    /* filter data */
    s_Data.Gyro_X = filterGyroX(s_Data.Gyro_X);
    s_Data.Gyro_Y = filterGyroY(s_Data.Gyro_Y);
    s_Data.Gyro_Z = filterGyroZ(s_Data.Gyro_Z);
    s_Data.Acceleration_X = filterAccelX(s_Data.Acceleration_X);
    s_Data.Acceleration_Y = filterAccelX(s_Data.Acceleration_Y);
    s_Data.Acceleration_Z = filterAccelX(s_Data.Acceleration_Z);
    s_Data.Magnetometer_X = filterMagnX(s_Data.Magnetometer_X);
    s_Data.Magnetometer_Y = filterMagnY(s_Data.Magnetometer_Y);
    s_Data.Magnetometer_Z = filterMagnZ(s_Data.Magnetometer_Z);

    magnet_axis_correction(&s_Data);

#if 1
    mahony_update_AccGyroMag(
                        s_Data.Gyro_X,
                        s_Data.Gyro_Y,
                        s_Data.Gyro_Z,
                        s_Data.Acceleration_X,
                        s_Data.Acceleration_Y,
                        s_Data.Acceleration_Z,
                        s_Data.Magnetometer_X,
                        s_Data.Magnetometer_Y,
                        s_Data.Magnetometer_Z
                     );
#else
    madgwick_update_AccGyroMag(
                        s_Data.Gyro_X,
                        s_Data.Gyro_Y,
                        s_Data.Gyro_Z,
                        s_Data.Acceleration_X,
                        s_Data.Acceleration_Y,
                        s_Data.Acceleration_Z,
                        s_Data.Magnetometer_X,
                        s_Data.Magnetometer_Y,
                        s_Data.Magnetometer_Z
                     );
#endif

    board->Quaternion[0].f_value = Quaternion[0];
    board->Quaternion[1].f_value = Quaternion[1];
    board->Quaternion[2].f_value = Quaternion[2];
    board->Quaternion[3].f_value = Quaternion[3];
#if 0 /* This version work , but for small angles. */
    convert_quaternion_to_euler_angles(&euler_angle);
#endif
    /* This is HAB version. */
    convert_quaternion_to_euler_angles_v2(&euler_angle);

///* Temporary removed   during debuging
    board->Euler_Angle[0].f_value = euler_angle.phi;
    board->Euler_Angle[1].f_value = euler_angle.theta;
    board->Euler_Angle[2].f_value = euler_angle.psi;
//*/
    GPIO_ResetBits( GPIOB, GPIO_Pin_0);


/*****************************************/
/* Here we should call driving function. */
/*****************************************/
    /* Update board PID.*/

    /* Pitch */
    board->PID->Pitch_kP_pid         = PID[Pitch].p_gain;
    board->PID->Pitch_kP_pid_dynamic = PID[Pitch].p_dyn_gain;
    board->PID->Pitch_kI_pid         = PID[Pitch].i_gain;
    board->PID->Pitch_kI_pid_dynamic = PID[Pitch].i_dyn_gain;
    board->PID->Pitch_kD_pid         = PID[Pitch].d_gain;
    board->PID->Pitch_kD_pid_dynamic = PID[Pitch].d_dyn_gain;

    /* Roll */
    board->PID->Roll_kP_pid         = PID[Roll].p_gain;
    board->PID->Roll_kP_pid_dynamic = PID[Roll].p_dyn_gain;
    board->PID->Roll_kI_pid         = PID[Roll].i_gain;
    board->PID->Roll_kI_pid_dynamic = PID[Roll].i_dyn_gain;
    board->PID->Roll_kD_pid         = PID[Roll].d_gain;
    board->PID->Roll_kD_pid_dynamic = PID[Roll].d_dyn_gain;

    /* Yaw */
    board->PID->Yaw_kP_pid         = PID[Yaw].p_gain;
    board->PID->Yaw_kP_pid_dynamic = PID[Yaw].p_dyn_gain;
    board->PID->Yaw_kI_pid         = PID[Yaw].i_gain;
    board->PID->Yaw_kI_pid_dynamic = PID[Yaw].i_dyn_gain;
    board->PID->Yaw_kD_pid         = PID[Yaw].d_gain;
    board->PID->Yaw_kD_pid_dynamic = PID[Yaw].d_dyn_gain;

    /* Open loop */

    //yaw_sensor_current_position = ((int32_t)(euler_angle.psi*1000.0))/(200);

    if(init_position_flag==0)
    {
      /* Here we are waiting for timeout till to angle value will be stabilised */
      if(init_position_counter < 1000)
      {
          init_position_counter++;
      }
      else
      {
          init_position_flag=1;
          /* Init motor position by current angle position. */
          pitch_motor_current_position = (int32_t)((euler_angle.theta*1000.0)/(200));
          roll_motor_current_position = (int32_t)((euler_angle.phi*1000.0)/(200));
          yaw_motor_current_position = (int32_t)((euler_angle.psi*1000.0)/(200));
      }
    }
    else
    {
        /* Stat regulation.*/
        pitch_sensor_current_position = (int32_t)((euler_angle.theta*10.0)/(2.0));
        roll_sensor_current_position = (int32_t)((euler_angle.phi*10.0)/(2.0));
        yaw_sensor_current_position = (int32_t)((euler_angle.psi*10.0)/(2.0));


        /* PITCH *****************************************************************************************************************/
        /*TODO: We use PID[Pitch].i_gain like parameter to change angle position of motor. */
        pitch_sensor_current_position = pitch_sensor_current_position +  PID[Pitch].i_gain;
        /* check if angle overfloat 180 degree. */
        if(pitch_sensor_current_position > 900)
        {
          pitch_sensor_current_position = -1800 + pitch_sensor_current_position;
        }
        if(pitch_sensor_current_position < -900)
        {
          pitch_sensor_current_position = 1800 + pitch_sensor_current_position;
        }

        /* Make step ingine. Open loop control only. If we will use motors with rotary encoder, it posible to make close loop. */
        if(pitch_motor_current_position > (pitch_sensor_current_position))
        {
            if( fabs( pitch_motor_current_position - pitch_sensor_current_position ) > 900   )
            {
                /* make pulse two time to increase speed*/
                pitch_motor_current_position++;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_6);
                delay(400);
                pitch_motor_current_position++;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_6);

                if(pitch_motor_current_position > 900)
                {
                    pitch_motor_current_position = -899;
                }
            }
            else
            {
                /* make pulse two time to increase speed*/
                pitch_motor_current_position--;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_7);
                delay(400);
                pitch_motor_current_position--;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_7);
            }
        }
        if(pitch_motor_current_position < (pitch_sensor_current_position))
        {
            if( fabs( pitch_motor_current_position - pitch_sensor_current_position ) > 900   )
            {
                /* make pulse two time to increase speed*/
                pitch_motor_current_position--;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_7);
                delay(400);
                pitch_motor_current_position--;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_7);
                if(pitch_motor_current_position < -900)
                {
                    pitch_motor_current_position = 899;
                }
            }
            else
            {
                /* make pulse two time to increase speed*/
                pitch_motor_current_position++;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_6);
                delay(400);
                pitch_motor_current_position++;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_6);
            }
        }
        /* END PITCH ************************************************************/


        /* ROLL *****************************************************************************************************************/
        /*TODO: We use PID[Roll].i_gain like parameter to change angle position of motor. */
        roll_sensor_current_position = roll_sensor_current_position +  PID[Roll].i_gain;
        /* check if angle overfloat 180 degree. */
        if(roll_sensor_current_position > 900)
        {
          roll_sensor_current_position = -1800 + roll_sensor_current_position;
        }
        if(roll_sensor_current_position < -900)
        {
          roll_sensor_current_position = 1800 + roll_sensor_current_position;
        }

        /* Make step ingine. Open loop control only. If we will use motors with rotary encoder, it posible to make close loop. */
        if(roll_motor_current_position > (roll_sensor_current_position))
        {
            if( fabs( roll_motor_current_position - roll_sensor_current_position ) > 900   )
            {
                /* make pulse two time to increase speed*/
                roll_motor_current_position++;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_2);
                delay(400);
                roll_motor_current_position++;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_2);

                if(roll_motor_current_position > 900)
                {
                    roll_motor_current_position = -899;
                }
            }
            else
            {
                /* make pulse two time to increase speed*/
                roll_motor_current_position--;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_3);
                delay(400);
                roll_motor_current_position--;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_3);
            }
        }
        if(roll_motor_current_position < (roll_sensor_current_position))
        {
            if( fabs( roll_motor_current_position - roll_sensor_current_position ) > 900   )
            {
                /* make pulse two time to increase speed*/
                roll_motor_current_position--;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_3);
                delay(400);
                roll_motor_current_position--;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_3);
                if(roll_motor_current_position < -900)
                {
                    roll_motor_current_position = 899;
                }
            }
            else
            {
                /* make pulse two time to increase speed*/
                roll_motor_current_position++;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_2);
                delay(400);
                roll_motor_current_position++;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_2);
            }
        }
        /* END ROLL ************************************************************/

        /* YAW *****************************************************************************************************************/
        /*TODO: We use PID[Yaw].i_gain like parameter to change angle position of motor. */
        yaw_sensor_current_position = yaw_sensor_current_position +  PID[Yaw].i_gain;
        /* check if angle overfloat 180 degree. */
        if(yaw_sensor_current_position > 900)
        {
          yaw_sensor_current_position = -1800 + yaw_sensor_current_position;
        }
        if(yaw_sensor_current_position < -900)
        {
          yaw_sensor_current_position = 1800 + yaw_sensor_current_position;
        }

        /* Make step ingine. Open loop control only. If we will use motors with rotary encoder, it posible to make close loop. */
        if(yaw_motor_current_position > (yaw_sensor_current_position))
        {
            if( fabs( yaw_motor_current_position - yaw_sensor_current_position ) > 900   )
            {
                /* make pulse two time to increase speed*/
                yaw_motor_current_position++;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_0);
                delay(400);
                yaw_motor_current_position++;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_0);

                if(yaw_motor_current_position > 900)
                {
                    yaw_motor_current_position = -899;
                }
            }
            else
            {
                /* make pulse two time to increase speed*/
                yaw_motor_current_position--;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_1);
                delay(400);
                yaw_motor_current_position--;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_1);
            }
        }
        if(yaw_motor_current_position < (yaw_sensor_current_position))
        {
            if( fabs( yaw_motor_current_position - yaw_sensor_current_position ) > 900   )
            {
                /* make pulse two time to increase speed*/
                yaw_motor_current_position--;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_1);
                delay(400);
                yaw_motor_current_position--;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_1);
                if(yaw_motor_current_position < -900)
                {
                    yaw_motor_current_position = 899;
                }
            }
            else
            {
                /* make pulse two time to increase speed*/
                yaw_motor_current_position++;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_0);
                delay(400);
                yaw_motor_current_position++;
                GPIO_ToggleBits( GPIOA, GPIO_Pin_0);
            }
        }
/* END YAW ************************************************************/
        /* Temporary evaluation */
     //   board->Euler_Angle[0].f_value = (float)yaw_sensor_current_position;
     //  board->Euler_Angle[1].f_value = (float)yaw_motor_current_position;
    }






#if 0  /* Loopback controll */
    /*This is for Pitch roll_output = update_PDF(&PID[Pitch],0,(int32_t)(euler_angle.theta*100.0), (int32_t) ( (rad2deg( s_Data.Gyro_Y))/GyroCoef)); */

    /*This is for Roll roll_output = update_PDF(&PID[Roll],0,(int32_t)(euler_angle.phi*100.0), (int32_t) ( (rad2deg( s_Data.Gyro_X))/GyroCoef)); */

    roll_output = update_PDF(&PID[Yaw],0,(int32_t)(euler_angle.psi*100.0), (int32_t) ( (rad2deg( s_Data.Gyro_Z))/GyroCoef));

    /* Just for test , we are send data through euler angles. */
  //  board->Euler_Angle[0].f_value = roll_output;
  //  board->Euler_Angle[1].f_value = euler_angle.theta*100.0;
  //  board->Euler_Angle[2].f_value = ( (rad2deg( s_Data.Gyro_Y))/GyroCoef);

//counter_plus,counter_minus
    current_counter_plus++;
    current_counter_minus++;
    if(roll_output > 1)
    {
      counter_plus = (int32_t)(100/roll_output);
    //  board->Euler_Angle[0].f_value = (float)counter_plus;
      if(current_counter_plus > counter_plus)
      {
          GPIO_ToggleBits( GPIOA, GPIO_Pin_7);
          delay(400);
          GPIO_ToggleBits( GPIOA, GPIO_Pin_7);
          delay(400);
          GPIO_ToggleBits( GPIOA, GPIO_Pin_7);
          current_counter_plus = 0;
      }

    }
    if(roll_output < -1)
    {
      counter_minus = (int32_t)(-100/roll_output);
    //  board->Euler_Angle[0].f_value = (float)counter_minus;

      if(current_counter_minus > counter_minus)
      {
          GPIO_ToggleBits( GPIOA, GPIO_Pin_6);
          delay(400);
          GPIO_ToggleBits( GPIOA, GPIO_Pin_6);
          delay(400);
          GPIO_ToggleBits( GPIOA, GPIO_Pin_6);
          current_counter_minus = 0;
      }
    }

#endif
/*****************************************/




/* Send data through serial port. */
    if(time_counter == 0)
    {


      {
        static uint8_t send_counter =0;

        if(send_counter >=2)
        {
          sensors_raw_data_serialisation_api(msg,board, sizeof(msg));
          send_counter = 0;
        }
        send_counter++;
      }

    }
}


/*
 * Function convert degree to radians.
 * It is neccessary for Update algoritm.
 */
float deg2rad(float degrees)
{
    return (float)(_M_PI / 180) * degrees;
}

/* Function convert degree to radians*/
float rad2deg(float rad)
{
    return (float)(180/_M_PI) * rad;
}



/* This function correct magnetic sensor in according with correction matrix. */
/* Additionaly it calculate ptoection of mag vector to horizont */
void magnet_axis_correction(sensor_float_data_t* sensor)
{
  float Ax, Ay, Az;
  float Mx, My, Mz;
  float norm;
  float A[3] = { 0, 0, 0 };
  float B[3] = { 0, 0, 0 };
  float C[3] = { 0, 0, 0 };

  float bias_X, bias_Y, bias_Z;

  Ax = sensor->Acceleration_X;
  Ay = sensor->Acceleration_Y;
  Az = sensor->Acceleration_Z;


/* Correction BIASES , it calculated by http://sailboatinstruments.blogspot.cz/2011/08/improved-magnetometer-calibration.html
 * MagCal
 */
  bias_X = - 104.0;
  bias_Y = - 41.0;
  bias_Z = - 350.0;

  Mx = sensor->Magnetometer_X + bias_X;
  My = sensor->Magnetometer_Y + bias_Y;
  Mz = sensor->Magnetometer_Z - bias_Z; // in according with Z orientation

 /* Correction matrix, it calculated by http://sailboatinstruments.blogspot.cz/2011/08/improved-magnetometer-calibration.html
  * MagCal
  */
  float E[] = {
                6.1223f, -0.0505f, -0.0721f,
               -0.0505f,  6.1163f, -0.1209f,
               -0.0721f, -0.1209f,  5.6381f
              };

  A[0] = Ax;
  A[1] = Ay;
  A[2] = Az;

  /* Calculate correction by muctiplication matrix E and vector M. */
  B[0] = E[0] * Mx + E[1] * My + E[2] * Mz;
  B[1] = E[3] * Mx + E[4] * My + E[5] * Mz;
  B[2] = E[6] * Mx + E[7] * My + E[8] * Mz;

#if 0   /* Just corrected magn vector. */
  sensor->Magnetometer_X = B[0];
  sensor->Magnetometer_Y = B[2];
  sensor->Magnetometer_Z = B[2];
#else  /* Here we can calculate proection. */
  /*TODO:
   * We need carefully check is it neccessary to calculate proection at all or not.
   *
   */
   /* Normalising accelerometer vector. */
   norm =(float)sqrt(A[0] * A[0] + A[1] * A[1] + A[2] * A[2]);
   A[0] = A[0]/norm;
   A[1] = A[1]/norm;
   A[2] = A[2]/norm;
   /* Normalising magnetometer vector. */
   norm =(float)sqrt(B[0] * B[0] + B[1] * B[1] + B[2] * B[2]);
   B[0] = B[0]/norm;
   B[1] = B[1]/norm;
   B[2] = B[2]/norm;

   /* Calculate cross of Accel vector and Magnet vector. */
   /* Result is normal vector to AccelMagnet plane. */
   C[0] = A[1] * B[2] - B[1] * A[2];
   C[1] = A[2] * B[0] - B[2] * A[0];
   C[2] = A[0] * B[1] - B[0] * A[1];

   /* Calculate cross of Accel vector and Magnet vectornormal vector to AccelMagnet plane. */
   /* Result is Magnet azimut vector in rotated coordinats. */

   sensor->Magnetometer_X = C[1] * A[2] - A[1] * C[2];
   sensor->Magnetometer_Y = C[2] * A[0] - A[2] * C[0];
   sensor->Magnetometer_Z = C[0] * A[1] - A[0] * C[1];

#endif
}

/* Function convert quaternions to Euler Angles. */
void convert_quaternion_to_euler_angles(euler_angle_t* euler_angle)
{
    float phi,theta,psi;

#if 0 /* It is conversion from madgwick */
    phi = (float)atan2(2 * (Quaternion[2] * Quaternion[3] - Quaternion[0] * Quaternion[1]), 2 * Quaternion[0] * Quaternion[0] - 1 + 2 * Quaternion[3] * Quaternion[3]);
    theta = (float)-atan((2.0 * (Quaternion[1] * Quaternion[3] + Quaternion[0] * Quaternion[2])) / sqrt(1.0 - pow((2.0 * Quaternion[1] * Quaternion[3] + 2.0 * Quaternion[0] * Quaternion[2]), 2.0)));
    psi = (float)atan2(2 * (Quaternion[1] * Quaternion[2] - Quaternion[0] * Quaternion[3]), 2 * Quaternion[0] * Quaternion[0] - 1 + 2 * Quaternion[1] * Quaternion[1]);
#endif

    /* Method My */
    phi = -(float)atan2 (2*(Quaternion[0] * Quaternion[1] + Quaternion[2] * Quaternion[3]), 1 - 2 * (Quaternion[1] * Quaternion[1] + Quaternion[2] * Quaternion[2]));
    theta = -(float)asin    (2*(Quaternion[0] * Quaternion[2] - Quaternion[3] * Quaternion[1]));
    psi = -(float)atan2 (2 * (Quaternion[0] * Quaternion[3] + Quaternion[1] * Quaternion[2]), 1 - 2 * (Quaternion[2] * Quaternion[2] + Quaternion[3] * Quaternion[3]));


    euler_angle->phi   = rad2deg(phi);
    euler_angle->theta = rad2deg(theta);
    euler_angle->psi   = rad2deg(psi);
 }

/* Function convert quaternions to Euler Angles. */
void convert_quaternion_to_euler_angles_v2(euler_angle_t* euler_angle)
{
    float heading;
		float attitude;
		float bank;
    float test;
    float sqx;
    float sqy;
    float sqz;

   	test = Quaternion[1] * Quaternion[2] + Quaternion[3] * Quaternion[0];
	  if (test > 0.499)
    { // singularity at north pole
        heading = 2 * atan2(Quaternion[1],Quaternion[0]);
		    attitude = PI/2;
		    bank = 0;
		}
	  if (test < -0.499)
    { // singularity at south pole
        heading = -2 * atan2(Quaternion[1],Quaternion[0]);
		    attitude = - PI/2;
		    bank = 0;
		}
    sqx = Quaternion[1] * Quaternion[1];
    sqy = Quaternion[2] * Quaternion[2];
    sqz = Quaternion[3] * Quaternion[3];
    heading = atan2(2 * Quaternion[2] * Quaternion[0] - 2 * Quaternion[1] * Quaternion[3] , 1 - 2 * sqy - 2 * sqz);
	  attitude = asin(2 * test);
    bank = atan2(2 * Quaternion[1] * Quaternion[0] - 2 * Quaternion[2] * Quaternion[3], 1 - 2 * sqx - 2 * sqz);

    euler_angle->phi   = -rad2deg(bank);
    euler_angle->theta = -rad2deg(heading);//Pitch
    euler_angle->psi   = -rad2deg(attitude);

}
