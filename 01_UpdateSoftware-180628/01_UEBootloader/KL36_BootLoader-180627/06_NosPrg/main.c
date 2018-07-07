//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//======================================================================

//��������GLOBLE_VAR�궨�弰������ͷ�ļ�
#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ���������ȫ�ֱ���������ʹ������
#include "includes.h"   //������ͷ�ļ�

//������ʵ����Ҫ��ɾ��main.cʹ�õ��ڲ�����������
void updateJudge();
void funToFlash();
void systemTimeGet(uint_8 *date);

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
  	//systick_init(CORE_CLOCK_DIV_16,1000);
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
			uart_sendN(UART_2,mUpdateLen,mUpdateInfo);
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

void systemTimeGet(uint_8 *date)
{
	time_t t;
	struct tm* curTime;

	t = (time_t)gTime;
	curTime = gmtime(&t);
	strftime(date,20,"%Y-%m-%d %H:%M:%S",curTime);
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
	//��2.1��bootloader����ӿں���
	info.fun_systemTimeGet = (uint_32)&systemTimeGet;
	//��2.2��adc������2��
	info.fun_adc_init = (uint_32)&adc_init;
	info.fun_adc_read = (uint_32)&adc_read;
	//��2.3��flash������7��
	info.fun_flash_init          = (uint_32)&flash_init;
	info.fun_flash_erase         = (uint_32)&flash_erase;
	info.fun_flash_write         = (uint_32)&flash_write;
	info.fun_flash_read_logic    = (uint_32)&flash_read_logic;
	info.fun_flash_read_physical = (uint_32)&flash_read_physical;
	info.fun_flash_protect       = (uint_32)&flash_protect;
	info.fun_flash_isempty       = (uint_32)&flash_isempty;
	//��2.4��gpio������11��
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
	//��2.5��lptmr������4��
	info.fun_lptmr_init        = (uint_32)&lptmr_init;
	info.fun_lptmr_reset       = (uint_32)&lptmr_reset;
	info.fun_enable_lptmr_int  = (uint_32)&enable_lptmr_int;
	info.fun_disable_lptmr_int = (uint_32)&disable_lptmr_int;
	//��2.6��spi������7��
	info.fun_SPI_init           = (uint_32)&SPI_init;
	info.fun_SPI_send1          = (uint_32)&SPI_send1;
	info.fun_SPI_sendN          = (uint_32)&SPI_sendN;
	info.fun_SPI_receive1       = (uint_32)&SPI_receive1;
	info.fun_SPI_receiveN       = (uint_32)&SPI_receiveN;
	info.fun_SPI_enable_re_int  = (uint_32)&SPI_enable_re_int;
	info.fun_SPI_disable_re_int = (uint_32)&SPI_disable_re_int;
	//��2.7��tsi������6��
	info.fun_tsi_init           = (uint_32)&tsi_init;
	info.fun_tsi_get_value16    = (uint_32)&tsi_get_value16;
	info.fun_tsi_set_threshold  = (uint_32)&tsi_set_threshold;
	info.fun_tsi_enable_re_int  = (uint_32)&tsi_enable_re_int;
	info.fun_tsi_disable_re_int = (uint_32)&tsi_disable_re_int;
	info.fun_tsi_softsearch     = (uint_32)&tsi_softsearch;
	//��2.8��uart������10��
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
	//��2.9��uecom��������ָ�룬21��
//	info.fun_uecom_init             = (uint_32)&uecom_init;
//	info.fun_uecom_fast_init        = (uint_32)&uecom_fast_init;
//	info.fun_uecom_deInit           = (uint_32)&uecom_deInit;
//	info.fun_uecom_switch           = (uint_32)&uecom_switch;
//	info.fun_uecom_send             = (uint_32)&uecom_send;
//	info.fun_uecom_getIMEI          = (uint_32)&uecom_getIMEI;
//	info.fun_uecom_getIMSI          = (uint_32)&uecom_getIMSI;
//	info.fun_uecom_getFirmVer       = (uint_32)&uecom_getFirmVer;
//	info.fun_uecom_getSignalPower   = (uint_32)&uecom_getSignalPower;
//	info.fun_uecom_getBSNum         = (uint_32)&uecom_getBSNum;
//	info.fun_uecom_getUEIP          = (uint_32)&uecom_getUEIP;
//	info.fun_uecom_config           = (uint_32)&uecom_config;
//	info.fun_uecom_getCurCommunity  = (uint_32)&uecom_getCurCommunity;
//	info.fun_http_get               = (uint_32)&http_get;
//	info.fun_uecom_interrupt        = (uint_32)&uecom_interrupt;
//	info.fun_uecom_default_delay_ms = (uint_32)&uecom_default_delay_ms;
//	info.fun_gnss_init              = (uint_32)&gnss_init;
//	info.fun_gnss_get_time          = (uint_32)&gnss_get_time;
//	info.fun_gnss_get_speed         = (uint_32)&gnss_get_speed;
//	info.fun_gnss_get_location      = (uint_32)&gnss_get_location;
//	info.fun_gnss_get_attitude      = (uint_32)&gnss_get_attitude;

	//��3������ָ��д��flash
	flash_erase(SECTORNO);
	flash_write(SECTORNO,0,sizeof(struct funInfo),(uint_8*)&info);
}
