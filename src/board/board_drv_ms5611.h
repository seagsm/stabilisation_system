#ifndef BOARD_DRV_MS5611_H
#define BOARD_DRV_MS5611_H 1

#include "stm32f10x.h"
#include "math.h"
#include "board_system_type.h"
#include "board_sys_tick.h"
#include "board_i2c.h"



/* MS5611, Standard address 0x77 */
#define MS5611_ADDR                          0xECU   /* 111011Cx , C = 0 */

/*  Autodetect: turn off BMP085 while initializing ms5611 and check PROM crc to confirm device */
#define BMP085_OFF                  digitalLo(BARO_GPIO, BARO_PIN);
#define BMP085_ON                   digitalHi(BARO_GPIO, BARO_PIN);


#define CMD_RESET               0x1EU /* ADC reset command       */
#define CMD_ADC_READ            0x00U /* ADC read command        */
#define CMD_ADC_CONV            0x40U /* ADC conversion command  */
#define CMD_ADC_D1              0x00U /* ADC D1 conversion       */
#define CMD_ADC_D2              0x10U /* ADC D2 conversion       */
#define CMD_ADC_256             0x00U /* ADC OSR=256             */
#define CMD_ADC_512             0x02U /* ADC OSR=512             */
#define CMD_ADC_1024            0x04U /* ADC OSR=1024            */
#define CMD_ADC_2048            0x06U /* ADC OSR=2048            */
#define CMD_ADC_4096            0x08U /* ADC OSR=4096            */
#define CMD_PROM_RD             0xA0U /* Prom read command       */
#define PROM_NB                 8U

typedef void (* baroOpFuncPtr)(void);                                           /* baro start operation */
typedef void (* baroCalculateFuncPtr)(int32_t *pressure, int32_t *temperature); /* baro calculation (filled params are */

typedef struct Baro_t
{
    uint16_t ut_delay;
    uint16_t up_delay;
    baroOpFuncPtr start_ut;
    baroOpFuncPtr get_ut;
    baroOpFuncPtr start_up;
    baroOpFuncPtr get_up;
    baroCalculateFuncPtr calculate;
} baro_t;



BOARD_ERROR ms5611Detect(baro_t *baro);
BOARD_ERROR ms5611_reset(void);






#endif