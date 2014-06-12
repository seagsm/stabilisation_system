#ifndef API_PID_H
#define API_PID_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "api_common.h"
#include "board_ppm.h"
#include "api_body_angle_calculation.h"
#include "api_rx_channels.h"
#include "api_end_device.h"

/* in 0.1 degree , 1000 = 100degree, 450 = 45 degree. */
#define MAX_ANGLE_INCLINATION 450

/* Pitch */
#define PITCH_P_GAIN        19
#define PITCH_P_GAIN_DYN    80
#define PITCH_I_GAIN        0
#define PITCH_I_GAIN_DYN    0
#define PITCH_I_MIN        -10000
#define PITCH_I_MAX         10000
#define PITCH_D_GAIN        60
#define PITCH_D_GAIN_DYN    0
/* ROll */
#define ROLL_P_GAIN         19
#define ROLL_P_GAIN_DYN     80
#define ROLL_I_GAIN         0
#define ROLL_I_GAIN_DYN     0
#define ROLL_I_MIN         -10000
#define ROLL_I_MAX          10000
#define ROLL_D_GAIN         60
#define ROLL_D_GAIN_DYN     0
/* Yaw */
#define YAW_P_GAIN          30
#define YAW_P_GAIN_DYN      100
#define YAW_I_GAIN          0
#define YAW_I_GAIN_DYN      0
#define YAW_I_MIN          -10000
#define YAW_I_MAX           10000
#define YAW_D_GAIN          0
#define YAW_D_GAIN_DYN      0

enum
{
  Roll  = 0,
  Pitch = 1,
  Yaw   = 2
};

typedef struct
{
  /* P */
  int32_t i32_p_gain;           /* proportional gain to keep angle level. */
  int32_t i32_p_dyn_gain;       /* proportional dynamic gain for angle speed. */

  /* I */
  int32_t i32_i_gain;           /* integral gain to keep angle level. */
  int32_t i32_i_dyn_gain;       /* integral dynamic gain for angle speed. */
  int32_t i32_i_state;          /* Integrator state,should be initialized to 0. */
  int32_t i32_i_max;	        /* Maximum allowable integrator state. */
  int32_t i32_i_min;            /* Minimum allowable integrator state. */

  /* D */
  int32_t i32_d_gain;           /* derivative gain to keep angle level. */
  int32_t i32_d_dyn_gain;       /* derivative dynamic gain for angle speed.*/
  int32_t i32_d_state;          /* should be initialized by 0. */
  int32_t i32_d_0_state;        /* should be initialized by 0. */
  int32_t i32_d_1_state;        /* should be initialized by 0. */
  int32_t i32_d_angle_speed;    /* for calculation delta angle speed, should be initialized by 0.*/

  /* PID output. */
  int32_t i32_pid_output;       /* PID output. */

} PID_element;


extern PID_element pid_api_pid_data[3];

        void api_pid_init(void);
        void api_pid_update_frame(void);
static  void api_pid_init_Pitch(void);
static  void api_pid_init_Roll(void);
static  void api_pid_init_Yaw(void);
static  void api_pid_update_PDF(PID_element *current_pid, int32_t i32_set_point, int32_t i32_angle_position, int32_t i32_angle_speed);

#endif