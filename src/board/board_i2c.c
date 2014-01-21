


#include "board_i2c.h"

#define DMA_ENABLE 1U
static  NVIC_InitTypeDef	NVIC_InitStructure;
static  DMA_InitTypeDef  	I2CDMA_InitStructure;
static volatile uint32_t I2CDirection = I2C_DIRECTION_TX;
static volatile uint32_t NumbOfBytes1;
static volatile uint32_t NumbOfBytes2;
static volatile uint8_t  Address;

static volatile uint16_t u16_I2C_DMA_TC_flag = 0U;



/* Local variables -------------------------------------------------------*/
static vu8 MasterDirection = Transmitter;
static u16 SlaveADDR;
static u16 check_begin = FALSE;


static volatile uint8_t tmp_del = 1U;

volatile uint8_t MasterReceptionComplete = 0U;

static volatile uint8_t MasterTransitionComplete = 0U; /* to indicat master's send process */
volatile uint8_t WriteComplete = 0U; /* to indicat target's internal write process */

int I2C_NumByteToWrite = 0U;
u8 I2C_NumByteWritingNow = 0U;
u8* I2C_pBuffer = 0U;
u16 I2C_WriteAddr = 0U;
/*P-V operation on I2C1 */
static vu8 PV_flag_1;

static volatile I2C_STATE i2c_comm_state;

/* DMA use buffer to storage data from slave device. */
static uint8_t u8_rc_dma_buffer[6];

/** Log */
static uint32_t u32_buf[100];
static uint32_t u32_buf_counter = 0U;
static uint32_t overflow = 0U;
/****/


BOARD_3X_DATA u16_3DX_DMA_data;


/* Initialization of I2C1 module. */
BOARD_ERROR be_board_i2c_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

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
    board_i2c_lowlevel_init();

    /* It using for I2C test. */
    be_result = be_board_gyro_detect();

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

    u8_write_buffer[0U] = u8_write_address;
    u8_write_buffer[1U] = u8_write_data;
    /* be_result = be_board_i2c_master_buffer_write(I2C1, u8_write_buffer,2U,I2C_MODULE_MODE, u8_device_address); */

    be_result = be_board_i2c_DMA_master_buffer_write(u8_write_buffer, 2U, u8_device_address);
    while(MasterTransitionComplete == 0U)
    {}
    return(be_result);
}

/* This function read data from slave device using DMA read function + interrupt. */
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
    /* be_result  = be_board_i2c_master_buffer_write(I2C1, &u8_start_read_address,1U,I2C_MODULE_MODE, u8_device_address);*/
    be_result = be_board_i2c_DMA_master_buffer_write(&u8_start_read_address, 1U, u8_device_address);
    while(MasterTransitionComplete == 0U)
    {}
    /* Read MSB and LSB from address 0xF6. */
    be_result |= be_board_i2c_master_buffer_DMA_read (pu8_pointer_to_buffer, u32_number_byte_to_read, u8_device_address);
    while(MasterReceptionComplete == 0U)
    {}

    return(be_result);
}

/* Read slave using DMA and interrupt. */
static BOARD_ERROR be_board_i2c_master_buffer_DMA_read(
                                                        uint8_t* pBuffer,
                                                        uint32_t NumByteToRead,
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
        DMA_DeInit(DMA1_Channel7);
        I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (u32)I2C1_DR_Address;

        I2CDMA_InitStructure.DMA_MemoryBaseAddr 	= (u32)u8_rc_dma_buffer; 		/* fixed local buffer for test. */
        I2CDMA_InitStructure.DMA_DIR 				= DMA_DIR_PeripheralSRC; 		/* fixed for receive function */
        I2CDMA_InitStructure.DMA_BufferSize 		= (uint32_t)NumByteToRead; 		/* number byte for read. */
        I2CDMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;	/* fixed */
        I2CDMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable; 		/* fixed */
        I2CDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte; 		/*fixed */
        I2CDMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte; 		/*fixed */
        I2CDMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal; 				/* fixed */
        I2CDMA_InitStructure.DMA_Priority 			= DMA_Priority_VeryHigh; 		/* up to user */
        I2CDMA_InitStructure.DMA_M2M 				= DMA_M2M_Disable; 				/* fixed */

        DMA_Init(DMA1_Channel7, &I2CDMA_InitStructure);
        DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);

        NVIC_InitStructure.NVIC_IRQChannel = (unsigned)DMA1_Channel7_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0U;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0U;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        /*initialize static parameter*/
        MasterDirection = Receiver;
        MasterReceptionComplete = 0U;

        /*initialize static parameter according to input parameter*/
        SlaveADDR = SlaveAddress;

        /* global state variable i2c_comm_state */
        i2c_comm_state = COMM_PRE;

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
static BOARD_ERROR be_board_i2c_DMA_master_buffer_write(uint8_t* pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAddress)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;


	    /* I2C1 TX DMA Channel configuration */
		DMA_DeInit(DMA1_Channel6);

		I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
		I2CDMA_InitStructure.DMA_MemoryBaseAddr 	= (uint32_t)pBuffer;   		/* This parameter will be configured durig communication */
        I2CDMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;    /* This parameter will be configured durig communication */
		I2CDMA_InitStructure.DMA_BufferSize 		= NumByteToWrite;           /* This parameter will be configured durig communication */
		I2CDMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;
		I2CDMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable;
		I2CDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
		I2CDMA_InitStructure.DMA_MemoryDataSize 	= DMA_MemoryDataSize_Byte;
		I2CDMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;
		I2CDMA_InitStructure.DMA_Priority 			= DMA_Priority_VeryHigh;
		I2CDMA_InitStructure.DMA_M2M 				= DMA_M2M_Disable;

	    DMA_Init(DMA1_Channel6, &I2CDMA_InitStructure);
	    DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);

		NVIC_InitStructure.NVIC_IRQChannel = (unsigned)DMA1_Channel6_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0U;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0U;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

        /*initialize static parameter*/
        MasterDirection = Transmitter;
        MasterTransitionComplete = 0U;

        /*initialize static parameter according to input parameter*/
        SlaveADDR = SlaveAddress;

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
    GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_6 | GPIO_Pin_7;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	I2C_DeInit(I2C1);

    GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* I2C1 and I2C2 configuration */
	I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode 					= I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle 			= I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 			= OwnAddress1;
    I2C_InitStructure.I2C_Ack 					= I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress 	= I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed 			= ClockSpeed;
	I2C_Cmd(I2C1, ENABLE);
    I2C_Init(I2C1, &I2C_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = (unsigned)I2C1_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0U;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1U;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = (unsigned)I2C1_ER_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0U;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0U;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    /* Enable the DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

/* DMA I2C1 DMA_CHANNEL_6 interrupt. End of transmission data to slave. */
void DMA1_Channel6_IRQHandler(void)
{
	if (DMA_GetFlagStatus(DMA1_FLAG_TC6))
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
			MasterTransitionComplete = 1U;
			/**????*/
#if 0
            i2c_comm_state = COMM_IN_PROCESS;
			I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE); /* use interrupt to handle check process */
			check_begin = TRUE;
			if(I2C1->CR1 & 0x200U)
			{
				I2C1->CR1 &= 0xFDFFU;
			}
			I2C_GenerateSTART(I2C1, ENABLE);
#endif
		}
		else /* slave send DMA finish */
		{

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

/* DMA I2C1 DMA_CHANNEL_7 interrupt. End of receive data from slave. */
void DMA1_Channel7_IRQHandler(void)
{
	if (DMA_GetFlagStatus(DMA1_FLAG_TC7))
	{
		if (I2C1->SR2 & 0x01U) /* master receive DMA finish */
		{
            /* Case: of Gyro data reading. */
            board_gyro_data.u16_X = (((uint16_t)  u8_rc_dma_buffer[0]) << 8U) + ((uint16_t)u8_rc_dma_buffer[1]);
            board_gyro_data.u16_Y = (((uint16_t)  u8_rc_dma_buffer[2]) << 8U) + ((uint16_t)u8_rc_dma_buffer[3]);
            board_gyro_data.u16_Z = (((uint16_t)  u8_rc_dma_buffer[4]) << 8U) + ((uint16_t)u8_rc_dma_buffer[5]);
            /* Case: gyro dev id. */
            u16_3DX_DMA_data.u16_X = u8_rc_dma_buffer[0];
            /* Disable DMA. */
            I2C_DMACmd(I2C1, DISABLE);
            /* Stop I2C communuation. */
			I2C_GenerateSTOP(I2C1, ENABLE);
            i2c_comm_state = COMM_DONE;
            /* Set data read ready flag. */
			MasterReceptionComplete = 1U;
            PV_flag_1 = 0U;
            GPIO_ResetBits( GPIOB, GPIO_Pin_12);
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

/* I2C event interrupt. Used for I2C DMA reading. */
void I2C1_EV_IRQHandler(void)
{
    uint32_t lastevent = I2C_GetLastEvent(I2C1);

    v_data_add(lastevent);

    switch (lastevent)
    {
        /* Master send start condition and now should send slave address. */
        case I2C_EVENT_MASTER_MODE_SELECT: /* EV5 (0x00030001)*/
            if(!check_begin)/* It will be used for transmit. */
            {
                i2c_comm_state = COMM_IN_PROCESS;
            }
            if (MasterDirection == Receiver)
            {   /* Master sending address for reading. Master will read data from slave. */
                I2C_Send7bitAddress(I2C1, (u8)SlaveADDR, I2C_Direction_Receiver);
            }
            else
            {
                /* Master sending slave address for write, master will write to salve. */
                I2C_Send7bitAddress(I2C1, (u8)SlaveADDR, I2C_Direction_Transmitter);
            }
            I2C_ITConfig(I2C1, I2C_IT_BUF , ENABLE);	/*also TxE int allowed */
            /* I2C_IT_BUF Buffer interrupt mask, allow us to have an EV7,byte received. */
            /* I2C_IT_EVT Event interrupt enabled */
            break;
        /********************** Master Receiver events ********************************/
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

        /************************* Master Transmitter events **************************/
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


static void v_data_add(uint32_t u32_data)
{


    u32_buf[u32_buf_counter] = u32_data ;

    u32_buf_counter++;
    if(u32_buf_counter>=100U)
    {
        u32_buf_counter=0U;
        overflow++;
    }
}


