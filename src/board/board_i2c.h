#ifndef BOARD_I2C_H
#define BOARD_I2C_H 1

#include "stm32f10x.h"
#include "board_system_type.h"
#include "board_NVIC.h"
#include "board_sys_tick.h"
#include "board_gyro.h"
#include "api_i2c_acquisition.h"


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


#define  ClockSpeed            100000U  /* 100000 is standart speed for i2c protocols. */

#define I2C_DIRECTION_TX 0U
#define I2C_DIRECTION_RX 1U

#define OwnAddress1 0x30U
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




/* Exported types ------------------------------------------------------------*/
typedef enum i2c_result
{
	NO_ERR 					= 0U,
	TIMEOUT 				= 1U,
	BUS_BUSY 				= 2U,
	SEND_START_ERR 			= 3U,
	ADDR_MATCH_ERR 			= 4U,
	ADDR_HEADER_MATCH_ERR 	= 5U,
	DATA_TIMEOUT 			= 6U,
	WAIT_COMM 				= 7U,
	STOP_TIMEOUT 			= 8U
}I2C_Result;

typedef enum i2c_state
{
	COMM_DONE 			= 0U, /* done successfully*/
	COMM_PRE 			= 1U,
	COMM_IN_PROCESS 	= 2U,
	CHECK_IN_PROCESS 	= 3U,
	COMM_EXIT 			= 4U /* exit since failure*/
}I2C_STATE;

#define Transmitter 0x00U
#define Receiver 0x01U
#define FALSE 0U
#define TRUE  1U
#define u8	uint8_t
#define u16	uint16_t

extern BOARD_U16_3X_DATA board_i2c_sensor_data;

BOARD_ERROR be_board_i2c_init(void);
static void board_i2c_unstick(void);
static void board_i2c_lowlevel_init(void);

BOARD_ERROR board_i2c_read(
                            uint8_t  u8_device_address,
                            uint8_t  u8_start_read_address,
                            uint16_t u16_number_byte_to_read,
                            uint8_t* pu8_pointer_to_buffer  /* pointer to bytes */
                          );

BOARD_ERROR board_i2c_write(uint8_t u8_device_address, uint8_t u8_write_address, uint8_t u8_write_data);

static BOARD_ERROR be_board_i2c_master_buffer_DMA_read(uint8_t* pBuffer,  uint16_t NumByteToRead, uint8_t SlaveAddress);
static BOARD_ERROR be_board_i2c_DMA_master_buffer_write(
                                                            uint8_t* pBuffer,
                                                            uint16_t NumByteToWrite,
                                                            uint8_t SlaveAddress
                                                       );

void DMA1_Channel6_IRQHandler(void);
void DMA1_Channel7_IRQHandler(void);
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);


/* Service functions. */
static void v_dma_ch7_one_time_read(void);
static void v_dma_ch6_one_time_write(void);

extern BOARD_ERROR be_board_i2c_write_start(uint8_t* pu8_buffer, uint16_t u16_num_byte_to_write, uint8_t u8_device_address);
extern BOARD_ERROR be_board_i2c_read_start(uint8_t* pu8_buffer, uint16_t u16_number_byte_to_read, uint8_t u8_device_address);



/* static void v_data_add(uint32_t u32_data); */


#endif