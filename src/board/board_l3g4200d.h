#ifndef BOARD_L3G4200D_H
#define BOARD_L3G4200D_H 1


#include "stm32f10x.h"
#include "board_i2c.h"
#include "board_sys_tick.h"
#include "board_system_type.h"


typedef void (* sensorInitFuncPtr)(void);                   /* sensor init prototype */
typedef void (* sensorReadFuncPtr)(void);          /* sensor read and align prototype */
typedef void (* sensorAlignFuncPtr)(int16_t *data);
/* typedef void (* sensorReadFuncPtr)(int16_t *data); */         /* sensor read and align prototype */

typedef struct
{
    sensorInitFuncPtr init;
    sensorReadFuncPtr read;
    sensorAlignFuncPtr align;
    sensorReadFuncPtr temperature;
} sensor_t;


uint32_t l3g4200dDetect(sensor_t * gyro);
void l3g4200dConfig(uint16_t lpf);
void failureMode(uint8_t mode);





#endif