#ifndef BOARD_I2C_H
#define BOARD_I2C_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_NVIC.h"
#include "board_sys_tick.h"
#include "board_gyro.h"


/* I2C SPE mask */
#define CR1_PE_Set              ((uint16_t)0x0001)
#define CR1_PE_Reset            ((uint16_t)0xFFFE)

/* I2C START mask */
#define CR1_START_Set           ((uint16_t)0x0100)
#define CR1_START_Reset         ((uint16_t)0xFEFF)

#define CR1_POS_Set           ((uint16_t)0x0800)
#define CR1_POS_Reset         ((uint16_t)0xF7FF)

/* I2C STOP mask */
#define CR1_STOP_Set            ((uint16_t)0x0200)
#define CR1_STOP_Reset          ((uint16_t)0xFDFF)

/* I2C ACK mask */
#define CR1_ACK_Set             ((uint16_t)0x0400)
#define CR1_ACK_Reset           ((uint16_t)0xFBFF)

/* I2C ENARP mask */
#define CR1_ENARP_Set           ((uint16_t)0x0010)
#define CR1_ENARP_Reset         ((uint16_t)0xFFEF)

/* I2C NOSTRETCH mask */
#define CR1_NOSTRETCH_Set       ((uint16_t)0x0080)
#define CR1_NOSTRETCH_Reset     ((uint16_t)0xFF7F)

/* I2C registers Masks */
#define CR1_CLEAR_Mask          ((uint16_t)0xFBF5)

/* I2C DMAEN mask */
#define CR2_DMAEN_Set           ((uint16_t)0x0800)
#define CR2_DMAEN_Reset         ((uint16_t)0xF7FF)

/* I2C LAST mask */
#define CR2_LAST_Set            ((uint16_t)0x1000)
#define CR2_LAST_Reset          ((uint16_t)0xEFFF)

/* I2C FREQ mask */
#define CR2_FREQ_Reset          ((uint16_t)0xFFC0)

/* I2C ADD0 mask */
#define OAR1_ADD0_Set           ((uint16_t)0x0001)
#define OAR1_ADD0_Reset         ((uint16_t)0xFFFE)

/* I2C ENDUAL mask */
#define OAR2_ENDUAL_Set         ((uint16_t)0x0001)
#define OAR2_ENDUAL_Reset       ((uint16_t)0xFFFE)

/* I2C ADD2 mask */
#define OAR2_ADD2_Reset         ((uint16_t)0xFF01)

/* I2C F/S mask */
#define CCR_FS_Set              ((uint16_t)0x8000)

/* I2C CCR mask */
#define CCR_CCR_Set             ((uint16_t)0x0FFF)

/* I2C FLAG mask */
#define FLAG_Mask               ((uint32_t)0x00FFFFFF)

/* I2C Interrupt Enable mask */
#define ITEN_Mask               ((uint32_t)0x07000000)


#define I2C_IT_BUF                      ((uint16_t)0x0400)
#define I2C_IT_EVT                      ((uint16_t)0x0200)
#define I2C_IT_ERR                      ((uint16_t)0x0100)


#define  ClockSpeed            100000U

#define I2C_DIRECTION_TX 0U
#define I2C_DIRECTION_RX 1U

#define OwnAddress1 0x28U
#define OwnAddress2 0x30U


#define I2C1_DMA_CHANNEL_TX           DMA1_Channel6
#define I2C1_DMA_CHANNEL_RX           DMA1_Channel7

#define I2C2_DMA_CHANNEL_TX           DMA1_Channel4
#define I2C2_DMA_CHANNEL_RX           DMA1_Channel5

#define I2C1_DR_Address              0x40005410U
#define I2C2_DR_Address              0x40005810U

#define I2C_MODULE_MODE   DMA

typedef enum
{
    Polling = 0x00U,
    Interrupt = 0x01U,
    DMA = 0x02U
} I2C_ProgrammingModel;

typedef enum
{
    Error = 0,
    Success = 1
}Status;


BOARD_ERROR be_board_i2c_init(void);
static void board_i2c_unstick(void);
static void board_i2c_lowlevel_init(I2C_TypeDef* I2Cx);

BOARD_ERROR board_i2c_read(
                            uint8_t  u8_device_address,
                            uint8_t  u8_start_read_address,
                            uint32_t u32_number_byte_to_read,
                            uint8_t* pu8_pointer_to_buffer  /* pointer to bytes */
                          );
BOARD_ERROR board_i2c_write(uint8_t u8_device_address, uint8_t u8_write_address, uint8_t u8_write_data);

static BOARD_ERROR be_board_i2c_master_buffer_read(I2C_TypeDef* I2Cx, uint8_t* pBuffer,  uint32_t NumByteToRead, I2C_ProgrammingModel Mode, uint8_t SlaveAddress);
static BOARD_ERROR be_board_i2c_master_buffer_write(I2C_TypeDef* I2Cx, uint8_t* pBuffer,  uint32_t NumByteToWrite, I2C_ProgrammingModel Mode, uint8_t SlaveAddress);
void                  board_i2c_slave_buffer_read_write(I2C_TypeDef* I2Cx,I2C_ProgrammingModel Mode);
static void           board_i2c_dma_config(I2C_TypeDef* I2Cx, uint8_t* pBuffer, uint32_t BufferSize, uint32_t Direction);

void DMA1_Channel6_IRQHandler(void);


#endif