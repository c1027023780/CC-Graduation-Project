#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include "common.h"

//=====================================================================
//�������ƣ�timeChange
//�������أ���
//����˵����ticks��ʱ���
//       time������������ڣ���:19700101000000(1970-01-01 00:00:00)
//���ܸ�Ҫ��ʱ���ת���ַ���ʾ����
//=====================================================================
void timeChange(uint_64 ticks,uint_8 *time);

#endif