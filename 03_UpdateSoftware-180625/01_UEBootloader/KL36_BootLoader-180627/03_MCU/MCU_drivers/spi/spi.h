//=====================================================================
//�ļ����ƣ�SPI.h
//���ܸ�Ҫ��SPIͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2016-03-17 
//=====================================================================
#ifndef _SPI_H             //��ֹ�ظ����壨 ��ͷ)
#define _SPI_H

#include "common.h"      //��������Ҫ��ͷ�ļ�
//�궨�壺����SPI�ںš�
#define SPI_0  0     //SPI0��
#define SPI_1  1     //SPI1��

//����SPIʵ��Ӳ�����ţ�ȷ�����º곣��ֵ��
//�ڴ˹����У�����ֻʹ��SPI0���еĵ�2����SPI1���еĵ�2����
//��������ڴ�ֻ��Ҫ��SPI_0_GROUP�궨��Ϊ2,SPI_1_GROUP�궨��Ϊ2��

//SPI_0��1=PTD0��1��2��3�ţ�2=PTA14��15��16��17�ţ�3=PTC4��5��6��7��
#define SPI_0_GROUP  3   //SD-FSL-KL25-EVB����ʹ��PTA14~17��

//SPI_1��1=PTE1��2��3��4�ţ�2=PTB10��11��16��17�ţ�3=PTD4��5��6��7��
#define SPI_1_GROUP  3    //SD-FSL-KL25-EVB����ʹ��PTB10��11��16��17��


#define SPI_baseadd(SPI_nub) (SPI_MemMapPtr)(0x40076000u+SPI_nub*0x00001000u)
//=====================================================================
//�������ƣ�SPI_init��
//����˵����SPI��ʼ��
//����������No��ģ��ţ�KL25оƬȡֵΪ0��1
//       MSTR��SPI���ӻ�ѡ��0ѡ��Ϊ����,1ѡ��Ϊ�ӻ���
//       BaudRate�������ʣ���ȡ12000��6000��4000��3000��1500��1000����λ��b/s
//       CPOL��CPOL=0������ЧSPIʱ�ӣ�����Ч����CPOL=1������ЧSPIʱ�ӣ�����Ч��
//       CPHA��CPHA=0��λΪ0�� CPHA=1��λΪ1��
//�������أ���
//������ע�� CPHA=0��ʱ���źŵĵ�һ�����س�����8�������ݴ���ĵ�һ�����ڵ����룻
//        CPHA=1��ʱ���źŵĵ�һ�����س�����8�������ݴ���ĵ�һ�����ڵ���㡣
//        CPHA=0ʱ��ͨ�����ȶ��������շ���1/2���ڵ�ʱ�̴�����ȡ����
//=====================================================================
void SPI_init(uint_8 No,uint_8 MSTR,uint_16 BaudRate,uint_8 CPOL,\
                                                         uint_8 CPHA);

//=====================================================================
//�������ƣ�SPI_send1.
//����˵����SPI����һ�ֽ����ݡ�
//����������No��ģ��š� ��ȡֵΪ0��1
//       data��     ��Ҫ���͵�һ�ֽ����ݡ�
//�������أ�0������ʧ�ܣ�1�����ͳɹ���
//=====================================================================
uint_8 SPI_send1(uint_8 No,uint_8 data);

//=====================================================================
//�������ƣ�SPI_sendN.
//����˵����SPI�������ݡ�
//����������No��ģ��š� ��ȡֵΪ0��1
//       n:     Ҫ���͵��ֽڸ�������ΧΪ(1~255)
//       data[]:����������׵�ַ��
//�������أ��ޡ�
//=====================================================================
void SPI_sendN(uint_8 No,uint_8 n,uint_8 data[]);

//=====================================================================
//�������ƣ�SPI_receive1.
//����˵����SPI����һ���ֽڵ�����
//����������No��ģ��š� ��ȡֵΪ0��1
//�������أ����յ������ݡ�
//=====================================================================
uint_8 SPI_receive1(uint_8 No);

//=====================================================================
//�������ƣ�SPI_receiveN.
//����˵����SPI�������ݡ���n=1ʱ�����ǽ���һ���ֽڵ����ݡ���
//����������No��ģ��š� ��ȡֵΪ0��1
//          n:    Ҫ���͵��ֽڸ�������ΧΪ(1~255),
//       data[]:���յ������ݴ�ŵ��׵�ַ��
//�������أ�1�����ճɹ�,���������ʧ�ܡ�
//=====================================================================
uint_8 SPI_receiveN(uint_8 No,uint_8 n,uint_8 data[]);

//=====================================================================
//�������ƣ�SPI_enable_re_int
//����˵������SPI�����жϡ�
//����������No��ģ��š� ��ȡֵΪ0��1��
//�������أ��ޡ�
//=====================================================================
void SPI_enable_re_int(uint_8 No);

//=====================================================================
//�������ƣ�SPI_disable_re_int
//����˵�����ر�SPI�����жϡ�
//����������No��ģ��š� ��ȡֵΪ0��1��
//�������أ��ޡ�
//=====================================================================
void SPI_disable_re_int(uint_8 No);

#endif   //��ֹ�ظ����壨 ��β)

//=====================================================================
//������
//��1�����ǿ�����Դ���룬�ڱ������ṩ��Ӳ��ϵͳ����ͨ������Ϸ��׸���ᣬ����֮����
//     ��ӭָ����
//��2������ʹ�÷Ǳ�����Ӳ��ϵͳ���û�����ֲ����ʱ������ϸ�����Լ���Ӳ��ƥ�䡣
//
//���ݴ�ѧNXPǶ��ʽ����(http://sumcu.suda.edu.cn,0512-65214835)