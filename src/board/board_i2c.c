/**
  ******************************************************************************
  * @file OptimizedI2Cexamples/src/I2CRoutines.c
  * @author  MCD Application Team
  * @version  V4.0.0
  * @date  06/18/2010
  * @brief  Contains the I2Cx slave/Master read and write routines.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/


#include "board_i2c.h"

#define DMA_ENABLE 1U

static DMA_InitTypeDef   I2CDMA_InitStructure;
static volatile uint32_t I2CDirection = I2C_DIRECTION_TX;
static volatile uint32_t NumbOfBytes1;
static volatile uint32_t NumbOfBytes2;
static volatile uint8_t  Address;

static volatile uint16_t u16_I2C_DMA_TC_flag = 0U;



/* Local variables -------------------------------------------------------*/
static vu8 MasterDirection = Transmitter;
static u16 SlaveADDR;
static u16 check_begin = FALSE;

static vu8 MasterReceptionComplete = 0U;
vu8 MasterTransitionComplete = 0U; /* to indicat master's send process */
vu8 WriteComplete = 0U; /* to indicat target's internal write process */

int I2C_NumByteToWrite = 0U;
u8 I2C_NumByteWritingNow = 0U;
u8* I2C_pBuffer = 0U;
u16 I2C_WriteAddr = 0U;
/*P-V operation on I2C1 */
static vu8 PV_flag_1;

static volatile I2C_STATE i2c_comm_state;

static uint8_t u8_rc_dma_buffer[6];











BOARD_ERROR be_board_i2c_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    NVIC_InitTypeDef NVIC_InitStructure;
    /************** I2C NVIC configuration *************************/
	/* NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); */
    /* Init I2C1 EV interrupt. */
	NVIC_InitStructure.NVIC_IRQChannel = (unsigned)I2C1_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =  I2C1_EV_PRIORITY_GROUP ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =         I2C1_EV_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    /* Init I2C1 ER interrupt. */
	NVIC_InitStructure.NVIC_IRQChannel = (unsigned)I2C1_ER_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =  I2C1_ER_PRIORITY_GROUP;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =         I2C1_ER_SUB_PRIORITY_GROUP;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);




    /* Restart stress for I2C2 slave device. */
    board_i2c_unstick();
    board_i2c_lowlevel_init(I2C1);

    /* It using for I2C test. */
    be_result = be_board_gyro_detect();
    return(be_result);
}

/* This function unstick I2C1 device. */
static void board_i2c_unstick(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    uint8_t u8_i;

    /*
        SCL  PB6(I2C1), PB10(I2C2)
        SDA  PB7(I2C1), PB11(I2C2)
     */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /* GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7);

    for (u8_i = 0U; u8_i < 8U; u8_i++)
    {
        while (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6))      /* Wait for any clock stretching to finish*/
        {
            gv_board_sys_tick_fast_delay(3U);
        }
        GPIO_ResetBits(GPIOB, GPIO_Pin_6);     /*Set bus low*/
        gv_board_sys_tick_fast_delay(3U);
        GPIO_SetBits(GPIOB, GPIO_Pin_6);       /*Set bus high */
        gv_board_sys_tick_fast_delay(3U);
    }

    /* Generate a start then stop condition. */
    GPIO_ResetBits(GPIOB, GPIO_Pin_7); /*Set bus data low */
    gv_board_sys_tick_fast_delay(3U);
    GPIO_ResetBits(GPIOB, GPIO_Pin_6); /*Set bus scl low */
    gv_board_sys_tick_fast_delay(3U);
    GPIO_SetBits(GPIOB, GPIO_Pin_6);   /*Set bus scl high */
    gv_board_sys_tick_fast_delay(3U);
    GPIO_SetBits(GPIOB, GPIO_Pin_7);   /*Set bus sda high */
    gv_board_sys_tick_fast_delay(3U);
}


/* This function write to I2C device. */
BOARD_ERROR board_i2c_write(
                                uint8_t u8_device_address,
                                uint8_t u8_write_address,
                                uint8_t u8_write_data
                            )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    uint8_t u8_write_buffer[2U]= { 0U, 0U};

    u8_write_buffer[0U] = u8_write_address;
    u8_write_buffer[1U] = u8_write_data;
    be_result = be_board_i2c_master_buffer_write(I2C1, u8_write_buffer,2U,I2C_MODULE_MODE, u8_device_address);
    return(be_result);
}

BOARD_ERROR board_i2c_read(
                            uint8_t  u8_device_address,
                            uint8_t  u8_start_read_address,
                            uint32_t u32_number_byte_to_read,
                            uint8_t* pu8_pointer_to_buffer  /* pointer to bytes */
                          )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    u16_I2C_DMA_TC_flag = 0U;
    /* Write read address for reading datas. */
    be_result  = be_board_i2c_master_buffer_write(I2C1, &u8_start_read_address,1U,I2C_MODULE_MODE, u8_device_address);
    /* Read MSB and LSB from address 0xF6. */
    be_result |= be_board_i2c_master_buffer_read (I2C1, pu8_pointer_to_buffer, u32_number_byte_to_read,I2C_MODULE_MODE, u8_device_address);

    return(be_result);
}

BOARD_ERROR board_i2c_DMA_read(
                            uint8_t  u8_device_address,
                            uint8_t  u8_start_read_address,
                            uint32_t u32_number_byte_to_read,
                            uint8_t* pu8_pointer_to_buffer  /* pointer to bytes */
                          )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    u16_I2C_DMA_TC_flag = 0U;
    /* Write read address for reading datas. */
    be_result  = be_board_i2c_master_buffer_write(I2C1, &u8_start_read_address,1U,I2C_MODULE_MODE, u8_device_address);
    /* Read MSB and LSB from address 0xF6. */
    be_result |= be_board_i2c_master_buffer_DMA_read (I2C1, pu8_pointer_to_buffer, u32_number_byte_to_read,I2C_MODULE_MODE, u8_device_address);


    return(be_result);
}









static BOARD_ERROR be_board_i2c_master_buffer_DMA_read(
                                                        I2C_TypeDef* I2Cx, uint8_t* pBuffer,
                                                        uint32_t NumByteToRead,
                                                        I2C_ProgrammingModel Mode,
                                                        uint8_t SlaveAddress
                                                      )

{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    volatile uint32_t temp = 0U;
    volatile uint32_t Timeout = 0U;

	/* PV operation */
	if (PV_flag_1 == 0U)
	{
        PV_flag_1 = 1U;
    /* Configure I2Cx DMA channel */
    /*** moved to here board_i2c_dma_config(I2Cx, pBuffer, NumByteToRead, I2C_DIRECTION_RX);*/

    /* Configure the DMA Rx Channel with the buffer address and the buffer size */

    /***I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer; */               /* pointer to buffer to save data. */
    I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)u8_rc_dma_buffer;                /* fixed local buffer for test. */
    I2CDMA_InitStructure.DMA_DIR            =           DMA_DIR_PeripheralSRC;  /* fixed for receive function */
    I2CDMA_InitStructure.DMA_BufferSize     = (uint32_t)NumByteToRead;          /* number byte for read. */

    I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;

    DMA_Cmd(I2C1_DMA_CHANNEL_RX, DISABLE);
    DMA_Init(I2C1_DMA_CHANNEL_RX, &I2CDMA_InitStructure);
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);

    /*** moved to interrupt DMA_Cmd(I2C1_DMA_CHANNEL_RX, ENABLE); */ /* I2C1_DMA_CHANNEL_RX <-> DMA1_Channel7*/



    /* Specifies if the next DMA transfer will be the last one. */
    /*** moved to interrupt I2C_DMALastTransferCmd(I2C1, ENABLE); */

    /* Enable I2C DMA requests */
    /*** moved to interrupt  I2C_DMACmd(I2C1, ENABLE);*/


    /***initialize static parameter*/
    MasterDirection = Receiver;
    MasterReceptionComplete = 0U;
    /***initialize static parameter according to input parameter*/
    SlaveADDR = SlaveAddress;
    /*** global state variable i2c_comm_state */
    i2c_comm_state = COMM_PRE;

    /***re-enable ACK bit incase it was disabled last call*/
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_ERR , ENABLE);/*only SB int allowed */


    /*** Send START condition */
    if(I2C1->CR1 & 0x200U)
    {
        I2C1->CR1 &= 0xFDFFU; /* No Stop generation */
    }
    /**/
    I2C_GenerateSTART(I2C1, ENABLE);
    }
    else
    {
        be_result = BOARD_ERR_BUSY;
    }

    return (be_result);
}

















/**
  * @brief  Reads buffer of bytes  from the slave.
  * @param pBuffer: Buffer of bytes to be read from the slave.
  * @param NumByteToRead: Number of bytes to be read by the Master.
  * @param Mode: Polling or DMA or Interrupt having the highest priority in the application.
  * @param SlaveAddress: The address of the slave to be addressed by the Master.
  * @retval : None.
  */
static BOARD_ERROR be_board_i2c_master_buffer_read(I2C_TypeDef* I2Cx, uint8_t* pBuffer,  uint32_t NumByteToRead, I2C_ProgrammingModel Mode, uint8_t SlaveAddress)

{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    volatile uint32_t temp = 0U;
    volatile uint32_t Timeout = 0U;

    /* Enable I2C errors interrupts (used in all modes: Polling, DMA and Interrupts */
    I2Cx->CR2 |= I2C_IT_ERR;
        DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, DISABLE);
    if (Mode == DMA) /* I2Cx Master Reception using DMA */
    {
        /* Configure I2Cx DMA channel */
        board_i2c_dma_config(I2Cx, pBuffer, NumByteToRead, I2C_DIRECTION_RX);

        /* Specifies if the next DMA transfer will be the last one. */
        I2C_DMALastTransferCmd(I2C1, ENABLE);

        /* Enable I2C DMA requests */
        I2C_DMACmd(I2C1, ENABLE);

        /* Send START condition */
        I2C_GenerateSTART(I2C1, ENABLE);

        /* Wait until SB flag is set: EV5  */
        Timeout = 0xFFFFU;
        while ((I2Cx->SR1&0x0001U) != 0x0001U)
        {
            if (Timeout-- == 0U)
            {
                /* return Error; */
                be_result = BOARD_ERR_ERROR;
               /* return (be_result); */
               break;
            }
        }

        Timeout = 0xFFFFU;
        /* Send slave address */
        I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Receiver);
        /* Wait until ADDR is set: EV6 */
        while ((I2Cx->SR1&0x0002U) != 0x0002U)
        {
            if (Timeout-- == 0U)
            {
                /* return Error; */
                be_result = BOARD_ERR_ERROR;
               /* return (be_result); */
               break;
            }
        }
        /* Clear ADDR flag by reading SR2 register */
        temp = I2Cx->SR2;
#if 1
        if (I2Cx == I2C1)
        {
            /* Wait until DMA end of transfer */

            GPIO_SetBits( GPIOB, GPIO_Pin_12);
            while (!DMA_GetFlagStatus(DMA1_FLAG_TC7))
            {}
            GPIO_ResetBits( GPIOB, GPIO_Pin_12);
            /* Disable DMA Channel */
            DMA_Cmd(I2C1_DMA_CHANNEL_RX, DISABLE);
            /* Clear the DMA Transfer Complete flag */
            DMA_ClearFlag(DMA1_FLAG_TC7);

        }
        else
        {
            be_result = BOARD_ERR_ERROR;
        }

        /* Program the STOP */
        I2Cx->CR1 |= CR1_STOP_Set;
        /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
        while ((I2Cx->CR1&0x200U) == 0x200U)
        {}
#endif
    }
    else
    {
        be_result = BOARD_ERR_ERROR;
    }

    return (be_result);
}



/**
  * @brief  Writes buffer of bytes.
  * @param pBuffer: Buffer of bytes to be sent to the slave.
  * @param NumByteToWrite: Number of bytes to be sent by the Master.
  * @param Mode: Polling or DMA or Interrupt having the highest priority in the application.
  * @param SlaveAddress: The address of the slave to be addressed by the Master.
  * @retval : None.
  */
static BOARD_ERROR be_board_i2c_master_buffer_write(I2C_TypeDef* I2Cx, uint8_t* pBuffer,  uint32_t NumByteToWrite, I2C_ProgrammingModel Mode, uint8_t SlaveAddress )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    __IO uint32_t temp = 0U;
    __IO uint32_t Timeout = 0U;

    /* Enable Error IT (used in all modes: DMA, Polling and Interrupts */
    I2Cx->CR2 |= I2C_IT_ERR;
    if (Mode == DMA)  /* I2Cx Master Transmission using DMA */
    {
        Timeout = 0xFFFFU;
        /* Configure the DMA channel for I2Cx transmission */
        board_i2c_dma_config (I2Cx, pBuffer, NumByteToWrite, I2C_DIRECTION_TX);
        /* Enable the I2Cx DMA requests */
        I2C_DMACmd(I2Cx, ENABLE);
        /* I2Cx->CR2 |= CR2_DMAEN_Set; */
        /* Send START condition */
        I2C_GenerateSTART(I2Cx, ENABLE);
        /*I2Cx->CR1 |= CR1_START_Set;*/
        /* Wait until SB flag is set: EV5 */
        while ((I2Cx->SR1&0x0001U) != 0x0001U)
        {
            if (Timeout-- == 0U)
            {
                /* return Error; */
                be_result = BOARD_ERR_ERROR;
                /* return (be_result); */
                break;
            }
        }
        Timeout = 0xFFFFU;
        /* Send slave address */
        /* Reset the address bit0 for write */
        SlaveAddress &= (uint8_t)OAR1_ADD0_Reset;
        Address = SlaveAddress;
        /* Send the slave address */
        I2Cx->DR = Address;
        /* Wait until ADDR is set: EV6 */
        while ((I2Cx->SR1&0x0002U) != 0x0002U)
        {
            if (Timeout-- == 0U)
                {
                    /* return Error; */
                    be_result = BOARD_ERR_ERROR;
                    /* return (be_result); */
                    break;
                }

        }

        /* Clear ADDR flag by reading SR2 register */
        temp = I2Cx->SR2;
        if (I2Cx == I2C1)
        {
            /* Wait until DMA end of transfer */
            while (!DMA_GetFlagStatus(DMA1_FLAG_TC6))
            {}

            /* Disable the DMA1 Channel 6 */
            DMA_Cmd(I2C1_DMA_CHANNEL_TX, DISABLE);
            /* Clear the DMA Transfer complete flag */
            DMA_ClearFlag(DMA1_FLAG_TC6);
        }
        else  /* I2Cx = I2C2 */
        {
            /* Wait until DMA end of transfer */
            while (!DMA_GetFlagStatus(DMA1_FLAG_TC4))
            {}
            /* Disable the DMA1 Channel 4 */
            DMA_Cmd(I2C2_DMA_CHANNEL_TX, DISABLE);
            /* Clear the DMA Transfer complete flag */
            DMA_ClearFlag(DMA1_FLAG_TC4);
        }

        /* EV8_2: Wait until BTF is set before programming the STOP */
        while ((I2Cx->SR1 & 0x00004U) != 0x000004U)
        {}
        /* Program the STOP */
        I2Cx->CR1 |= CR1_STOP_Set;
        /* Make sure that the STOP bit is cleared by Hardware */
        while ((I2Cx->CR1&0x200U) == 0x200U)
        {}
    }
    else
    {
        be_result = BOARD_ERR_ERROR;
    }
    return(be_result);
}


/**
* @brief  Initializes peripherals: I2Cx, GPIO, DMA channels .
  * @param  None
  * @retval None
  */
static void board_i2c_lowlevel_init(I2C_TypeDef* I2Cx)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    /* GPIOB clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* Enable the DMA1 clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    if (I2Cx == I2C1)
    {
        /* I2C1 clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
        /* I2C1 SDA and SCL configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* Enable I2C1 reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
        /* Release I2C1 from reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
    }
   /* I2C1 and I2C2 configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = OwnAddress1;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_InitStructure.I2C_OwnAddress1 = OwnAddress2;
    I2C_Init(I2C2, &I2C_InitStructure);

    if (I2Cx == I2C1)
    {   /* I2C1 TX DMA Channel configuration */
        DMA_DeInit(I2C1_DMA_CHANNEL_TX);
        I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
        I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;   /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_BufferSize = 0xFFFFU;            /* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        I2CDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        I2CDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
        I2CDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        I2CDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        I2CDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        I2CDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(I2C1_DMA_CHANNEL_TX, &I2CDMA_InitStructure);

        /* I2C1 RX DMA Channel configuration */
        DMA_DeInit(I2C1_DMA_CHANNEL_RX);
        DMA_Init(I2C1_DMA_CHANNEL_RX, &I2CDMA_InitStructure);

    #if 0 /* For I2C TX DMA transfer complete interrupt disabled. Sending of 1 byte take 200uS. */
    {
        NVIC_InitTypeDef  NVIC_InitStructure;
        /* Setup DMA end of transfer interrupt. */
        NVIC_InitStructure.NVIC_IRQChannel = (unsigned char)DMA1_Channel6_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_Channel6_PRIORITY_GROUP;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_Channel6_SUB_PRIORITY_GROUP;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);
    }
    #endif

    #if 1 /* For I2C RX DMA transfer complete interrupt disabled. Sending of 1 byte take 200uS. */
    {
        NVIC_InitTypeDef  NVIC_InitStructure;
        /* Setup DMA end of transfer interrupt. */
        NVIC_InitStructure.NVIC_IRQChannel = (unsigned char)DMA1_Channel7_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_Channel7_PRIORITY_GROUP;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_Channel7_SUB_PRIORITY_GROUP;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
    }
    #endif
    }
}

/**
  * @brief  Initializes DMA channel used by the I2C Write/read routines.
  * @param  None.
  * @retval None. for I2C1 only
  */
static void board_i2c_dma_config(I2C_TypeDef* I2Cx, uint8_t* pBuffer, uint32_t BufferSize, uint32_t Direction)
{
    /* Initialize the DMA with the new parameters */
    if (Direction == I2C_DIRECTION_TX)
    {
        /* Configure the DMA Tx Channel with the buffer address and the buffer size */
        I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
        I2CDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        I2CDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;

        if (I2Cx == I2C1)
        {
            I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
            DMA_Cmd(I2C1_DMA_CHANNEL_TX, DISABLE);
            DMA_Init(I2C1_DMA_CHANNEL_TX, &I2CDMA_InitStructure);
            DMA_Cmd(I2C1_DMA_CHANNEL_TX, ENABLE);
        }
    }
    else /* Reception */
    {
        /* Configure the DMA Rx Channel with the buffer address and the buffer size */
        I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
        I2CDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        I2CDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;
        if (I2Cx == I2C1)
        {

            I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
            DMA_Cmd(I2C1_DMA_CHANNEL_RX, DISABLE);
            DMA_Init(I2C1_DMA_CHANNEL_RX, &I2CDMA_InitStructure);
            DMA_Cmd(I2C1_DMA_CHANNEL_RX, ENABLE);
        }
    }
}

/*DMA_CHANNEL_TX */
/*#define  DMA_ISR_TCIF6                       ((uint32_t)0x00200000) */       /*!< Channel 6 Transfer Complete flag */

void DMA1_Channel6_IRQHandler(void)
{
    u16_I2C_DMA_TC_flag = 1U;
    /* Restart DMA chanel for new transaction. */
    /*DMA1_Channel6->CCR  &= (uint16_t)(~DMA_CCR1_EN);*/
    /*DMA1_Channel6->CNDTR = 0x01U;*/ /* u16_counter; */
    /*DMA1_Channel6->CCR  |= DMA_CCR1_EN; */

    /* clear flag "end_of_TX through DMA channel". */
    /* DMA_ClearFlag(DMA1_FLAG_TC6); */
    DMA1->IFCR |= DMA_ISR_TCIF6;
}


/*DMA_CHANNEL_RX */

void DMA1_Channel7_IRQHandler(void)
{

	if (DMA_GetFlagStatus(DMA1_FLAG_TC7))
	{
		if (I2C1->SR2 & 0x01U) /* master receive DMA finish */
		{
            board_gyro_data.u16_X = (((uint16_t)  u8_rc_dma_buffer[0]) << 8U) + ((uint16_t)u8_rc_dma_buffer[1]);
            board_gyro_data.u16_Y = (((uint16_t)  u8_rc_dma_buffer[2]) << 8U) + ((uint16_t)u8_rc_dma_buffer[3]);
            board_gyro_data.u16_Z = (((uint16_t)  u8_rc_dma_buffer[4]) << 8U) + ((uint16_t)u8_rc_dma_buffer[5]);

			I2C_DMACmd(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
			i2c_comm_state = COMM_DONE;
			MasterReceptionComplete = 1U;
			PV_flag_1 =0U;
            GPIO_ResetBits( GPIOB, GPIO_Pin_12);
		}
		else /* slave receive DMA finish */
		{

		}
		DMA_ClearFlag(DMA1_FLAG_TC7);
	}
	if (DMA_GetFlagStatus(DMA1_FLAG_GL7))
	{
		DMA_ClearFlag( DMA1_FLAG_GL7);
	}
	if (DMA_GetFlagStatus(DMA1_FLAG_HT7))
	{
		DMA_ClearFlag( DMA1_FLAG_HT7);
	}

#if 0
    /* Restart DMA chanel for new transaction. */
    DMA_Cmd(I2C1_DMA_CHANNEL_RX, DISABLE);

    /* Program the STOP */
    I2C1->CR1 |= CR1_STOP_Set;
    /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
    while ((I2C1->CR1&0x200U) == 0x200U)
    {}
    /* Set user flag DMA RX complete. */
    u16_I2C_DMA_TC_flag = 1U;
    /* Clear the DMA Transfer Complete flag */
    DMA_ClearFlag(DMA1_FLAG_TC7);
#endif

}

void I2C1_EV_IRQHandler(void)
{
	uint32_t lastevent= I2C_GetLastEvent(I2C1);

	switch (lastevent)
	{
		/************************** Master Invoke**************************************/
		case I2C_EVENT_MASTER_MODE_SELECT: /* EV5 */
				/* MSL SB BUSY 30001 */
          		if(!check_begin)
				{
					i2c_comm_state = COMM_IN_PROCESS;
				}

                if (MasterDirection == Receiver)
				{
					I2C_Send7bitAddress(I2C1, (u8)SlaveADDR, I2C_Direction_Receiver);
				}
				else
				{
				/* Send slave Address for write */
					I2C_Send7bitAddress(I2C1, (u8)SlaveADDR, I2C_Direction_Transmitter);
				}
				I2C_ITConfig(I2C1, I2C_IT_BUF , ENABLE);/*also TxE int allowed */
             break;
        /********************** Master Receiver events ********************************/
		case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED: /* EV6 */
				/* MSL BUSY ADDR 0x30002 */
				I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);

				/*without it, no NAK signal on bus after last Data */
				/*I2C data line would be hold low  */
				I2C_DMALastTransferCmd(I2C1, ENABLE);

				I2C_DMACmd(I2C1, ENABLE);
				DMA_Cmd(DMA1_Channel7, ENABLE);
				break;
        default:
            break;
    }


}

void I2C1_ER_IRQHandler(void)
{
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_AF))
	{
		if(check_begin)
		{
			I2C_GenerateSTART(I2C1, ENABLE);
		}
		else if(I2C1->SR2 & 0x01U)
		{
			/*!! receive over */
			I2C_GenerateSTOP(I2C1, ENABLE);
			i2c_comm_state = COMM_EXIT;
			PV_flag_1 = 0U;
		}
	I2C_ClearFlag(I2C1, I2C_FLAG_AF);
	}

	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BERR))
	{
		if (I2C1->SR2 & 0x01U)
		{
			I2C_GenerateSTOP(I2C1, ENABLE);
			i2c_comm_state = COMM_EXIT;
			PV_flag_1 = 0U;
		}
		I2C_ClearFlag(I2C1, I2C_FLAG_BERR);
	}
}