/*
 * valueType.h
 *
 *  Created on: 2018��5��9��
 *      Author: 13341
 */
#ifndef _VALUETYPE_H
#define _VALUETYPE_H

#include "common.h"

//=====================================================================
//�������ƣ�myItoa
//�������أ���
//����˵����num����ת��Ϊ�ַ��������֣�str���ַ���ת����ɺ�洢��λ�ã�
//���ܸ�Ҫ����ָ�����Ƶ�������תΪ�ַ���
//=====================================================================
uint_8 *myItoa(int_32 num,uint_8 *str);

uint_8 *myFtoa(double number,int_32 ndigit,uint_8 *buf);        //doubleת�ַ���
//�ַ�������
void copyN(uint_8 * dest,uint_8*source,uint_16 size);
#endif
