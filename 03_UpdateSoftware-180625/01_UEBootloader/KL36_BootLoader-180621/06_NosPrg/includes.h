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
#include "gpio.h"
#include "uart.h"
#include "systick.h"
#include "flash.h"
#include "light.h"
#include "bootloader.h"
#include "ueupdate.h"
#include "string.h"
#include "printf.h"


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
	//flash����ָ�룬7��
	uint_32 fun_flash_init;
	uint_32 fun_flash_erase;
	uint_32 fun_flash_write;
	uint_32 fun_flash_read_logic;
	uint_32 fun_flash_read_physical;
	uint_32 fun_flash_protect;
	uint_32 fun_flash_isempty;

	//light����ָ�룬3��
	uint_32 fun_light_init;
	uint_32 fun_light_control;
	uint_32 fun_light_change;

	uint_32 fun_getTime;
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
