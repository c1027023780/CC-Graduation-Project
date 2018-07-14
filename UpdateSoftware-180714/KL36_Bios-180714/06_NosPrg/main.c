//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//======================================================================

//��������GLOBLE_VAR�궨�弰������ͷ�ļ�
#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ���������ȫ�ֱ���������ʹ������
#include "includes.h"   //������ͷ�ļ�

//������ʵ����Ҫ��ɾ��main.cʹ�õ��ڲ�����������
void updateJudge();
void systemTimeGet(uint_8 *date);
void systemTimeSet(uint_64 tick);
void systemInfoGet(uint_8 *data);

int main(void)
{
	//��1��������ʵ����Ҫ��ɾ������������ʹ�õı���������ʱ��׼��ֵ��
	uint_16 i;
	uint_8 flag;
	uint_8 order;
	uint_16 mUpdateLen;
	vuint_32 mCount;
	uint_8 mUpdateInfo[100];

	//��2���������� �����ж�
	DISABLE_INTERRUPTS;
	//��3��������ʵ����Ҫ��ɾ�� ��ʼ������ģ��
	uart_init(UART_2,115200);
	systick_init(CORE_CLOCK_DIV_16,1000);
	flash_init();
	light_init(LIGHT_GREEN,LIGHT_ON);
	ueupdate_init(USERBASE/1024);
	//��4��������ʵ����Ҫ��ɾ�� ���йر�������ֵ
	gFunFlag = 0;
	gRecvLen = 0;
	gTime = 0;
	mCount = 0;

	//��5��������ʵ����Ҫ��ɾ�� ʹ��ģ���ж�
	uart_enable_re_int(UART_2);
	//��6�������䡿�����ж�
	ENABLE_INTERRUPTS;   //�����ж�

	updateJudge();//�ж��Ƿ���Ҫ���и��²���

	//������ѭ��
	//��ѭ����ʼ======================================================
	for(;;)
	{
		switch(gFunFlag)
		{
		case 0:  //�����������

			light_control(LIGHT_GREEN,LIGHT_OFF);

			//��ת�û�����
			DISABLE_INTERRUPTS;
			bootloader_goToUserApp(USERBASE);
			ENABLE_INTERRUPTS;   //�����ж�
			break;
		case 1:  //��ʾ��λ������bootloader���򣬿ɽ��ո�������
			uart_send_string(UART_2,"Start Update");
			gFunFlag = 10; //��0��1��2��������
			break;
		case 2:  //���յ���������
			gFunFlag = 10;  //��0��1��2��������
			//�������ݣ����и��²���
			flag = ueupdate_recv(gRecvBuf,gRecvLen);
			//���ݸ��½��պ����ķ���ֵ���������ݻط���������µȲ������ж�
			switch(flag)
			{
			case 0:  //�ɹ�������λ������/�����������
				if(ueupdate_back(mUpdateInfo,&mUpdateLen) == 0)
				{
					//�践������
					uart_sendN(UART_2,mUpdateLen,mUpdateInfo);
				}
				break;
			case 3:  //���������ϣ�ת���û�����
				gFunFlag = 0;
				break;
			default:  //�������󣬲�������
				break;
			}
			break;
		default:  //�ȴ����ո��´���
			mCount++;
			if(mCount > 0x5ffff)  //�̵���˸����ʾ����Bootloader����
			{
				mCount = 0;
				light_change(LIGHT_GREEN);
			}
			break;
		}
	} //��ѭ��end_for
	//��ѭ������======================================================
}

//uint_64 *tick
void systemTimeGet(uint_8 *date)
{
	timeChange(gTime,date);
}

void systemTimeSet(uint_64 tick)
{
	gTime = tick;
}

void systemInfoGet(uint_8 *data)
{
	flash_read_logic(data,SECTORNO,0,24);
}

//=====================================================================
//�������ƣ�updateJudge
//�������أ���
//����˵������
//���ܸ�Ҫ����ȡ29�������ݣ��ж��Ƿ���Ҫ���и��²���
//=====================================================================
void updateJudge()
{
	//��1����������
	uint_8* str;
	uint_32 msp;

	//��ȡ�û������ж��������׵�ַ��������RAM��ߵ�ַ+1������û��������
	flash_read_logic((uint_8*)&msp,SECTORNO+1,0,4);
	if(msp != 0x20001800) //MKL36Z64��RAM��ߵ�ַ+1����0x20001800
	{
		//û���û�����
		gFunFlag = 10;//��0��1��2��������
	}
	else  //���û�����
	{
		str = (uint_8 *)UPDATEADDRESS;
		//��3���ж��Ƿ���Ҫ���²���
		if(strcmp(str,"update") != 0)
			gFunFlag = 0;//�������
		else
		{
			str[0] = 0;
			gFunFlag = 1;//�����
		}
	}
}
