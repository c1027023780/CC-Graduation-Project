//=====================================================================
//�ļ����ƣ�i2c.c
//���ܸ�Ҫ��i2c�ײ���������Դ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2013-05-4   V2.1
//        2015-01-21   V3.0
//=====================================================================
#include  "i2c.h"

static const I2C_MemMapPtr I2C_BASE_TABLE[2] = I2C_BASE_PTRS;
static const IRQn_Type I2C_IRQ_TABLE[2] = {I2C0_IRQn,I2C1_IRQn};


//****************************�ڲ����ú���*****************************
//=====================================================================
//�������ƣ�wait
//���ܸ�Ҫ���ȴ����
//����˵����I2C_No:ģ���,��ȡֵΪ0,1
//�������أ�1����ʾ��ʱ��0����ʾ����
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
//�������ƣ�i2c_read
//���ܸ�Ҫ��������
//����˵����I2C_No:ģ���,��ȡֵΪ0,1
//�������أ��Ĵ����е�����
//=====================================================================
static uint_8 i2c_read(uint_8 I2C_No)
{
	uint_8 result;
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	result = i2c_ptr->D;
	return result;
}

//=====================================================================
//�������ƣ� i2c_write
//���ܸ�Ҫ�� д����
//����˵���� I2C_No:ģ���,��ȡֵΪ0,1
//          data��Ҫд�������
//�������أ� ��
//=====================================================================
static void i2c_write(uint_8 I2C_No,uint_8 data)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	i2c_ptr->D = data;
}

//=====================================================================
//�������ƣ� i2c_stop
//���ܸ�Ҫ�� ֹͣ�ź�
//����˵���� I2C_No:ģ���,��ȡֵΪ0,1
//�������أ� ��
//=====================================================================
static void i2c_stop(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	BCLR(I2C_C1_MST_SHIFT,i2c_ptr->C1); //�������ĳɴӻ�����ֹͣ�ź�
	BCLR(I2C_C1_TX_SHIFT,i2c_ptr->C1);  //��λ����ģʽ
}

//=====================================================================
//�������ƣ� i2c_start
//���ܸ�Ҫ�� ��ʼ�ź�
//����˵���� I2C_No:ģ���,��ȡֵΪ0,1
//�������أ� ��
//=====================================================================
static void i2c_start(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	BSET(I2C_C1_TX_SHIFT,i2c_ptr->C1);  //��λ����ģʽ
	BSET(I2C_C1_MST_SHIFT,i2c_ptr->C1); //�������ĳɴӻ�����ֹͣ�ź�
}

//=====================================================================
//�������ƣ� i2c_start
//���ܸ�Ҫ�� ���¿�ʼ�ź�
//����˵���� I2C_No:ģ���,��ȡֵΪ0,1
//�������أ� ��
//=====================================================================
static void i2c_restart(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	BSET(I2C_C1_TX_SHIFT,i2c_ptr->C1);  //��λ����ģʽ
	BSET(I2C_C1_RSTA_SHIFT,i2c_ptr->C1); //�������ĳɴӻ�����ֹͣ�ź�
}

//=====================================================================
//�������ƣ� i2c_sendACK
//���ܸ�Ҫ�� ����ACK
//����˵���� I2C_No:ģ���,��ȡֵΪ0,1
//�������أ���
//=====================================================================
static void i2c_sendACK(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	BCLR(I2C_C1_TXAK_SHIFT,i2c_ptr->C1);
}

//=====================================================================
//�������ƣ� i2c_sendNACK
//���ܸ�Ҫ�� ����NACK
//����˵���� I2C_No:ģ���,��ȡֵΪ0,1
//�������أ���
//=====================================================================
static void i2c_sendNACK(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	BSET(I2C_C1_TXAK_SHIFT,i2c_ptr->C1);
}

//=====================================================================
//�������ƣ� i2c_changeDir
//���ܸ�Ҫ�� �ı䷽��
//����˵���� I2C_No:ģ���,��ȡֵΪ0,1
//          direction: ���� 0�ǽ��գ�1�Ƿ���
//�������أ� ��
//=====================================================================
static void i2c_changeDir(uint_8 I2C_No,uint_8 direction)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	i2c_ptr->C1 &= ~I2C_C1_TX_MASK;
	i2c_ptr->C1 |= I2C_C1_TX(direction);
}

//=====================================================================
//�������ƣ� i2c_get_ack
//���ܸ�Ҫ�� ��ȡӦ����߷�Ӧ���ź�
//����˵���� I2C_No:ģ���,��ȡֵΪ0,1
//�������أ�0��Ӧ���źţ�1����Ӧ���ź�
//=====================================================================
static uint_8 i2c_get_ack(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	uint_8 ack;
	ack = BGET(I2C_S_RXAK_SHIFT,i2c_ptr->S);
    return ack;
}

///********************����ӿں���*************************************
//=====================================================================
//�������ƣ�i2c_init                                                        
//���ܸ�Ҫ����ʼ��IICXģ�顣Ĭ��Ϊ100K.
//����˵����I2C_No:ģ���,��ȡֵΪ0,1
//          Mode ģʽ  1������     0���ӻ�
//          address ��ģ���ʼ����ַ    ��Χ(1~255)
//          BaudRateΪ�����ʣ��䵥λΪKb/s����ȡֵΪ25,50,75,100K
//�������أ� ��
//=====================================================================
void I2C_init(uint_8 I2C_No,uint_8 Mode,uint_8 address,uint_8 BaudRate)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	if(I2C_No == IIC_0)
	{
		BSET(SIM_SCGC4_I2C0_SHIFT,SIM_SCGC4); //��ʱ����
		#if (IIC_0_GROUP == 1)
		PORTE_PCR18 = PORT_PCR_MUX(0x4);      //ʹ��IIC0_SDA
		PORTE_PCR19 = PORT_PCR_MUX(0x4);      //ʹ��IIC0_SCL
		#endif
		#if (IIC_0_GROUP == 2)
		PORTE_PCR24 = PORT_PCR_MUX(0x5);      //ʹ��IIC0_SCL
		PORTE_PCR25 = PORT_PCR_MUX(0x5);      //ʹ��IIC0_SDA
		#endif

		#if (IIC_0_GROUP == 3)
		PORTB_PCR0 = PORT_PCR_MUX(0x2);      //ʹ��IIC0_SCL
		PORTB_PCR1 = PORT_PCR_MUX(0x2);      //ʹ��IIC0_SDA
		#endif

		#if (IIC_0_GROUP == 4)
		PORTB_PCR2 = PORT_PCR_MUX(0x2);      //ʹ��IIC0_SCL
		PORTB_PCR3 = PORT_PCR_MUX(0x2);      //ʹ��IIC0_SDA
		#endif

		#if (IIC_0_GROUP == 5)
		PORTC_PCR8 = PORT_PCR_MUX(0x2);      //ʹ��IIC0_SCL
		PORTC_PCR9 = PORT_PCR_MUX(0x2);      //ʹ��IIC0_SDA
		#endif
	}
	else
	{
		BSET(SIM_SCGC4_I2C1_SHIFT,SIM_SCGC4); //��ʱ����
		#if (IIC_1_GROUP == 1)
		PORTE_PCR1 = PORT_PCR_MUX(0x6);      //ʹ��IIC1_SCL
		PORTE_PCR0 = PORT_PCR_MUX(0x6);      //ʹ��IIC1_SDA
		#endif

		#if (IIC_1_GROUP == 2)
		PORTA_PCR3 = PORT_PCR_MUX(0x2);      //ʹ��IIC1_SCL
		PORTA_PCR4 = PORT_PCR_MUX(0x2);      //ʹ��IIC1_SDA
		#endif

		#if (IIC_1_GROUP == 3)
		PORTC_PCR1 = PORT_PCR_MUX(0x2);      //ʹ��IIC1_SCL
		PORTC_PCR2 = PORT_PCR_MUX(0x2);      //ʹ��IIC1_SDA
		#endif

		#if (IIC_1_GROUP == 4)
		PORTC_PCR10 = PORT_PCR_MUX(0x2);      //ʹ��IIC1_SCL
		PORTC_PCR11 = PORT_PCR_MUX(0x2);      //ʹ��IIC1_SDA
		#endif
	}
	i2c_ptr->C1 = 0X00;   //��ʼ��C1
	i2c_ptr->FLT |= I2C_FLT_STOPF_MASK;//��ֹͣ��־λ
	i2c_ptr->S |= I2C_S_ARBL_MASK | I2C_S_IICIF_MASK; //��ʼ��S
    if(1 == Mode)   //����ģʽ
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
        BSET(I2C_C1_IICEN_SHIFT,i2c_ptr->C1);//��i2c0ģ��ʹ��
    }
    else            //�ӻ�ģʽ
    {
    	i2c_ptr->A1 = address << 1;
        BSET(I2C_C1_IICEN_SHIFT,i2c_ptr->C1);//ʹ��I2Cģ��
        BCLR(I2C_C1_TX_SHIFT,i2c_ptr->C1);//TX = 0,MCU����Ϊ����ģʽ
        BCLR(I2C_C1_MST_SHIFT,i2c_ptr->C1);//MSTλ��1���0������Ϊ�ӻ�ģʽ
    }
}

//=====================================================================
//�������ƣ�  i2c_read1
//���ܸ�Ҫ��  �Ӵӻ���1���ֽ�����
//����˵����  I2C_No:ģ��ţ���ȡֵΪ0��1
//           Data:�����յ���һ���ֽ�����
//�������أ�  1����ʾ��ȡʧ�ܣ�0����ȡ�ɹ�
//����˵��:  �ú���Ϊ���⺯�������Ը��������ȡ����Ӧ��1���ֽڣ������ȡ2��
//           �������ֽ������ݣ���ʹ��I2C_readN������������ʹ��ѭ�����ô˺�
//           ����
//=====================================================================
uint_8 I2C_read1(uint_8 I2C_No,uint_8 slaveAddress,uint_8 commond,\
		                                                  uint_8 *Data)
{      
	uint_8 temp;
	i2c_start(I2C_No);               //���Ϳ�ʼ�ź�
	temp = (slaveAddress << 1) & 0xfe;
	i2c_write(I2C_No,temp);          //����ַ�ͷ���λ���͸��ӻ�
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	if(i2c_get_ack(I2C_No) == 1)     //�ж��Ƿ���ACK
		return 1;                    //���Ƿ�Ӧ���ź��򷵻�ʧ��
	i2c_write(I2C_No,commond);       //������͸��ӻ�
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	i2c_restart(I2C_No);             //���¿�ʼ
	temp = (slaveAddress << 1) | 0x01;
	i2c_write(I2C_No,temp);  //����ַ�ͷ���λ���͸��ӻ�
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	if(i2c_get_ack(I2C_No) == 1)     //�ж��Ƿ���ACK
		return 1;                    //���Ƿ�Ӧ���ź��򷵻�ʧ��
	i2c_changeDir(I2C_No,0);         //�ı䷽��
	i2c_sendNACK(I2C_No);            //����NACK
	i2c_read(I2C_No);                //�ն��������¸����ݴ���
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	i2c_stop(I2C_No);                //����ֹͣ�ź�
	*Data = i2c_read(I2C_No);  //��ȡ���ݣ�������ɣ������ں����������ʱ
	return 0;
}

//=====================================================================
//�������ƣ� I2C_readN.
//����˵���� ����N���ֽڣ�������N��СΪ2��
//���������� i2cNO:i2cģ���      ����ȡֵΪ0,1
//          slaveAddress���ӻ��豸��ַ
//          commond������
//          Data����ŵ������׵�ַ
//          DataNum��Ҫ��ȡ���ֽ��������ڵ���2��
//�������أ� 1����ʾ��ȡʧ�ܣ�0����ȡ�ɹ�
//=====================================================================
uint_8 I2C_readN(uint_8 I2C_No,uint_8 slaveAddress,uint_8 commond,\
		                                  uint_8 *Data, uint_8 DataNum)
{
	uint_8 i,temp;
	i2c_start(I2C_No);               //���Ϳ�ʼ�ź�
	temp = (slaveAddress << 1) & 0xfe;
	i2c_write(I2C_No,temp);          //����ַ�ͷ���λ���͸��ӻ�
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	if(i2c_get_ack(I2C_No) == 1)     //�ж��Ƿ���ACK
		return 1;                    //���Ƿ�Ӧ���ź��򷵻�ʧ��
	i2c_write(I2C_No,commond);       //������͸��ӻ�
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	i2c_restart(I2C_No);             //��restart֮ǰ��������������ʱ
	temp = (slaveAddress << 1) | 0x01;
	i2c_write(I2C_No,temp);          //����ַ�ͷ���λ���͸��ӻ�
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	if(i2c_get_ack(I2C_No) == 1)     //�ж��Ƿ���ACK
		return 1;                    //���Ƿ�Ӧ���ź��򷵻�ʧ��
	i2c_changeDir(I2C_No,0);         //�ı䷽��
	i2c_sendACK(I2C_No);             //����Ӧ��λACK
	i2c_read(I2C_No);                //�ն��������¸����ݴ���
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	for(i = 0; i < DataNum-2; i++)
	{
		Data[i] = i2c_read(I2C_No);
		if(i2c_wait(I2C_No) == 1)    //�ȴ����
			return 1;                //ʧ��
	}
	i2c_sendNACK(I2C_No);            //���ͷ�Ӧ��λACK
	Data[i++] = i2c_read(I2C_No);
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	i2c_stop(I2C_No);                //����ֹͣ�ź�
	Data[i] = i2c_read(I2C_No);//��ȡ���ݣ�������ɣ������ں����������ʱ
}

//=====================================================================
//�������ƣ� I2C_rece1.
//����˵���� ���յ����ֽ�
//���������� i2cNO:i2cģ���      ����ȡֵΪ0,1
//          slaveAddress���ӻ��豸��ַ
//          Data����ŵ������׵�ַ
//�������أ� 1����ʾ����ʧ�ܣ�0�����ճɹ�
//=====================================================================
uint_8 I2C_rece1(uint_8 I2C_No, uint_8 slaveAddress, uint_8 *Data)
{
	uint_8 temp,i;
	i2c_start(I2C_No);             //��restart֮ǰ��������������ʱ
	temp = (slaveAddress << 1) | 0x01;
	i2c_write(I2C_No,temp);          //������͸��ӻ�
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	if(i2c_get_ack(I2C_No) == 1)     //�ж��Ƿ���ACK
		return 1;                    //���Ƿ�Ӧ���ź��򷵻�ʧ��
	i2c_changeDir(I2C_No,0);         //�ı䷽��
	i2c_sendNACK(I2C_No);             //����Ӧ��λNACK
	i2c_read(I2C_No);                //�ն��������¸����ݴ���
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	i2c_stop(I2C_No);                //����ֹͣ�ź�
	*Data = i2c_read(I2C_No); //��ȡ���ݣ�������ɣ������ں����������ʱ
	return 0;
}


//=====================================================================
//�������ƣ� I2C_receN.
//����˵���� ����N���ֽڣ�������N��СΪ2��
//���������� i2cNO:i2cģ���      ����ȡֵΪ0,1
//          slaveAddress���ӻ��豸��ַ
//          Data����ŵ������׵�ַ
//          DataNum��Ҫ��ȡ���ֽ��������ڵ���2��
//�������أ� 1����ʾ����ʧ�ܣ�0�����ճɹ�
//=====================================================================
uint_8 I2C_receN(uint_8 I2C_No, uint_8 slaveAddress, uint_8 *Data,\
		                                                uint_8 DataNum)
{
	uint_8 temp,i;
	i2c_start(I2C_No);             //��restart֮ǰ��������������ʱ
	temp = (slaveAddress << 1) | 0x01;
	i2c_write(I2C_No,temp);          //������͸��ӻ�
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	if(i2c_get_ack(I2C_No) == 1)     //�ж��Ƿ���ACK
		return 1;                    //���Ƿ�Ӧ���ź��򷵻�ʧ��
	i2c_changeDir(I2C_No,0);         //�ı䷽��
	i2c_sendACK(I2C_No);             //����Ӧ��λACK
	i2c_read(I2C_No);                //�ն��������¸����ݴ���
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	for(i = 0; i < DataNum-2; i++)
	{
		Data[i] = i2c_read(I2C_No);
		i2c_wait(I2C_No);            //�ȴ����
	}
	i2c_sendNACK(I2C_No);            //���ͷ�Ӧ��λACK
	Data[i++] = i2c_read(I2C_No);
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	i2c_stop(I2C_No);                //����ֹͣ�ź�
	Data[i] = i2c_read(I2C_No); //��ȡ���ݣ�������ɣ������ں����������ʱ
	return 0;
}

//======================================================================
//�������ƣ� I2C_write1
//���ܸ�Ҫ�� ��ӻ�д1���ֽ�����
//����˵���� I2C_No:ģ���       ����ȡֵΪ0��1
//          slaveAddress���ӻ���ַ
//          Data:Ҫ�����ӻ���1���ֽ�����
//�������أ� Ϊ0,�ɹ�дһ���ֽ�;Ϊ1,дһ���ֽ�ʧ��
//����˵��:  1����ʾ����ʧ�ܣ�0�����ͳɹ�
//======================================================================
uint_8 I2C_write1(uint_8 I2C_No, uint_8 slaveAddress, uint_8 Data)
{
	i2c_start(I2C_No);               //���Ϳ�ʼ�ź�
	slaveAddress = (slaveAddress << 1) & 0xfe;
	i2c_write(I2C_No,slaveAddress);  //����ַ�ͷ���λ���͸��ӻ�
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	if(i2c_get_ack(I2C_No) == 1)     //�ж��Ƿ���ACK
		return 1;                    //���Ƿ�Ӧ���ź��򷵻�ʧ��
	i2c_write(I2C_No,Data);          //����ַ�ͷ���λ���͸��ӻ�
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	i2c_stop(I2C_No);                //����ֹͣ�ź�
}

//======================================================================
//�������ƣ� I2C_writeN
//���ܸ�Ҫ�� ��ӻ�д���ֽ�����
//����˵���� I2C_No:ģ���       ����ȡֵΪ0��1
//          *Data:Ҫ�����ӻ������׵�ַ
//          DataNum�����͵��ֽ���
//�������أ� Ϊ0,�ɹ�дһ���ֽ�;Ϊ1,дһ���ֽ�ʧ��
//����˵��: 1����ʾ����ʧ�ܣ�0�����ͳɹ�
//======================================================================
uint_8 I2C_writeN(uint_8 I2C_No, uint_8 slaveAddress, uint_8 *Data, \
		                                                uint_8 DataNum)
{
	uint_8 i;
	i2c_start(I2C_No);               //���Ϳ�ʼ�ź�
	slaveAddress = (slaveAddress << 1) & 0xfe;
	i2c_write(I2C_No,slaveAddress);  //����ַ�ͷ���λ���͸��ӻ�
	if(i2c_wait(I2C_No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	if(i2c_get_ack(I2C_No) == 1)     //�ж��Ƿ���ACK
		return 1;                    //���Ƿ�Ӧ���ź��򷵻�ʧ��
	for(i = 0; i < DataNum; i++)
	{
		i2c_write(I2C_No,Data[i]);   //����ַ�ͷ���λ���͸��ӻ�
		if(i2c_wait(I2C_No) == 1)    //�ȴ����
			return 1;                //ʧ��
	}
	i2c_stop(I2C_No);                //����ֹͣ�ź�
	return 0;                        //�ɹ�
}

//==============================================================================
//�������ƣ�I2C_enable_re_int.
//����˵������i2c��IRQ�жϡ�
//����������i2cNO:i2cģ��ţ���ȡֵΪ0��1
//�������أ��ޡ�
//==============================================================================
void I2C_enable_re_int(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	i2c_ptr->S |= I2C_S_IICIF_MASK;
	i2c_ptr->C1 |= I2C_C1_IICIE_MASK;
	enable_irq (I2C_IRQ_TABLE[I2C_No]);
}

//==============================================================================
//�������ƣ�I2C_disable_re_int
//����˵�����ر�i2c��IRQ�жϡ�
//����������i2cNO:i2cģ���      ����ȡֵΪ0,1
//�������أ��ޡ�
//==============================================================================
void I2C_disable_re_int(uint_8 I2C_No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[I2C_No];
	i2c_ptr->C1 &= ~I2C_C1_IICIE_MASK;
	disable_irq (I2C_IRQ_TABLE[I2C_No]);
}




