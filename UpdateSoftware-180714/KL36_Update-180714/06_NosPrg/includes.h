//======================================================================
//�ļ����ƣ�includes.h
//���ܸ�Ҫ��Ӧ�ù�����ͷ�ļ�
//��Ȩ���У����ݴ�ѧǶ��ʽϵͳ���������о���(sumcu.suda.edu.cn)
//�汾���£�2017.2.1  V1.0��2018.1.6  V2.2��WYH��
//������������1�������õ��Ĺ�������2��ȫ�ֱ���һ�������ദʹ�ô�����������Σ�
//       ��3��ȫ�ֱ�����������4�������궨��
//======================================================================

#ifndef _INCLUDES_H    //��ֹ�ظ����壨��ͷ��
#define _INCLUDES_H

//��1��������ʵ����Ҫ��ɾ�������õ��Ĺ���
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "adc.h"
#include "flash.h"
#include "gpio.h"
#include "i2c.h"
#include "lptmr.h"
#include "spi.h"
#include "tsi.h"
#include "uart.h"
#include "wdog.h"
#include "lcd.h"
#include "light.h"
#include "uecom.h"
#include "valueType.h"

//��2����������ȫ�ֱ���һ�������ദʹ�ô������������
//Ϊ��������ȫ�ֱ�����ʹ��ǰ׺G_VAR_PREFIX�궨����׼����GLOBLE_VAR��main.c���к궨��
#ifdef GLOBLE_VAR   //main.c�ļ��а�����ͷ�ļ�ʱ��ȫ�ֱ������ӡ�extern��ǰ׺
  #define G_VAR_PREFIX
#else      //�����ļ��а�����ͷ�ļ�ʱ��ȫ�ֱ����Զ��ӡ�extern��ǰ׺
  #define G_VAR_PREFIX extern
#endif

//��3��������ʵ����Ҫ��ɾ��ȫ�ֱ�����������һ�ɼ�ǰ׺G_VAR_PREFIX������һ����������
//    ȫ�ֱ�������һ����g��ͷ�������ܱ�֤ȫ������Ψһ�ԣ�����һ��Ҫע���京��
//    ������ʱ��׼��ֵ��
//G_VAR_PREFIX struct comData gComData;  //ͨ��֡��comData�ṹ���ڱ��ļ��ж��壩
G_VAR_PREFIX struct UserData gUserData;  //�û���Ϣ֡��comData�ṹ���ڱ��ļ��ж��壩
G_VAR_PREFIX struct FlashData gFlashData;  //��Ҫд��flash�е�����
G_VAR_PREFIX uint_8 gTime[14];    //ʱ���ת���������ʱ��"20170927010021"
G_VAR_PREFIX uint_8 gRecvBuf[1024];  //�յ������ݻ�����
G_VAR_PREFIX uint_16 gRecvLength;    //�յ������ݳ��ȣ�Ϊ0ʱ��ʾû���յ�����

//��4��������ʵ����Ҫ��ɾ��ȫ��ʹ�õĺ곣����ʹ��ȫ��д
#define SEND_TIME   10           //����ʱ�䣨�ֵ���������
#define WAIT_RECV_SECONDS   35   //�������ݵĵȴ��������ֵ������ʱ����δ���յ��������UEģ��ϵ�
#define TSI_VALUE  0x00CC0011    //������Ӧ��ֵ��ʵ���ã�
#define SECTOR_NO  50            //Flash������
#define OFFSET     0             //Flash������ƫ��
#define CNT sizeof(struct comData)    //ͨ��֡�ֽ���
//#define ENABLE_WDOG	   1       //�����Ƿ������Ź�

#define USERSECT 26

#pragma  pack(push,1)   //ȡ��4�ֽڶ���
//�ն�UE���˻�����ϵͳHCI��ͨ��֡�ṹ��comData����
//�洢����«��صĻ�����Ϣ
//����������¼���«Ӳ�������������Ϣ��������flash�е���Ϣ
struct FlashData
{
	//��1����Ʒ��Ϣ
	uint_8 equipName[30];       //��Ʒ����
	uint_8 equipID[20];              //��Ʒ���к�
	uint_8 equipType[20];         //��Ʒ�ͺ�
	uint_8 vendor[30];               //��������
	uint_8 softVer[4];                  //�����汾
	uint_64 productTime;         //����ʱ�䡣��64λʱ�������תΪ������ʱ���룩
	uint_8 userName[20];        //�û���
	uint_8 phone[11];              //�ֻ���
	//��2����������Ϣ
	uint_8 serverIP[15];             //������IP
	uint_8 serverPort[5];            //�������˿ں�
    //��3���û�����flash����Ϣ
	uint_32 sendFrequencySec;   //����Ƶ��
	uint_32 resetCount;                  //��λ����
	uint_8   frameCmd[2];             //��ǰ�����ݸ�ʽ
};
//������ʵ����Ҫ��ɾ���洢�û��ɼ�������
struct UserData
{
	//����������Щ����Ϊ����Ҫ�еı���
    uint_8 cmd[2];        //����
	uint_16 sn;              //֡��
	uint_8 IMSI[15];         //IMSI��
	uint_8 serverIP[15];     //������IP
	uint_8 serverPort[5];    //�������˿�
	uint_64 currentTime;     //����ʱ�䣨ʹ��64λʱ�������ת��Ϊ������ʱ���룩
	uint_32 resetCount;
	uint_32 sendFrequencySec;
	//������ʵ����Ҫ��ɾ���˴��ɰ���Ҫ��ɾ������ע�����������֡�ṹ����һ��
	uint_8 userName[20];     //�û���
	uint_8 softVer[4];       //�����汾
	uint_8 equipName[30];    //��Ʒ����
	uint_8 equipID[20];      //��Ʒ���к�
	uint_8 equipType[20];    //��Ʒ����
	uint_8 vendor[30];       //��������
	int_32 mcuTemp;          //оƬ�¶�
	uint_8 surpBaseInfo[20]; //��ע1
	uint_8 phone[11];        //�ֻ���
	uint_8 IMEI[15];         //IMEI��
	uint_8 signalPower;      //�ź�ǿ��
	uint_16 bright;          //�������ȣ�����������A/Dֵ��
	uint_16 touchNum;        //��������������TSI��
	uint_8 surplusInfo[20];  //��ע2
	uint_8 lbs_location[25];
	//����ư��1��
	//�˴��������û��Զ�������


//	//����ʱֱ�ӻ�ȡ������
//	uint_8 IMSI[15];           //IMSI��
//	uint_64 currentTime;   //��ǰʱ�䣨ʹ��64λʱ�������ת��Ϊ������ʱ���룩
//	int_32 mcuTemp;        //оƬ�¶�
//	uint_8 signalPower;     //�ź�ǿ��
//	uint_16 bright;            //�������ȣ�����������A/Dֵ��
//	uint_16 touchNum;     //��������������TSI��
//	uint_8 lbs_location[25];  //�洢��վ��Ϣ
};
#pragma  pack(pop)     //�ָ�4�ֽڶ���

#endif                  //��ֹ�ظ����壨��β��

/*
 ֪ʶҪ�أ�
 ��1�����ļ�����������룬һ��Ϊ������ʵ����Ҫ��ɾ�������������ݾ���
 ��Ŀ������и��ģ���һ��Ϊ������������������������Ŀ�޹أ���ͨ�õģ�
 ������ݾ�����Ŀ���и��ġ�
 ��2��ȫ�ֱ���һ�������ദʹ�ô��������������Ϊ�˽��ȫ�ֱ����ظ�����
 �����⣬��GLOBLE_VAR����main.c������������ֻ����main.c�а���
 ��ͷ�ļ�ʱ�Ż�����ȫ�ֱ������������ļ��а������ļ�ʱ������ʱ���ͻ��Զ���
 ��ǰ׺extern��������main������������ȫ�ֱ��������ļ���ֱ��ʹ�á�
 ��3������ȫ�ֱ���ʱ��һ�ɼ�ǰ׺G_VAR_PREFIX����ֻ�ڱ��ļ�
 �ġ�ȫ�ֱ����������������� ���ڴ�һ���������ɡ� ȫ�ֱ�������һ����g��ͷ��
 �����ܱ�֤ȫ������Ψһ�ԣ�����һ��Ҫע���京�壨����ʱ��׼��ֵ����
 ��4��ȫ��ʹ�õĺ곣����ʹ��ȫ��д
 */