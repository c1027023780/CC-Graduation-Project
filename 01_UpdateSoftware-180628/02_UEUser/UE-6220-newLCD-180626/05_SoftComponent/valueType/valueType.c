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
//                   radix���������ֵĽ���
//���ܸ�Ҫ����ָ�����Ƶ�������תΪ�ַ���
//=====================================================================
uint_8 *myItoa(uint_32 num,uint_8 *str,uint_8 radix)
{
	//��1����������ʼ������
	uint_8 index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	uint_32 unum; /* �м���� */
	uint_8 i=0,j,k;
	// ��2��ȷ��unum��ֵ
	if(radix==10&&num<0) //��Ϊʮ���Ƹ���/
	{
	    unum=(unsigned)-num;
	    str[i++]='-';
	}
	else                                  //�������
		unum=(unsigned)num;
	//��3������
	do {
	    str[i++]=index[unum%(unsigned)radix];
	    unum/=radix;
	}while(unum);
	str[i]='\0';
	//��4��ת��
	if(str[0]=='-') k=1; /* ʮ���Ƹ��� */
	else k=0;
	//��5�� ��ԭ����"/2"��Ϊ"/2.0",��֤��num��16~255֮�䣬radix����16ʱ��Ҳ�ܵõ���ȷ���
	for(j=k;j<=(i-1)/2.0+k;j++)
	{
	    num=str[j];
	    str[j]=str[i-j-1+k];
	    str[i-j-1+k]=num;
	}
	return str;
}

//doubleת�ַ���
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
    // �������ִ���
    myItoa(int_part,str_int,10);
    // С�����ִ���
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
