//======================================================================
//�ļ����ƣ�bootloader.h
//���ܸ�Ҫ��bootloader����ͷ�ļ�
//������λ�����ݴ�ѧNXPǶ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2018-01-19,CC
//======================================================================

#ifndef _BOOTLOADER_H     //��ֹ�ظ����壨_LIGHT_H  ��ͷ)
#define _BOOTLOADER_H

//ͷ�ļ�����
#include "common.h"

//#define USERBASE    0x2000  //�û������ַ
//#define UPDATEBASE  0x9000  //���´����ַ

void bootloader_goToUserApp(volatile uint_32 app_start_addr);

#endif
