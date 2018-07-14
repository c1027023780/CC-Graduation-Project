//=====================================================================
//�ļ����ƣ�isr.c
//���ܸ�Ҫ�� �жϵײ���������Դ�ļ�
//��Ȩ���У����ݴ�ѧǶ��ʽϵͳ���������о���(sumcu.suda.edu.cn)
//���¼�¼��2017-04-07   V1.0
//=====================================================================
#include "includes.h"

extern uint_64 getTime(void);

//�ڲ���������
uint_8 CreateFrame(uint_8 Data,uint_8 * buffer,uint_16 *len);

//========================�жϺ�����������===============================

void SysTick_Handler(void)
{
	gTime++;
}

//==================================================================
//ISR��UART0_IRQHandler��UART0�жϷ������̣�
//��������������0���յ�����
//���ܸ�Ҫ����1�����жϱ�־
//       ��2������0���ؽ��յ�������
//==================================================================
void UART0_IRQHandler(void)
{
	uint_8 ch;
	uint_8 flag;
	DISABLE_INTERRUPTS;      //�����ж�
	//-------------------------------
	if(uart_get_re_int(UART_0))
	{
		ch = uart_re1 (UART_0, &flag);    //���ý���һ���ֽڵĺ�����������ж�λ
		if(flag)
		{
			uart_send1(UART_0, ch);     //��ԭ���ڷ���һ���ֽ�
		}
	}
	//-------------------------------
	ENABLE_INTERRUPTS;       //�����ж�
}

//==================================================================
//ISR��UART1_IRQHandler��UART1�жϷ������̣�
//��������������1���յ�����
//���ܸ�Ҫ����1�����жϱ�־
//       ��2���ն�UEͨ��ģ����չ������������
//==================================================================
void UART1_IRQHandler(void)
{
	uint_8 ch;
	uint_8 flag;
	DISABLE_INTERRUPTS;      //�����ж�
	//-------------------------------
	if(uart_get_re_int(UART_1))
	{
		ch = uart_re1 (UART_1, &flag);    //���ý���һ���ֽڵĺ�����������ж�λ
		if(flag)
		{
			uart_send1(UART_1, ch);     //��ԭ���ڷ���һ���ֽ�
		}
	}
	//-------------------------------
	ENABLE_INTERRUPTS;       //�����ж�
}

//==================================================================
//ISR��UART2_IRQHandler��UART2�жϷ������̣�
//��������������2���յ�����
//���ܸ�Ҫ����1�����жϱ�־
//       ��2������2���ؽ��յ�������
//==================================================================
void UART2_IRQHandler(void)
{
	uint_8 i;
	uint_8 ch;
	uint_8 flag;
	uint_8* updateFlag;
	static uint_8 buf[7];

	//�����ж�
	DISABLE_INTERRUPTS;      //�����ж�
	//-------------------------------
	if(uart_get_re_int(UART_2))
	{
		ch = uart_re1 (UART_2, &flag);    //���ý���һ���ֽڵĺ�����������ж�λ
		if(flag)
		{
			//�������µ�7���ֽ�����
			for(i = 6; i > 0;i--)
			{
				buf[i] = buf[i-1];
			}
			buf[0] = ch;
			//��������
			if(strncmp(buf,"ekahs",5) == 0)
			{
				uart_send_string(UART_2,"shake:GEC-6220");
			}
			//��������
			else if(strncmp(buf,"etadpu",6) == 0)
			{
//				flash_erase(SECTORNO-1);
//				flash_write(SECTORNO-1,0,6,"update");
				updateFlag = (uint_8 *)UPDATEADDRESS;
				memcpy(updateFlag,"update\0",7);

				NVIC_SystemReset();
			}
			//��װ����֡����
			if(CreateFrame(ch,gRecvBuf,&gRecvLen) == 0)
			{
				gFunFlag = 2;  //���յ���λ����������
			}
		}
	}
	//-------------------------------
	ENABLE_INTERRUPTS;       //�����ж�
}


//===========================================================================
//�������ƣ�CreateFrame
//���ܸ�Ҫ���齨����֡��������֡���ݼ��뵽����֡��
//����˵����Data��                   ����֡����
//          buffer:        ����֡����
//�������أ���֡״̬    0:��֡�ɹ�;1:��ʼ��֡��2��֡ͷ����3����֡ʧ��
//��ע��ʮ����������֡��ʽ
//             ֡ͷ +֡�� + ���� + ���� + У�� + ֡β
//             2B  2B   1B   nB   2B   2B
//===========================================================================
#define FrameHeadH    (0x50)         //֡ͷ     ASCII���ӦP
#define FrameHeadL    (0x50)         //֡ͷ     ASCII���ӦP
#define FrameTaiH     (0x43)         //֡β     ASCII���ӦC
#define FrameTaiL     (0x43)         //֡β     ASCII���ӦC

uint_8 CreateFrame(uint_8 Data,uint_8 * buffer,uint_16 *len)
{
	static uint_16 dataLen = 0;  //֡�ֽڼ�����
	static uint_16 frameLen = 0; //֡��
	uint_8 frameFlag;            //��֡״̬

	frameFlag = 1;
	switch(dataLen)
	{
	case 0:  //��һ������
		if(Data == FrameHeadH)
		{
			buffer[dataLen++] = Data;
			frameFlag = 1;    //��ʼ��֡
		}
		break;
	case 1:  //�ڶ�������
		if(Data == FrameHeadL)
		{
			buffer[dataLen++] = Data;
		}
		else
		{
			dataLen = 0;
			frameFlag = 2;    //֡ͷ����
		}
		break;
	case 2:  //����������
		buffer[dataLen++] = Data;
		frameLen = Data<<8;
		break;
	case 3:  //���ĸ�����
		buffer[dataLen++] = Data;
		frameLen += Data;
		break;
	default:
		buffer[dataLen++] = Data;
		if(frameLen == dataLen)
		{
			if(buffer[frameLen-2] == FrameTaiH && buffer[frameLen-1] == FrameTaiL)
			{
				frameFlag = 0; //��֡�ɹ�
				*len = frameLen;
				dataLen = 0;
				frameLen = 0;
			}
			else
			{
				frameFlag = 3; //��֡ʧ��
				dataLen = 0;
				frameLen = 0;
			}
		}
		break;
	}

	return frameFlag;
}
