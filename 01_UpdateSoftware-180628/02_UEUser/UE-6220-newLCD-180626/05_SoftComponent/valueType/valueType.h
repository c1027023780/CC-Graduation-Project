/*
 * valueType.h
 *
 *  Created on: 2018年5月9日
 *      Author: 13341
 */
#ifndef _VALUETYPE_H
#define _VALUETYPE_H

#include "common.h"
#include <string.h>
#include <malloc.h>
#include <math.h>

extern uint_8 *myItoa(uint_32 num,uint_8 *str,uint_8 radix);        //整型数转字符串，仿官方itoa
extern uint_8 *myFtoa(double number,int_32 ndigit,uint_8 *buf);        //double转字符串

#endif
