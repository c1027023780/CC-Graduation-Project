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
//功能概要：将指定进制的整型数转为字符串
//=====================================================================
uint_8 *myItoa(int_32 num,uint_8 *str)
{
	//（1）声明使用到的变量
	uint_8 length,i,j;
	int_32 tmp_32;
	uint_8 tmp[10];
	length = 0;
	//（2）判断是否为负数
	if(num<0)str[length++]='-';
	//（3）获得每一位数的ASCII字符并保存在tmp
	tmp_32 = num;
	i=0;
	while(tmp_32)
	{
	    tmp[i++]=tmp_32%10+'0';
	    tmp_32/=10;
	}
	//（4）将tmp中的数据按照正确的顺序放入数组str中，并添加结束符
	for(j=0;j<i;j++)str[length++]=tmp[i-j-1];
	str[length]=0;
	return str;
}

//double转字符串
uint_8 *myFtoa(double number,int_32 ndigit,uint_8 *buf)
{
	//（1）声明使用到的变量
	uint_16 tmp_16;
	int_32 iNumber;
	int_32 tmp_32;
	//（2）获得指定保存小数点后，应显示的数字.
	//（若传入浮点数4.12，并保留两位小数，则会得到数字412）
	tmp_16 = 1;
	tmp_32 = ndigit;
	while(tmp_32--)tmp_16*=10;
	iNumber = number*tmp_16;
	myItoa(iNumber,buf);
	//（3）将小数点插入正确的位置，并添加结束符
	tmp_32 = ndigit;
	tmp_16 = 0;
	while(buf[tmp_16])tmp_16++;
	buf[tmp_16+1]=0;
	while(tmp_32--)buf[tmp_16]=buf[--tmp_16];
	buf[tmp_16]='.';
    return buf;
}

//字符串复制
void copyN(uint_8 * dest,uint_8*source,uint_16 size)
{
	uint_16 i = 0;
	for(i=0;i<size;i++)
		dest[i]=source[i];
}
