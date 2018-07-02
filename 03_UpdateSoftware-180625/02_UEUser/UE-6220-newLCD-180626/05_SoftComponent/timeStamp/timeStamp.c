#include "timeStamp.h"

//=====================================================================
//�������ƣ�timeChange
//�������أ���
//����˵����ticks��ʱ���
//       time������������ڣ���:19700101000000(1970-01-01 00:00:00)
//���ܸ�Ҫ��ʱ���ת���ַ���ʾ����
//��ע��KDS��KEIL���ڲ�ʹ�ú�����һ��
//=====================================================================
void timeChange(uint_64 ticks,uint_8 *time)
{
	struct tm* curTime;
	uint_8 s[100],i;
	time_t t;
	
	t = (time_t)ticks;
	curTime = gmtime(&t);          //KDS��֧��localtime
//	curTime = localtime(&t);			//KEIL�MMDK��֧��gmtime

	strftime((char *)s,sizeof(s),"%Y%m%d%H%M%S",curTime);

	for(i = 0;i < 14;i++)
	{
		time[i] = s[i] - '0';
	}
	
}
