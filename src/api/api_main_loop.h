#ifndef API_MAIN_LOOP_H
#define API_MAIN_LOOP_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_main_loop.h"
#include "board_dma.h"

#include "api_i2c_acquisition.h"
#include "api_data_normalising.h"
#include "api_cmd.h"
#include "board_baro.h"

#include "MadgwickAHRS.h"
#include "api_end_device.h"
#include "api_filters.h"
#include "board_drv_bmp085.h"

/* Main loop interrupt period, should not be less
 * than sensor data acquisition time (2.7mSec)
 */
#define PERIOD_OF_MAIN_LOOP 4000U /* 3000 - 3mSec, 4000 - 4mSec. */

BOARD_ERROR be_api_main_loop_init(void);
BOARD_ERROR be_api_main_loop_start(void);
static  void v_api_main_loop_process(void);
static  void v_api_main_loop_control_loop(void);









#endif

