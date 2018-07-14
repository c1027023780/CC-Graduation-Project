/*
 * valueType.h
 *
 *  Created on: 2018年5月9日
 *      Author: 13341
 */
#ifndef _VALUETYPE_H
#define _VALUETYPE_H

#include "common.h"

//=====================================================================
//函数名称：myItoa
//函数返回：无
//参数说明：num：待转换为字符串的数字；str：字符串转换完成后存储的位置；
//功能概要：将指定进制的整型数转为字符串
//=====================================================================
uint_8 *myItoa(int_32 num,uint_8 *str);

uint_8 *myFtoa(double number,int_32 ndigit,uint_8 *buf);        //double转字符串
//字符串复制
void copyN(uint_8 * dest,uint_8*source,uint_16 size);
#endif
