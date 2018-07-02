//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//======================================================================

//��������GLOBLE_VAR�궨�弰������ͷ�ļ�
#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ���������ȫ�ֱ���������ʹ������
#include "includes.h"   //������ͷ�ļ�

//������ʵ����Ҫ��ɾ��main.cʹ�õ��ڲ�����������
void updateJudge();
void funToFlash();

uint_64 getTime(void);

int main(void)
{
	//��1��������ʵ����Ҫ��ɾ������������ʹ�õı���������ʱ��׼��ֵ��
	uint_16 i;
	uint_8 flag;
	uint_8 order;
	uint_16 mUpdateLen;
	uint_32 mCount;
	uint_8 mUpdateInfo[512];

	//��2���������� �����ж�
  	DISABLE_INTERRUPTS;
	//��3��������ʵ����Ҫ��ɾ�� ��ʼ������ģ��
  	uart_init(UART_2,9600);
  	systick_init(CORE_CLOCK_DIV_16,1000);
	flash_init();
	light_init(LIGHT_GREEN,LIGHT_OFF);
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
			funToFlash(); //����������ָ��д��flash

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
			flag = ueupdate_recv(gRecvBuf,gRecvLen);
			switch(flag)
			{
    		case 0:  order = 4;break;  //�������ݳɹ�
    		case 1:  continue;  //���Ǹ�������
    		case 3:  order = 5;break;  //��ǰ���¹��̷�������
    		case 4:  order = 6;break;  //ȱʧ����
    		default: order = 7;break;  //����
			}
			if(flag == 6)  //���������ϣ�ת���û�����
			{
				gFunFlag = 0;
				continue;
			}
			//����λ����������
			ueupdate_send(order,mUpdateInfo,&mUpdateLen);
			for(i = 0;i < mUpdateLen;i++)
			{
				uart_send1(UART_2,mUpdateInfo[i]);
			}
			//uart_sendN(UART_2,mUpdateLen,mUpdateInfo); //�ᷢ��ȫ
			gFunFlag = 10;  //��0��1��2��������
			break;
		default:
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

uint_64 getTime(void)
{
	return gTime;
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
	uint_8 str[7];
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
		//��2����ȡflash����״̬����
		flash_read_logic(str,SECTORNO,sizeof(struct funInfo),6);
		str[6] = 0;

		//��3���ж��Ƿ���Ҫ���²���
		if(strcmp(str,"update") != 0)
			gFunFlag = 0;//�������
		else
			gFunFlag = 1;//�����
	}
}

//=====================================================================
//�������ƣ�funToFlash
//�������أ���
//����˵������
//���ܸ�Ҫ������������������ַд��Flash�����û�������ָ�����
//=====================================================================
void funToFlash()
{
	//��1����������
	struct funInfo info;

	//��2������ָ�븳ֵ
	//��2.1��gpio����
	info.fun_gpio_init           = (uint_32)&gpio_init;
	info.fun_gpio_set            = (uint_32)&gpio_set;
	info.fun_gpio_get            = (uint_32)&gpio_get;
	info.fun_gpio_pull           = (uint_32)&gpio_pull;
	info.fun_gpio_reverse        = (uint_32)&gpio_reverse;
	info.fun_gpio_enable_int     = (uint_32)&gpio_enable_int;
	info.fun_gpio_disable_int    = (uint_32)&gpio_disable_int;
	info.fun_gpio_get_int        = (uint_32)&gpio_get_int;
	info.fun_gpio_clear_int      = (uint_32)&gpio_clear_int;
	info.fun_gpio_clear_allint   = (uint_32)&gpio_clear_allint;
	info.fun_gpio_drive_strength = (uint_32)&gpio_drive_strength;
	//��2.2��uart����
	info.fun_uart_init           = (uint_32)&uart_init;
	info.fun_uart_send1          = (uint_32)&uart_send1;
	info.fun_uart_sendN          = (uint_32)&uart_sendN;
	info.fun_uart_send_string    = (uint_32)&uart_send_string;
	info.fun_uart_re1            = (uint_32)&uart_re1;
	info.fun_uart_reN            = (uint_32)&uart_reN;
	info.fun_uart_enable_re_int  = (uint_32)&uart_enable_re_int;
	info.fun_uart_disable_re_int = (uint_32)&uart_disable_re_int;
	info.fun_uart_get_re_int     = (uint_32)&uart_get_re_int;
	info.fun_uart_deint          = (uint_32)&uart_deint;
	//��2.3��flash����
	info.fun_flash_init          = (uint_32)&flash_init;
	info.fun_flash_erase         = (uint_32)&flash_erase;
	info.fun_flash_write         = (uint_32)&flash_write;
	info.fun_flash_read_logic    = (uint_32)&flash_read_logic;
	info.fun_flash_read_physical = (uint_32)&flash_read_physical;
	info.fun_flash_protect       = (uint_32)&flash_protect;
	info.fun_flash_isempty       = (uint_32)&flash_isempty;
	//��2.4��light����
	info.fun_light_init    = (uint_32)&light_init;
	info.fun_light_control = (uint_32)&light_control;
	info.fun_light_change  = (uint_32)&light_change;

	info.fun_getTime = (uint_32)&getTime;

	//��3������ָ��д��flash
	flash_erase(SECTORNO);
	flash_write(SECTORNO,0,sizeof(struct funInfo),(uint_8*)&info);
}
