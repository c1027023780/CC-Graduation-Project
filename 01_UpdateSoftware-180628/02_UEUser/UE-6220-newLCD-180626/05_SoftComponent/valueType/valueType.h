/*
 * valueType.h
 *
 *  Created on: 2018��5��9��
 *      Author: 13341
 */
#ifndef _VALUETYPE_H
#define _VALUETYPE_H

#include "common.h"
#include <string.h>
#include <malloc.h>
#include <math.h>

extern uint_8 *myItoa(uint_32 num,uint_8 *str,uint_8 radix);        //������ת�ַ������¹ٷ�itoa
extern uint_8 *myFtoa(double number,int_32 ndigit,uint_8 *buf);        //doubleת�ַ���

#endif
