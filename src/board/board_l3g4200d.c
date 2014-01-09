#include "board_l3g4200d.h"

/*Real L3G4200D 8 bit address. */
#define L3G4200D_ADDRESS         0xD3U
#define L3G4200D_ID              0xD3U

/* Registers */
#define L3G4200D_WHO_AM_I        0x0FU
#define L3G4200D_CTRL_REG1       0x20U
#define L3G4200D_CTRL_REG2       0x21U
#define L3G4200D_CTRL_REG3       0x22U
#define L3G4200D_CTRL_REG4       0x23U
#define L3G4200D_CTRL_REG5       0x24U
#define L3G4200D_REFERENCE       0x25U
#define L3G4200D_STATUS_REG      0x27U
#define L3G4200D_GYRO_OUT        0xA8U

/* Bits */
#define L3G4200D_POWER_ON        0x0FU
#define L3G4200D_FS_SEL_2000DPS  0xF0U
#define L3G4200D_FS_SEL_500DPS   0xD0U
#define L3G4200D_FS_SEL_250DPS   0xC0U
#define L3G4200D_DLPF_32HZ       0x00U
#define L3G4200D_DLPF_54HZ       0x40U
#define L3G4200D_DLPF_78HZ       0x80U
#define L3G4200D_DLPF_93HZ       0xC0U

static uint8_t mpuLowPassFilter = L3G4200D_DLPF_32HZ;
/* static uint8_t mpuLowPassFilter = L3G4200D_DLPF_93HZ; */


static void l3g4200dInit(void);
static void l3g4200dRead(void);
static void l3g4200dAlign(int16_t *gyroData);

uint32_t l3g4200dDetect(sensor_t *gyro)
{
    uint8_t deviceid;

    gv_board_sys_tick_fast_delay(25U);

    board_i2c_read(L3G4200D_ADDRESS, L3G4200D_WHO_AM_I, 1U, &deviceid);
    if (deviceid != L3G4200D_ID)
    {
      /*  return 0U;*/
    }
    gyro->init = l3g4200dInit;
    gyro->read = l3g4200dRead;
    gyro->align = l3g4200dAlign;

    return 1U;
}

void l3g4200dConfig(uint16_t lpf)
{
    switch (lpf)
    {
        case 32:
            mpuLowPassFilter = L3G4200D_DLPF_32HZ;
            break;
        case 54:
            mpuLowPassFilter = L3G4200D_DLPF_54HZ;
            break;
        case 78:
            mpuLowPassFilter = L3G4200D_DLPF_78HZ;
            break;
        case 93:
            mpuLowPassFilter = L3G4200D_DLPF_93HZ;
            break;
        default:
            break;
    }

    board_i2c_write(L3G4200D_ADDRESS, L3G4200D_CTRL_REG1, L3G4200D_POWER_ON | mpuLowPassFilter);
}

static void l3g4200dInit(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    gv_board_sys_tick_fast_delay(100U);
    /* setup gyro rate, see datasheet*/
    /* L3G4200D_FS_SEL_2000DPS  2000 degree/sec */
    /* L3G4200D_FS_SEL_500DPS   500 degree/sec */
    /* L3G4200D_FS_SEL_250DPS   250 degree/sec */
    be_result = board_i2c_write(L3G4200D_ADDRESS, L3G4200D_CTRL_REG4, L3G4200D_FS_SEL_250DPS);
    if (be_result != BOARD_ERR_OK)
    {
        failureMode(3U);
    }
    gv_board_sys_tick_fast_delay(5U);
    be_result = board_i2c_write(L3G4200D_ADDRESS, L3G4200D_CTRL_REG1, L3G4200D_POWER_ON | mpuLowPassFilter);
    if (be_result != BOARD_ERR_OK)
    {
        failureMode(3U);
    }
    gv_board_sys_tick_fast_delay(5U);
}

static void l3g4200dAlign(int16_t *gyroData)
{
 /* Any align is not neccessary. */
#if 0
    gyroData[0] = -gyroData[0];
    gyroData[1] = gyroData[1];
    gyroData[2] = -gyroData[2];
#endif
}

uint16_t gyroData[3];

/* Read 3 gyro values into user-provided buffer. No overrun checking is done. */
static void l3g4200dRead(void)
{
    uint8_t buf[6];
    board_i2c_read(L3G4200D_ADDRESS, L3G4200D_GYRO_OUT, 6U, buf);
    gyroData[0] = (uint16_t)buf[0];
    gyroData[0] = gyroData[0] << 8U;
    gyroData[0] = (((uint16_t)  buf[0]) << 8U) | ((uint16_t)buf[1]);/* / 4; */
    gyroData[1] = (uint16_t)((buf[2] << 8U) | buf[3]);/* / 4; */
    gyroData[2] = (uint16_t)((buf[4] << 8U) | buf[5]);/* / 4; */
}


void failureMode(uint8_t mode)
{


  while(1)
  {
    /* printf("Gyro falture %d", mode); */
  }
/* TBD
  LED1_ON;
    LED0_OFF;
    while (1) {
        LED1_TOGGLE;
        LED0_TOGGLE;
        delay(475 * mode - 2);
        BEEP_ON
        delay(25);
        BEEP_OFF;
    }
*/
}








