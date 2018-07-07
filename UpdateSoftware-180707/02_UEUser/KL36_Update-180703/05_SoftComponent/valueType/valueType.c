/*
 * valueType.c
 *
 *  Created on: 2018��5��9��
 *      Author: 13341
 */
#include "valueType.h"
//=====================================================================
//�������ƣ�myItoa
//�������أ���
//����˵����num����ת��Ϊ�ַ��������֣�str���ַ���ת����ɺ�洢��λ�ã�
//���ܸ�Ҫ����ָ�����Ƶ�������תΪ�ַ���
//=====================================================================
uint_8 *myItoa(int_32 num,uint_8 *str)
{
	//��1������ʹ�õ��ı���
	uint_8 length,i,j;
	int_32 tmp_32;
	uint_8 tmp[10];
	length = 0;
	//��2���ж��Ƿ�Ϊ����
	if(num<0)str[length++]='-';
	//��3�����ÿһλ����ASCII�ַ���������tmp
	tmp_32 = num;
	i=0;
	while(tmp_32)
	{
	    tmp[i++]=tmp_32%10+'0';
	    tmp_32/=10;
	}
	//��4����tmp�е����ݰ�����ȷ��˳���������str�У�����ӽ�����
	for(j=0;j<i;j++)str[length++]=tmp[i-j-1];
	str[length]=0;
	return str;
}

//doubleת�ַ���
uint_8 *myFtoa(double number,int_32 ndigit,uint_8 *buf)
{
	//��1������ʹ�õ��ı���
	uint_16 tmp_16;
	int_32 iNumber;
	int_32 tmp_32;
	//��2�����ָ������С�����Ӧ��ʾ������.
	//�������븡����4.12����������λС�������õ�����412��
	tmp_16 = 1;
	tmp_32 = ndigit;
	while(tmp_32--)tmp_16*=10;
	iNumber = number*tmp_16;
	myItoa(iNumber,buf);
	//��3����С���������ȷ��λ�ã�����ӽ�����
	tmp_32 = ndigit;
	tmp_16 = 0;
	while(buf[tmp_16])tmp_16++;
	buf[tmp_16+1]=0;
	while(tmp_32--)buf[tmp_16]=buf[--tmp_16];
	buf[tmp_16]='.';
    return buf;
}

//�ַ�������
void copyN(uint_8 * dest,uint_8*source,uint_16 size)
{
	uint_16 i = 0;
	for(i=0;i<size;i++)
		dest[i]=source[i];
}
