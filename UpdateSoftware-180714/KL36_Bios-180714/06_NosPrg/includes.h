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
#include "i2c.h"
#include "lptmr.h"
#include "spi.h"
#include "systick.h"
#include "tsi.h"
#include "uart.h"
#include "bootloader.h"
#include "light.h"
#include "uecom.h"
#include "ueupdate.h"
#include "timeStamp.h"


//��2����������ȫ�ֱ���һ�������ദʹ�ô����������
//    Ϊ��������ȫ�ֱ�����ʹ��ǰ׺G_VAR_PREFIX�궨����׼��
//    GLOBLE_VAR��main.c���к궨��
#ifdef GLOBLE_VAR		 //main.c�ļ��а�����ͷ�ļ�ʱ��ȫ�ֱ������ӡ�extern��ǰ׺
#define G_VAR_PREFIX
#else                    //�����ļ��а�����ͷ�ļ�ʱ��ȫ�ֱ����Զ��ӡ�extern��ǰ׺
#define G_VAR_PREFIX extern
#endif


//��3��������ʵ����Ҫ��ɾ��ȫ�ֱ���������һ�ɼ�ǰ׺G_VAR_PREFIX������һ����������
//    ȫ�ֱ�������һ����g��ͷ�������ܱ�֤ȫ������Ψһ�ԣ�����һ��Ҫע���京��
//    ������ʱ��׼��ֵ��
G_VAR_PREFIX vuint_8 gFunFlag;      //���������ܱ�ʶ
G_VAR_PREFIX uint_8 gRecvBuf[500]; //���ڽ������ݻ�����
G_VAR_PREFIX uint_16 gRecvLen;     //���ڽ������ݳ���
G_VAR_PREFIX uint_64 gTime;

//��4��������ʵ����Ҫ��ɾ��ȫ��ʹ�õĺ궨�峣����ʹ��ȫ��д
#define SECTORNO 25                 //����ָ��͸���״̬��������
#define USERBASE (SECTORNO+1)*1024  //�û�����λ�� 26������ʼ
#define UPDATEADDRESS 0x200003F8

#endif
