//=====================================================================
//文件名称：i2c.c
//功能概要：i2c底层驱动构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2013-05-4   V2.1
//        2015-01-21   V3.0
//=====================================================================
#include  "i2c.h"

static const I2C_MemMapPtr I2C_BASE_TABLE[2] = I2C_BASE_PTRS;
static const IRQn_Type I2C_IRQ_TABLE[2] = {I2C0_IRQn,I2C1_IRQn};


//****************************内部调用函数*****************************
//=====================================================================
//函数名称：wait
//功能概要：等待完成
//参数说明：I2C_No:模块号,其取值为0,1
//函数返回：1：表示超时，0：表示正常
//=====================================================================
static uint_8 i2c_wait(uint_8 I2C_No)
{
	uint_16 time=0;
	uint_16 time_out = 0xfffe;
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	while(BGET(I2C_S_IICIF_SHIFT,i2c_ptr->S) == 0)
	{
		time++;
		if(time > time_out)
			return 1;
	}
	BSET(I2C_S_IICIF_SHIFT,i2c_ptr->S);
	return 0;
}

//=====================================================================
//函数名称：i2c_read
//功能概要：读数据
//参数说明：I2C_No:模块号,其取值为0,1
//函数返回：寄存器中的数据
//=====================================================================
static uint_8 i2c_read(uint_8 I2C_No)
{
	uint_8 result;
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	result = i2c_ptr->D;
	return result;
}

//=====================================================================
//函数名称： i2c_write
//功能概要： 写数据
//参数说明： I2C_No:模块号,其取值为0,1
//          data：要写入的数据
//函数返回： 无
//=====================================================================
static void i2c_write(uint_8 I2C_No,uint_8 data)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	i2c_ptr->D = data;
}

//=====================================================================
//函数名称： i2c_stop
//功能概要： 停止信号
//参数说明： I2C_No:模块号,其取值为0,1
//函数返回： 无
//=====================================================================
static void i2c_stop(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	BCLR(I2C_C1_MST_SHIFT,i2c_ptr->C1); //将主机改成从机发送停止信号
	BCLR(I2C_C1_TX_SHIFT,i2c_ptr->C1);  //该位接收模式
}

//=====================================================================
//函数名称： i2c_start
//功能概要： 开始信号
//参数说明： I2C_No:模块号,其取值为0,1
//函数返回： 无
//=====================================================================
static void i2c_start(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	BSET(I2C_C1_TX_SHIFT,i2c_ptr->C1);  //该位接收模式
	BSET(I2C_C1_MST_SHIFT,i2c_ptr->C1); //将主机改成从机发送停止信号
}

//=====================================================================
//函数名称： i2c_start
//功能概要： 重新开始信号
//参数说明： I2C_No:模块号,其取值为0,1
//函数返回： 无
//=====================================================================
static void i2c_restart(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	BSET(I2C_C1_TX_SHIFT,i2c_ptr->C1);  //该位接收模式
	BSET(I2C_C1_RSTA_SHIFT,i2c_ptr->C1); //将主机改成从机发送停止信号
}

//=====================================================================
//函数名称： i2c_sendACK
//功能概要： 发送ACK
//参数说明： I2C_No:模块号,其取值为0,1
//函数返回：无
//=====================================================================
static void i2c_sendACK(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	BCLR(I2C_C1_TXAK_SHIFT,i2c_ptr->C1);
}

//=====================================================================
//函数名称： i2c_sendNACK
//功能概要： 发送NACK
//参数说明： I2C_No:模块号,其取值为0,1
//函数返回：无
//=====================================================================
static void i2c_sendNACK(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	BSET(I2C_C1_TXAK_SHIFT,i2c_ptr->C1);
}

//=====================================================================
//函数名称： i2c_changeDir
//功能概要： 改变方向。
//参数说明： I2C_No:模块号,其取值为0,1
//          direction: 方向 0是接收，1是发送
//函数返回： 无
//=====================================================================
static void i2c_changeDir(uint_8 I2C_No,uint_8 direction)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	i2c_ptr->C1 &= ~I2C_C1_TX_MASK;
	i2c_ptr->C1 |= I2C_C1_TX(direction);
}

//=====================================================================
//函数名称： i2c_get_ack
//功能概要： 获取应答或者非应答信号
//参数说明： I2C_No:模块号,其取值为0,1
//函数返回：0：应答信号；1：非应答信号
//=====================================================================
static uint_8 i2c_get_ack(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	uint_8 ack;
	ack = BGET(I2C_S_RXAK_SHIFT,i2c_ptr->S);
    return ack;
}

///********************对外接口函数*************************************
//=====================================================================
//函数名称：i2c_init                                                        
//功能概要：初始化IICX模块。默认为100K.
//参数说明：I2C_No:模块号,其取值为0,1
//          Mode 模式  1：主机     0：从机
//          address 本模块初始化地址    范围(1~255)
//          BaudRate为波特率，其单位为Kb/s，其取值为25,50,75,100K
//函数返回： 无
//=====================================================================
void I2C_init(uint_8 I2C_No,uint_8 Mode,uint_8 address,uint_8 BaudRate)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	if(I2C_No == IIC_0)
	{
		BSET(SIM_SCGC4_I2C0_SHIFT,SIM_SCGC4); //开时钟门
		#if (IIC_0_GROUP == 1)
		PORTE_PCR18 = PORT_PCR_MUX(0x4);      //使能IIC0_SDA
		PORTE_PCR19 = PORT_PCR_MUX(0x4);      //使能IIC0_SCL
		#endif
		#if (IIC_0_GROUP == 2)
		PORTE_PCR24 = PORT_PCR_MUX(0x5);      //使能IIC0_SCL
		PORTE_PCR25 = PORT_PCR_MUX(0x5);      //使能IIC0_SDA
		#endif

		#if (IIC_0_GROUP == 3)
		PORTB_PCR0 = PORT_PCR_MUX(0x2);      //使能IIC0_SCL
		PORTB_PCR1 = PORT_PCR_MUX(0x2);      //使能IIC0_SDA
		#endif

		#if (IIC_0_GROUP == 4)
		PORTB_PCR2 = PORT_PCR_MUX(0x2);      //使能IIC0_SCL
		PORTB_PCR3 = PORT_PCR_MUX(0x2);      //使能IIC0_SDA
		#endif

		#if (IIC_0_GROUP == 5)
		PORTC_PCR8 = PORT_PCR_MUX(0x2);      //使能IIC0_SCL
		PORTC_PCR9 = PORT_PCR_MUX(0x2);      //使能IIC0_SDA
		#endif
	}
	else
	{
		BSET(SIM_SCGC4_I2C1_SHIFT,SIM_SCGC4); //开时钟门
		#if (IIC_1_GROUP == 1)
		PORTE_PCR1 = PORT_PCR_MUX(0x6);      //使能IIC1_SCL
		PORTE_PCR0 = PORT_PCR_MUX(0x6);      //使能IIC1_SDA
		#endif

		#if (IIC_1_GROUP == 2)
		PORTA_PCR3 = PORT_PCR_MUX(0x2);      //使能IIC1_SCL
		PORTA_PCR4 = PORT_PCR_MUX(0x2);      //使能IIC1_SDA
		#endif

		#if (IIC_1_GROUP == 3)
		PORTC_PCR1 = PORT_PCR_MUX(0x2);      //使能IIC1_SCL
		PORTC_PCR2 = PORT_PCR_MUX(0x2);      //使能IIC1_SDA
		#endif

		#if (IIC_1_GROUP == 4)
		PORTC_PCR10 = PORT_PCR_MUX(0x2);      //使能IIC1_SCL
		PORTC_PCR11 = PORT_PCR_MUX(0x2);      //使能IIC1_SDA
		#endif
	}
	i2c_ptr->C1 = 0X00;   //初始化C1
	i2c_ptr->FLT |= I2C_FLT_STOPF_MASK;//清停止标志位
	i2c_ptr->S |= I2C_S_ARBL_MASK | I2C_S_IICIF_MASK; //初始化S
    if(1 == Mode)   //主机模式
    {
    	switch(BaudRate)
    	{
			case 25:
				i2c_ptr->F = 0x9f;
				break;
			case 50:
				i2c_ptr->F = 0x5f;
				break;
			case 75:
				i2c_ptr->F = 0x25;
				break;
			case 100:
				i2c_ptr->F = 0x1f;
				break;
			default:
				i2c_ptr->F = 0x1f;
				break;
    	}
        BSET(I2C_C1_IICEN_SHIFT,i2c_ptr->C1);//开i2c0模块使能
    }
    else            //从机模式
    {
    	i2c_ptr->A1 = address << 1;
        BSET(I2C_C1_IICEN_SHIFT,i2c_ptr->C1);//使能I2C模块
        BCLR(I2C_C1_TX_SHIFT,i2c_ptr->C1);//TX = 0,MCU设置为接收模式
        BCLR(I2C_C1_MST_SHIFT,i2c_ptr->C1);//MST位由1变成0，设置为从机模式
    }
}

//=====================================================================
//函数名称：  i2c_read1
//功能概要：  从从机读1个字节数据
//参数说明：  I2C_No:模块号，其取值为0，1
//           Data:带回收到的一个字节数据
//函数返回：  1：表示读取失败；0：读取成功
//函数说明:  该函数为对外函数，可以根据命令读取到相应的1个字节，若想读取2个
//           或以上字节数数据，请使用I2C_readN函数，不建议使用循环调用此函
//           数。
//=====================================================================
uint_8 I2C_read1(uint_8 I2C_No,uint_8 slaveAddress,uint_8 commond,\
		                                                  uint_8 *Data)
{      
	uint_8 temp;
	i2c_start(I2C_No);               //发送开始信号
	temp = (slaveAddress << 1) & 0xfe;
	i2c_write(I2C_No,temp);          //将地址和方向位发送给从机
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	if(i2c_get_ack(I2C_No) == 1)     //判断是否是ACK
		return 1;                    //若是非应答信号则返回失败
	i2c_write(I2C_No,commond);       //将命令发送给从机
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	i2c_restart(I2C_No);             //重新开始
	temp = (slaveAddress << 1) | 0x01;
	i2c_write(I2C_No,temp);  //将地址和方向位发送给从机
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	if(i2c_get_ack(I2C_No) == 1)     //判断是否是ACK
		return 1;                    //若是非应答信号则返回失败
	i2c_changeDir(I2C_No,0);         //改变方向
	i2c_sendNACK(I2C_No);            //发送NACK
	i2c_read(I2C_No);                //空读，触发下个数据传递
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	i2c_stop(I2C_No);                //发送停止信号
	*Data = i2c_read(I2C_No);  //读取数据，操作完成，可以在后面加少量延时
	return 0;
}

//=====================================================================
//函数名称： I2C_readN.
//功能说明： 接收N个字节，在这里N最小为2。
//函数参数： i2cNO:i2c模块号      ，其取值为0,1
//          slaveAddress：从机设备地址
//          commond：命令
//          Data：存放的数据首地址
//          DataNum：要读取的字节数（大于等于2）
//函数返回： 1：表示读取失败；0：读取成功
//=====================================================================
uint_8 I2C_readN(uint_8 I2C_No,uint_8 slaveAddress,uint_8 commond,\
		                                  uint_8 *Data, uint_8 DataNum)
{
	uint_8 i,temp;
	i2c_start(I2C_No);               //发送开始信号
	temp = (slaveAddress << 1) & 0xfe;
	i2c_write(I2C_No,temp);          //将地址和方向位发送给从机
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	if(i2c_get_ack(I2C_No) == 1)     //判断是否是ACK
		return 1;                    //若是非应答信号则返回失败
	i2c_write(I2C_No,commond);       //将命令发送给从机
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	i2c_restart(I2C_No);             //在restart之前可以增加少许延时
	temp = (slaveAddress << 1) | 0x01;
	i2c_write(I2C_No,temp);          //将地址和方向位发送给从机
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	if(i2c_get_ack(I2C_No) == 1)     //判断是否是ACK
		return 1;                    //若是非应答信号则返回失败
	i2c_changeDir(I2C_No,0);         //改变方向
	i2c_sendACK(I2C_No);             //发送应答位ACK
	i2c_read(I2C_No);                //空读，触发下个数据传递
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	for(i = 0; i < DataNum-2; i++)
	{
		Data[i] = i2c_read(I2C_No);
		if(i2c_wait(I2C_No) == 1)    //等待完成
			return 1;                //失败
	}
	i2c_sendNACK(I2C_No);            //发送非应答位ACK
	Data[i++] = i2c_read(I2C_No);
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	i2c_stop(I2C_No);                //发送停止信号
	Data[i] = i2c_read(I2C_No);//读取数据，操作完成，可以在后面加少量延时
}

//=====================================================================
//函数名称： I2C_rece1.
//功能说明： 接收单个字节
//函数参数： i2cNO:i2c模块号      ，其取值为0,1
//          slaveAddress：从机设备地址
//          Data：存放的数据首地址
//函数返回： 1：表示接收失败；0：接收成功
//=====================================================================
uint_8 I2C_rece1(uint_8 I2C_No, uint_8 slaveAddress, uint_8 *Data)
{
	uint_8 temp,i;
	i2c_start(I2C_No);             //在restart之前可以增加少许延时
	temp = (slaveAddress << 1) | 0x01;
	i2c_write(I2C_No,temp);          //将命令发送给从机
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	if(i2c_get_ack(I2C_No) == 1)     //判断是否是ACK
		return 1;                    //若是非应答信号则返回失败
	i2c_changeDir(I2C_No,0);         //改变方向
	i2c_sendNACK(I2C_No);             //发送应答位NACK
	i2c_read(I2C_No);                //空读，触发下个数据传递
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	i2c_stop(I2C_No);                //发送停止信号
	*Data = i2c_read(I2C_No); //读取数据，操作完成，可以在后面加少量延时
	return 0;
}


//=====================================================================
//函数名称： I2C_receN.
//功能说明： 接收N个字节，在这里N最小为2。
//函数参数： i2cNO:i2c模块号      ，其取值为0,1
//          slaveAddress：从机设备地址
//          Data：存放的数据首地址
//          DataNum：要读取的字节数（大于等于2）
//函数返回： 1：表示接收失败；0：接收成功
//=====================================================================
uint_8 I2C_receN(uint_8 I2C_No, uint_8 slaveAddress, uint_8 *Data,\
		                                                uint_8 DataNum)
{
	uint_8 temp,i;
	i2c_start(I2C_No);             //在restart之前可以增加少许延时
	temp = (slaveAddress << 1) | 0x01;
	i2c_write(I2C_No,temp);          //将命令发送给从机
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	if(i2c_get_ack(I2C_No) == 1)     //判断是否是ACK
		return 1;                    //若是非应答信号则返回失败
	i2c_changeDir(I2C_No,0);         //改变方向
	i2c_sendACK(I2C_No);             //发送应答位ACK
	i2c_read(I2C_No);                //空读，触发下个数据传递
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	for(i = 0; i < DataNum-2; i++)
	{
		Data[i] = i2c_read(I2C_No);
		i2c_wait(I2C_No);            //等待完成
	}
	i2c_sendNACK(I2C_No);            //发送非应答位ACK
	Data[i++] = i2c_read(I2C_No);
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	i2c_stop(I2C_No);                //发送停止信号
	Data[i] = i2c_read(I2C_No); //读取数据，操作完成，可以在后面加少量延时
	return 0;
}

//======================================================================
//函数名称： I2C_write1
//功能概要： 向从机写1个字节数据
//参数说明： I2C_No:模块号       ，其取值为0，1
//          slaveAddress：从机地址
//          Data:要发给从机的1个字节数据
//函数返回： 为0,成功写一个字节;为1,写一个字节失败
//函数说明:  1：表示发送失败；0：发送成功
//======================================================================
uint_8 I2C_write1(uint_8 I2C_No, uint_8 slaveAddress, uint_8 Data)
{
	i2c_start(I2C_No);               //发送开始信号
	slaveAddress = (slaveAddress << 1) & 0xfe;
	i2c_write(I2C_No,slaveAddress);  //将地址和方向位发送给从机
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	if(i2c_get_ack(I2C_No) == 1)     //判断是否是ACK
		return 1;                    //若是非应答信号则返回失败
	i2c_write(I2C_No,Data);          //将地址和方向位发送给从机
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	i2c_stop(I2C_No);                //发送停止信号
}

//======================================================================
//函数名称： I2C_writeN
//功能概要： 向从机写多字节数据
//参数说明： I2C_No:模块号       ，其取值为0，1
//          *Data:要发给从机数据首地址
//          DataNum：发送的字节数
//函数返回： 为0,成功写一个字节;为1,写一个字节失败
//函数说明: 1：表示发送失败；0：发送成功
//======================================================================
uint_8 I2C_writeN(uint_8 I2C_No, uint_8 slaveAddress, uint_8 *Data, \
		                                                uint_8 DataNum)
{
	uint_8 i;
	i2c_start(I2C_No);               //发送开始信号
	slaveAddress = (slaveAddress << 1) & 0xfe;
	i2c_write(I2C_No,slaveAddress);  //将地址和方向位发送给从机
	if(i2c_wait(I2C_No) == 1)        //等待完成
		return 1;                    //失败
	if(i2c_get_ack(I2C_No) == 1)     //判断是否是ACK
		return 1;                    //若是非应答信号则返回失败
	for(i = 0; i < DataNum; i++)
	{
		i2c_write(I2C_No,Data[i]);   //将地址和方向位发送给从机
		if(i2c_wait(I2C_No) == 1)    //等待完成
			return 1;                //失败
	}
	i2c_stop(I2C_No);                //发送停止信号
	return 0;                        //成功
}

//==============================================================================
//函数名称：I2C_enable_re_int.
//功能说明：打开i2c的IRQ中断。
//函数参数：i2cNO:i2c模块号，其取值为0，1
//函数返回：无。
//==============================================================================
void I2C_enable_re_int(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	i2c_ptr->S |= I2C_S_IICIF_MASK;
	i2c_ptr->C1 |= I2C_C1_IICIE_MASK;
	enable_irq (I2C_IRQ_TABLE[I2C_No]);
}

//==============================================================================
//函数名称：I2C_disable_re_int
//功能说明：关闭i2c的IRQ中断。
//函数参数：i2cNO:i2c模块号      ，其取值为0,1
//函数返回：无。
//==============================================================================
void I2C_disable_re_int(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	i2c_ptr->C1 &= ~I2C_C1_IICIE_MASK;
	disable_irq (I2C_IRQ_TABLE[I2C_No]);
}




