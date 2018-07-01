//======================================================================
//�ļ����ƣ�includes.h
//���ܸ�Ҫ��Ӧ�ù�����ͷ�ļ�
//��Ȩ���У����ݴ�ѧǶ��ʽϵͳ���������о���(sumcu.suda.edu.cn)
//�汾���£�2017-02-01  V1.0��2017-09-27  V2.0
//������������1�������õ��Ĺ�������2��ȫ�ֱ���һ�������ദʹ�ô���������Σ�
//       ��3��ȫ�ֱ�����������4�������궨��
//======================================================================

#ifndef _INCLUDES_H
#define _INCLUDES_H

//��1��������ʵ����Ҫ��ɾ�������õ��Ĺ���
#include "common.h"
#include "adc.h"
#include "flash.h"
#include "gpio.h"
#include "lptmr.h"
#include "spi.h"
#include "systick.h"
#include "tsi.h"
#include "uart.h"
#include "bootloader.h"
#include "light.h"
//#include "uecom.h"
#include "ueupdate.h"
#include "string.h"
#include "time.h"


//��2����������ȫ�ֱ���һ�������ദʹ�ô����������
//    Ϊ��������ȫ�ֱ�����ʹ��ǰ׺G_VAR_PREFIX�궨����׼��
//    GLOBLE_VAR��main.c���к궨��
#ifdef GLOBLE_VAR		 //main.c�ļ��а�����ͷ�ļ�ʱ��ȫ�ֱ������ӡ�extern��ǰ׺
  #define G_VAR_PREFIX
#else                    //�����ļ��а�����ͷ�ļ�ʱ��ȫ�ֱ����Զ��ӡ�extern��ǰ׺
  #define G_VAR_PREFIX extern
#endif

#pragma  pack(push,1)
struct funInfo{
	//bootloader�����ṩ�Ľӿں���
	uint_32 fun_systemTimeGet;

	//adc��������ָ�룬2��
	uint_32 fun_adc_init;
	uint_32 fun_adc_read;

	//flash����ָ�룬7��
	uint_32 fun_flash_init;
	uint_32 fun_flash_erase;
	uint_32 fun_flash_write;
	uint_32 fun_flash_read_logic;
	uint_32 fun_flash_read_physical;
	uint_32 fun_flash_protect;
	uint_32 fun_flash_isempty;

	//gpio��������ָ�룬11��
	uint_32 fun_gpio_init;
	uint_32 fun_gpio_set;
	uint_32 fun_gpio_get;
	uint_32 fun_gpio_reverse;
	uint_32 fun_gpio_pull;
	uint_32 fun_gpio_enable_int;
	uint_32 fun_gpio_disable_int;
	uint_32 fun_gpio_get_int;
	uint_32 fun_gpio_clear_int;
	uint_32 fun_gpio_clear_allint;
	uint_32 fun_gpio_drive_strength;

	//lptmr��������ָ�룬4��
	uint_32 fun_lptmr_init;
	uint_32 fun_lptmr_reset;
	uint_32 fun_enable_lptmr_int;
	uint_32 fun_disable_lptmr_int;

	//spi��������ָ�룬7��
	uint_32 fun_SPI_init;
	uint_32 fun_SPI_send1;
	uint_32 fun_SPI_sendN;
	uint_32 fun_SPI_receive1;
	uint_32 fun_SPI_receiveN;
	uint_32 fun_SPI_enable_re_int;
	uint_32 fun_SPI_disable_re_int;

	//tsi��������ָ�룬6��
	uint_32 fun_tsi_init;
	uint_32 fun_tsi_get_value16;
	uint_32 fun_tsi_set_threshold;
	uint_32 fun_tsi_enable_re_int;
	uint_32 fun_tsi_disable_re_int;
	uint_32 fun_tsi_softsearch;

	//uart����ָ�룬10��
	uint_32 fun_uart_init;
	uint_32 fun_uart_send1;
	uint_32 fun_uart_sendN;
	uint_32 fun_uart_send_string;
	uint_32 fun_uart_re1;
	uint_32 fun_uart_reN;
	uint_32 fun_uart_enable_re_int;
	uint_32 fun_uart_disable_re_int;
	uint_32 fun_uart_get_re_int;
	uint_32 fun_uart_deint;

	//uecom��������ָ�룬21��
//	uint_32 fun_uecom_init;
//	uint_32 fun_uecom_fast_init;
//	uint_32 fun_uecom_deInit;
//	uint_32 fun_uecom_switch;
//	uint_32 fun_uecom_send;
//	uint_32 fun_uecom_getIMEI;
//	uint_32 fun_uecom_getIMSI;
//	uint_32 fun_uecom_getFirmVer;
//	uint_32 fun_uecom_getSignalPower;
//	uint_32 fun_uecom_getBSNum;
//	uint_32 fun_uecom_getUEIP;
//	uint_32 fun_uecom_config;
//	uint_32 fun_uecom_getCurCommunity;
//	uint_32 fun_http_get;
//	uint_32 fun_uecom_interrupt;
//	uint_32 fun_uecom_default_delay_ms;
//	uint_32 fun_gnss_init;
//	uint_32 fun_gnss_get_time;
//	uint_32 fun_gnss_get_speed;
//	uint_32 fun_gnss_get_location;
//	uint_32 fun_gnss_get_attitude;
};
#pragma  pack(pop)

//��3��������ʵ����Ҫ��ɾ��ȫ�ֱ���������һ�ɼ�ǰ׺G_VAR_PREFIX������һ����������
//    ȫ�ֱ�������һ����g��ͷ�������ܱ�֤ȫ������Ψһ�ԣ�����һ��Ҫע���京��
//    ������ʱ��׼��ֵ��
G_VAR_PREFIX uint_8 gFunFlag;      //���������ܱ�ʶ
G_VAR_PREFIX uint_8 gRecvBuf[512]; //���ڽ������ݻ�����
G_VAR_PREFIX uint_16 gRecvLen;     //���ڽ������ݳ���
G_VAR_PREFIX uint_64 gTime;

//��4��������ʵ����Ҫ��ɾ��ȫ��ʹ�õĺ궨�峣����ʹ��ȫ��д
#define USERBASE 0x7800  //�û�����λ�� 30������ʼ
#define UPDATEADDRESS  0xBC00  //���´����ַ
#define SECTORNO 29          //����ָ��͸���״̬��������

#endif
