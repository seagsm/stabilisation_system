#ifndef BOARD_DRV_L3G4200D_H
#define BOARD_DRV_L3G4200D_H 1


#include "stm32f10x.h"
#include "board_i2c.h"
#include "board_sys_tick.h"
#include "board_system_type.h"


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

#define L3G4200D_FS_SEL_2000DPS  0xF0U  /* 2000 degree/sec */
#define L3G4200D_FS_SEL_500DPS   0xD0U  /* 500  degree/sec */
#define L3G4200D_FS_SEL_250DPS   0xC0U  /* 250  degree/sec */

#define L3G4200D_DLPF_32HZ       0x00U
#define L3G4200D_DLPF_54HZ       0x40U
#define L3G4200D_DLPF_78HZ       0x80U
#define L3G4200D_DLPF_93HZ       0xC0U

/* Current definition. */
#define BOARD_DRV_GYRO_RATE               L3G4200D_FS_SEL_500DPS
#define BOARD_DRV_LOWPASS_FILTER_RATE     L3G4200D_DLPF_32HZ

BOARD_ERROR be_board_drv_l3g4200d_init(void);
BOARD_ERROR be_board_drv_l3g4200d_align(void);
BOARD_ERROR be_board_drv_l3g4200d_read(BOARD_U16_3X_DATA *p_board_drv_gyro_data);
BOARD_ERROR be_board_drv_l3g4200d_detect(void);
BOARD_ERROR be_board_drv_l3g4200d_config(uint16_t u16_lpf);


#endif