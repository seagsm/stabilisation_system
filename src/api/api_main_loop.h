#ifndef API_MAIN_LOOP_H
#define API_MAIN_LOOP_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_main_loop.h"

#include "api_i2c_acquisition.h"

/* Main loop interrupt period, should not be less
 * than sensor data acquisition time (2.7mSec)
 */
#define PERIOD_OF_MAIN_LOOP 3500U /* 3000 - 3mSec, 4000 - 4mSec. */


extern    BOARD_U16_3X_DATA bu163d_api_main_loop_gyro_raw_data;
extern    BOARD_U16_3X_DATA bu163d_api_main_loop_acce_raw_data;
extern    BOARD_U16_3X_DATA bu163d_api_main_loop_magn_raw_data;


BOARD_ERROR be_api_main_loop_init(void);
BOARD_ERROR be_api_main_loop_start(void);
static  void v_api_main_loop_process(void);
static  void v_api_main_loop_sensor_data_preprocessing(void);
static  void v_api_main_loop_control_loop(void);









#endif

