


#include "board_i2c.h"

/* Common variables for setup interrupt and DMA modules. */
static  NVIC_InitTypeDef	NVIC_InitStructure;
static  DMA_InitTypeDef  	DMA_I2C_InitStructure;

/* Used in I2C1_EV_IRQHandler to detect direction of transmittion for correction R/W bit of address. */
static vu8 vu8_master_direction = Transmitter;

/* Address of slave device, used in I2C1_EV_IRQHandler. */
static uint8_t u8_slave_addr;

/* Masters receive process state */
static vu8 vu8_master_reception_complete;

/* Masters send process state */
static vu8 vu8_master_transition_complete;

/*Prevent miss operation on I2C1 */
static vu8 PV_flag_1;

/* DMA use buffer to storage data from slave device. */
static uint8_t u8_rc_dma_buffer[6];

/* Flag of one time operation. Should be 0 for multitime functions. */
static uint8_t u8_one_time_rw = 0U;

/* Global.Used for one time sensor data reading. */
BOARD_U16_3X_DATA board_i2c_sensor_data;


/* Initialization of I2C1 module. */
BOARD_ERROR be_board_i2c_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Restart stress for I2C1 slave device. */
    board_i2c_unstick();
    board_i2c_lowlevel_init();

    /* It using for I2C test. */
    be_result = be_board_gyro_detect();

    return(be_result);
}


/**/
BOARD_ERROR be_board_i2c_write_start(uint8_t* pu8_buffer, uint16_t u16_num_byte_to_write, uint8_t u8_device_address)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    u8_one_time_rw = 0U;/* Should be zero. */
    be_result = be_board_i2c_DMA_master_buffer_write(pu8_buffer, u16_num_byte_to_write, u8_device_address);
    return(be_result);
}
/**/
BOARD_ERROR be_board_i2c_read_start(uint8_t* pu8_buffer, uint16_t u16_number_byte_to_read, uint8_t u8_device_address)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    u8_one_time_rw = 0U;/* Should be zero. */
    be_result = be_board_i2c_master_buffer_DMA_read (pu8_buffer, u16_number_byte_to_read, u8_device_address);
    return(be_result);
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

    u8_write_buffer[0U] = u8_write_address; /* Address of device register. */
    u8_write_buffer[1U] = u8_write_data;    /* Data for writing to device register. */

    /* Use one time read ISR. */
    u8_one_time_rw = 1U;
    be_result = be_board_i2c_DMA_master_buffer_write(u8_write_buffer, 2U, u8_device_address);
    while(vu8_master_transition_complete == 0U)
    {}
    return(be_result);
}

/* This function read data from slave device using DMA read function + interrupt. */
BOARD_ERROR board_i2c_read(
                            uint8_t  u8_device_address,
                            uint8_t  u8_start_read_address,
                            uint16_t u16_number_byte_to_read,
                            uint8_t* pu8_pointer_to_buffer  /* pointer to bytes */
                          )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Write read address for reading datas. */
    /* be_result  = be_board_i2c_master_buffer_write(I2C1, &u8_start_read_address,1U,I2C_MODULE_MODE, u8_device_address);*/
    /* Use one time read ISR. */
    u8_one_time_rw = 1U;
    be_result = be_board_i2c_DMA_master_buffer_write(&u8_start_read_address, 1U, u8_device_address);
    while(vu8_master_transition_complete == 0U)
    {}
    u8_one_time_rw = 1U;
    be_result |= be_board_i2c_master_buffer_DMA_read (pu8_pointer_to_buffer, u16_number_byte_to_read, u8_device_address);
    while(vu8_master_reception_complete == 0U)
    {}

    return(be_result);
}

/* Read slave using DMA and interrupt. */
static BOARD_ERROR be_board_i2c_master_buffer_DMA_read(
                                                        uint8_t* pBuffer,
                                                        uint16_t NumByteToRead,
                                                        uint8_t SlaveAddress
                                                      )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

	/* PV operation */
	if (PV_flag_1 == 0U)
	{
        PV_flag_1 = 1U;

        /* Configure I2Cx DMA channel */
        DMA_DeInit(DMA1_Channel7);
        DMA_I2C_InitStructure.DMA_PeripheralBaseAddr = (u32)I2C1_DR_Address;

        /* DMA_I2C_InitStructure.DMA_MemoryBaseAddr 	= (u32)u8_rc_dma_buffer; */	/* fixed local buffer for test. */
        DMA_I2C_InitStructure.DMA_MemoryBaseAddr 	= (u32)pBuffer;                 /* pointer to output buffer. */

        DMA_I2C_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralSRC;        /* fixed for receive function */
        DMA_I2C_InitStructure.DMA_BufferSize            = (uint32_t)NumByteToRead;      /* number byte for read. */
        DMA_I2C_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;    /* fixed */
        DMA_I2C_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;         /* fixed */
        DMA_I2C_InitStructure.DMA_PeripheralDataSize    = DMA_MemoryDataSize_Byte;      /*fixed */
        DMA_I2C_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;      /*fixed */
        DMA_I2C_InitStructure.DMA_Mode                  = DMA_Mode_Normal;              /* fixed */
        DMA_I2C_InitStructure.DMA_Priority              = DMA_Priority_VeryHigh;        /* up to user */
        DMA_I2C_InitStructure.DMA_M2M                   = DMA_M2M_Disable;              /* fixed */

        DMA_Init(DMA1_Channel7, &DMA_I2C_InitStructure);
        DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);

        NVIC_InitStructure.NVIC_IRQChannel = (unsigned)DMA1_Channel7_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_Channel7_PRIORITY_GROUP;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_Channel7_SUB_PRIORITY_GROUP;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        /*initialize static parameter*/
        vu8_master_direction = Receiver;
        vu8_master_reception_complete = 0U;

        /*initialize static parameter according to input parameter*/
        u8_slave_addr = SlaveAddress;

        I2C_AcknowledgeConfig(I2C1, ENABLE);
        I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_ERR , ENABLE);/*only SB int allowed */
        /* Send START condition */
        if(I2C1->CR1 & 0x200U)
        {
            I2C1->CR1 &= 0xFDFFU;
        }
        I2C_GenerateSTART(I2C1, ENABLE);
    }
    else
    {
        be_result = BOARD_ERR_BUSY;
    }
    return (be_result);
}





/**
  * Writes buffer of bytes.
  */
static BOARD_ERROR be_board_i2c_DMA_master_buffer_write(uint8_t* pBuffer, uint16_t NumByteToWrite, uint8_t SlaveAddress)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;


    /* I2C1 TX DMA Channel configuration */
    DMA_DeInit(DMA1_Channel6);

    DMA_I2C_InitStructure.DMA_PeripheralBaseAddr        = (uint32_t)I2C1_DR_Address;
    DMA_I2C_InitStructure.DMA_MemoryBaseAddr            = (uint32_t)pBuffer;            /* This parameter will be configured durig communication */
    DMA_I2C_InitStructure.DMA_DIR                       = DMA_DIR_PeripheralDST;        /* This parameter will be configured durig communication */
    DMA_I2C_InitStructure.DMA_BufferSize                = NumByteToWrite;               /* This parameter will be configured durig communication */
    DMA_I2C_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;
    DMA_I2C_InitStructure.DMA_MemoryInc                 = DMA_MemoryInc_Enable;
    DMA_I2C_InitStructure.DMA_PeripheralDataSize        = DMA_MemoryDataSize_Byte;
    DMA_I2C_InitStructure.DMA_MemoryDataSize 	        = DMA_MemoryDataSize_Byte;
    DMA_I2C_InitStructure.DMA_Mode                      = DMA_Mode_Normal;
    DMA_I2C_InitStructure.DMA_Priority                  = DMA_Priority_VeryHigh;
    DMA_I2C_InitStructure.DMA_M2M                       = DMA_M2M_Disable;

    DMA_Init(DMA1_Channel6, &DMA_I2C_InitStructure);
    DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel                  = (unsigned)DMA1_Channel6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= DMA1_Channel6_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority       = DMA1_Channel6_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd               = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*initialize static parameter*/
    vu8_master_direction = Transmitter;
    vu8_master_transition_complete = 0U;

    /*initialize static parameter according to input parameter*/
    u8_slave_addr = SlaveAddress;

    I2C_AcknowledgeConfig(I2C1, ENABLE);
    I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, ENABLE);

    /* Send START condition */
    if(I2C1->CR1 & 0x200U)
    {
        I2C1->CR1 &= 0xFDFFU;
    }
    I2C_GenerateSTART(I2C1, ENABLE);

    return(be_result);
}

/**
* @brief  Initializes peripherals: I2Cx, GPIO, DMA channels .
  * @param  None
  * @retval None
  */
static void board_i2c_lowlevel_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    /* GPIOB clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* I2C1 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    /* I2C1 SDA and SCL configuration */
    GPIO_InitStructure.GPIO_Pin         = GPIO_Pin_6 | GPIO_Pin_7;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    I2C_DeInit(I2C1);

    GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* I2C1 and I2C2 configuration */
    I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode                  = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle             = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1           = OwnAddress1;
    I2C_InitStructure.I2C_Ack                   = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress 	= I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed            = ClockSpeed;
    I2C_Cmd(I2C1, ENABLE);
    I2C_Init(I2C1, &I2C_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel                      = (unsigned)I2C1_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = I2C1_EV_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = I2C1_EV_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel                      = (unsigned)I2C1_ER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = I2C1_ER_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = I2C1_ER_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the DMA1 clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

/* DMA I2C1 DMA_CHANNEL_6 interrupt. End of transmission data to slave. */
void DMA1_Channel6_IRQHandler(void)
{
    if (DMA_GetFlagStatus(DMA1_FLAG_TC6))
    {
        /* This function for case of one time write. */
        v_dma_ch6_one_time_write();
        if(u8_one_time_rw == 0U)
        {
            /* start read function. */
            be_api_i2c_read_process_start();
        }
        DMA_ClearFlag(DMA1_FLAG_TC6);
    }

    if(DMA_GetFlagStatus(DMA1_FLAG_GL6))
    {
	DMA_ClearFlag( DMA1_FLAG_GL6);
    }

    if(DMA_GetFlagStatus(DMA1_FLAG_HT6))
    {
	DMA_ClearFlag( DMA1_FLAG_HT6);
    }
}

static void v_dma_ch6_one_time_write(void)
{
    if (I2C1->SR2 & 0x01U) /* master send DMA finish, check process later */
    {
	/* DMA1-6 (I2C1 Tx DMA)transfer complete ISR */
	I2C_DMACmd(I2C1, DISABLE);
	DMA_Cmd(DMA1_Channel6, DISABLE);

        /* wait until BTF */
	while (!(I2C1->SR1 & 0x04U))
	{}
	I2C_GenerateSTOP(I2C1, ENABLE);

        /* wait until BUSY clear */
	while (I2C1->SR2 & 0x02U)
	{}
	vu8_master_transition_complete = 1U;
    }
    else /* slave send DMA finish */
    {

    }
}



/* DMA I2C1 DMA_CHANNEL_7 interrupt. End of receive data from slave. */
void DMA1_Channel7_IRQHandler(void)
{
    if (DMA_GetFlagStatus(DMA1_FLAG_TC7))
    {
        /* This function for case of one time read */

        v_dma_ch7_one_time_read();
        if(u8_one_time_rw == 0U)
        {
            /* TODO: should be optimised. */
/*
            api_i2c_data.array[api_i2c_data.u8_device].data[0]= u8_rc_dma_buffer[0];
            api_i2c_data.array[api_i2c_data.u8_device].data[1]= u8_rc_dma_buffer[1];
            api_i2c_data.array[api_i2c_data.u8_device].data[2]= u8_rc_dma_buffer[2];
            api_i2c_data.array[api_i2c_data.u8_device].data[3]= u8_rc_dma_buffer[3];
            api_i2c_data.array[api_i2c_data.u8_device].data[4]= u8_rc_dma_buffer[4];
            api_i2c_data.array[api_i2c_data.u8_device].data[5]= u8_rc_dma_buffer[5];
*/
            api_i2c_data.u8_device++;
            if(api_i2c_data.u8_device < MAX_DEV_NUM) /* Maximum device number = 3. */
            {
                be_api_i2c_write_process_start();
            }
            else
            {
                api_i2c_data.u8_ready = 1U;
                GPIO_ResetBits( GPIOB, GPIO_Pin_12);/* for test only */
            }
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
}

static void v_dma_ch7_one_time_read(void)
{
    if (I2C1->SR2 & 0x01U) /* master receive DMA finish */
    {
        /*TODO: Should be optimised. */
        /* Case of one time sansor data reading. */
        board_i2c_sensor_data.u16_X = (((uint16_t)  u8_rc_dma_buffer[0]) << 8U) + ((uint16_t)u8_rc_dma_buffer[1]);
        board_i2c_sensor_data.u16_Y = (((uint16_t)  u8_rc_dma_buffer[2]) << 8U) + ((uint16_t)u8_rc_dma_buffer[3]);
        board_i2c_sensor_data.u16_Z = (((uint16_t)  u8_rc_dma_buffer[4]) << 8U) + ((uint16_t)u8_rc_dma_buffer[5]);

        /* Disable DMA. */
        I2C_DMACmd(I2C1, DISABLE);

        /* Stop I2C communuation. */
	I2C_GenerateSTOP(I2C1, ENABLE);

        /* Set data read ready flag. */
	vu8_master_reception_complete = 1U;
        PV_flag_1 = 0U;
    }
}

/* I2C event interrupt. Used for I2C DMA reading. */
void I2C1_EV_IRQHandler(void)
{
    uint32_t lastevent = I2C_GetLastEvent(I2C1);

    switch (lastevent)
    {
        /* Master send start condition and now should send slave address. */
        case I2C_EVENT_MASTER_MODE_SELECT: /* EV5 (0x00030001)*/
            if (vu8_master_direction == Receiver)
            {   /* Master sending address for reading. Master will read data from slave. */
                I2C_Send7bitAddress(I2C1, u8_slave_addr, I2C_Direction_Receiver);
            }
            else
            {
                /* Master sending slave address for write, master will write to salve. */
                I2C_Send7bitAddress(I2C1, u8_slave_addr, I2C_Direction_Transmitter);
            }
            I2C_ITConfig(I2C1, I2C_IT_BUF , ENABLE);	/*also TxE int allowed */
            /* I2C_IT_BUF Buffer interrupt mask, allow us to have an EV7,byte received. */
            /* I2C_IT_EVT Event interrupt enabled */
            break;
        /* Master Receiver events */
        case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED: /* EV6 */
            /*Disable I2C interrupt and enable DMA module. */
            I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
            /*without it, no NAK signal on bus after last Data */
            /*I2C data line would be hold low  */
            I2C_DMALastTransferCmd(I2C1, ENABLE);
            I2C_DMACmd(I2C1, ENABLE);
            DMA_Cmd(DMA1_Channel7, ENABLE);
            break;

        case I2C_EVENT_MASTER_BYTE_RECEIVED: /* EV7 */
			/* MSL BUSY RXNE 0x30040 */
            break;

        /* Master Transmitter events */
	case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: /* EV8 just after EV6 */
            /* BUSY, MSL, ADDR, TXE and TRA 0x70082 */

            I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
            I2C_DMACmd(I2C1, ENABLE);
            DMA_Cmd(DMA1_Channel6, ENABLE);
            break;

        case I2C_EVENT_M_BYTE_TRANSMITTING: /* EV8 I2C_EVENT_MASTER_BYTE_TRANSMITTING*/
            break;

        case I2C_EVENT_M_BYTE_TRANSMITTED: /* EV8-2 */
            /*TRA, BUSY, MSL, TXE and BTF 0x70084 */
            break;

        default:
            break;
    }
}

/* This function error interrupt for I2C. */
void I2C1_ER_IRQHandler(void)
{
    if(I2C_GetFlagStatus(I2C1, I2C_FLAG_AF))
    {
        if(I2C1->SR2 & 0x01U)
	{
            /*!! receive over */
            I2C_GenerateSTOP(I2C1, ENABLE);
            PV_flag_1 = 0U;
	}
        I2C_ClearFlag(I2C1, I2C_FLAG_AF);
	}

	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BERR))
	{
		if (I2C1->SR2 & 0x01U)
		{
			I2C_GenerateSTOP(I2C1, ENABLE);
			PV_flag_1 = 0U;
		}
		I2C_ClearFlag(I2C1, I2C_FLAG_BERR);
	}
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

