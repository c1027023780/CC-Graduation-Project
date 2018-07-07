/*
 * valueType.c
 *
 *  Created on: 2018年5月9日
 *      Author: 13341
 */
#include "valueType.h"
//=====================================================================
//函数名称：myItoa
//函数返回：无
//参数说明：num：待转换为字符串的数字；str：字符串转换完成后存储的位置；
//                   radix：传入数字的进制
//功能概要：将指定进制的整型数转为字符串
//=====================================================================
uint_8 *myItoa(uint_32 num,uint_8 *str,uint_8 radix)
{
	//（1）创建并初始化变量
	uint_8 index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	uint_32 unum; /* 中间变量 */
	uint_8 i=0,j,k;
	// （2）确定unum的值
	if(radix==10&&num<0) //若为十进制负数/
	{
	    unum=(unsigned)-num;
	    str[i++]='-';
	}
	else                                  //其他情况
		unum=(unsigned)num;
	//（3）逆序
	do {
	    str[i++]=index[unum%(unsigned)radix];
	    unum/=radix;
	}while(unum);
	str[i]='\0';
	//（4）转换
	if(str[0]=='-') k=1; /* 十进制负数 */
	else k=0;
	//（5） 将原来的"/2"改为"/2.0",保证当num在16~255之间，radix等于16时，也能得到正确结果
	for(j=k;j<=(i-1)/2.0+k;j++)
	{
	    num=str[j];
	    str[j]=str[i-j-1+k];
	    str[i-j-1+k]=num;
	}
	return str;
}

//double转字符串
uint_8 *myFtoa(double number,int_32 ndigit,uint_8 *buf)
{
    int_64 int_part;
    double float_part;
    uint_8 str_int[32];
    uint_8 str_float[32];
    memset(str_int,0,32);
    memset(str_float,0,32);
    int_part = (int_64)number;
    float_part = number - int_part;
    // 整数部分处理
    myItoa(int_part,str_int,10);
    // 小数部分处理
    if(ndigit>0)
    {
        float_part =fabs( pow(10,ndigit)*float_part);
        myItoa((int_64)float_part,str_float,10);
    }
    int_32 i = strlen(str_int);
    str_int[i] = '.';
    strcat(str_int,str_float);
    strcpy(buf,str_int);
    return buf;
}
