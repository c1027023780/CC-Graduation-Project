//======================================================================
//�ļ����ƣ�uart.h
//���ܸ�Ҫ��UART�ײ���������ͷ�ļ�
//��Ȩ���У����ݴ�ѧǶ��ʽϵͳ���������о���(sumcu.suda.edu.cn)
//���¼�¼��2017-04-19 V1.0
//����оƬ��KL36(uart0��δʵ��)
//======================================================================

#ifndef _UART_H         //��ֹ�ظ����壨 ��ͷ)
#define _UART_H

#include "common.h"    //��������Ҫ��ͷ�ļ�

//�궨�崮�ں�
#define UART_0    0
#define UART_1    1
#define UART_2    2
//����UARTxʹ�õ�������(TX,RX)
//UART_0�����������ã�0:PTE20~21, 1:PTA2~1, 2:PTB17~16, 3:PTD7~6
#define UART_0_GROUP    0
//UART_1�����������ã�0:PTE0~1, 1:PTA19~18, 2:PTC4~3
#define UART_1_GROUP    0                         //��BC95����
//UART_2�����������ã�0:PTE16~17, 1:PTE22~23, 2:PTD3~2, 3:PTD5~4
#define UART_2_GROUP    0

//======================================================================
//�������ƣ�uart_init
//���ܸ�Ҫ����ʼ��uartģ��                                                
//����˵����uartNo:���ںţ�UART_0��UART_1��UART_2 
//          baud:�����ʣ�300��600��1200��2400��4800��9600��19200��115200...
//�������أ���
//======================================================================
//void uart_init(uint_8 uartNo, uint_32 baud_rate);
typedef  void (*fun_uart_init)(uint_8 uartNo, uint_32 baud_rate);
volatile fun_uart_init uart_init;

//======================================================================
//�������ƣ�uart_send1
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2 
//          ch:Ҫ���͵��ֽ�
//�������أ�����ִ��״̬��1=���ͳɹ���0=����ʧ�ܡ�
//���ܸ�Ҫ�����з���1���ֽ�
//======================================================================
//uint_8 uart_send1(uint_8 uartNo, uint_8 ch);
typedef  uint_8 (*fun_uart_send1)(uint_8 uartNo, uint_8 ch);
volatile fun_uart_send1 uart_send1;

//======================================================================
//�������ƣ�uart_sendN
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2 
//         buff: ���ͻ�����
//         len:���ͳ���
//�������أ� ����ִ��״̬��1=���ͳɹ���0=����ʧ��
//���ܸ�Ҫ������ ����n���ֽ�
//======================================================================
//uint_8 uart_sendN(uint_8 uartNo ,uint_16 len ,uint_8* buff);
typedef  uint_8 (*fun_uart_sendN)(uint_8 uartNo ,uint_16 len ,uint_8* buff);
volatile fun_uart_sendN uart_sendN;

//======================================================================
//�������ƣ�uart_send_string
//����˵����uartNo:UARTģ���:UART_0��UART_1��UART_2 
//          buff:Ҫ���͵��ַ������׵�ַ
//�������أ� ����ִ��״̬��1=���ͳɹ���0=����ʧ��
//���ܸ�Ҫ����ָ��UART�˿ڷ���һ����'\0'�������ַ���
//======================================================================
//uint_8 uart_send_string(uint_8 uartNo, void *buff);
typedef  uint_8 (*fun_uart_send_string)(uint_8 uartNo, void *buff);
volatile fun_uart_send_string uart_send_string;

//======================================================================
//�������ƣ�uart_re1
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2 
//        *fp:���ճɹ���־��ָ��:*fp=1:���ճɹ���*fp=0:����ʧ��
//�������أ����շ����ֽ�
//���ܸ�Ҫ�����н���1���ֽ�
//======================================================================
//uint_8 uart_re1(uint_8 uartNo,uint_8 *fp);
typedef  uint_8 (*fun_uart_re1)(uint_8 uartNo,uint_8 *fp);
volatile fun_uart_re1 uart_re1;

//======================================================================
//�������ƣ�uart_reN
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2
//          buff: ���ջ�����
//          len:���ճ���
//�������أ�����ִ��״̬ 1=���ճɹ�;0=����ʧ��
//���ܸ�Ҫ������ ����n���ֽ�,����buff��
//======================================================================
//uint_8 uart_reN(uint_8 uartNo ,uint_16 len ,uint_8* buff);
typedef  uint_8 (*fun_uart_reN)(uint_8 uartNo ,uint_16 len ,uint_8* buff);
volatile fun_uart_reN uart_reN;

//======================================================================
//�������ƣ�uart_enable_re_int
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2 
//�������أ���
//���ܸ�Ҫ�������ڽ����ж�
//======================================================================
//void uart_enable_re_int(uint_8 uartNo);
typedef  void (*fun_uart_enable_re_int)(uint_8 uartNo);
volatile fun_uart_enable_re_int uart_enable_re_int;

//======================================================================
//�������ƣ�uart_disable_re_int
//����˵����uartNo: ���ں� :UART_0��UART_1��UART_2 
//�������أ���
//���ܸ�Ҫ���ش��ڽ����ж�
//======================================================================
//void uart_disable_re_int(uint_8 uartNo);
typedef  void (*fun_uart_disable_re_int)(uint_8 uartNo);
volatile fun_uart_disable_re_int uart_disable_re_int;

//======================================================================
//�������ƣ�uart_get_re_int
//����˵����uartNo: ���ں� :UART_0��UART_1��UART_2
//�������أ������жϱ�־ 1=�н����ж�;0=�޽����ж�
//���ܸ�Ҫ����ȡ���ڽ����жϱ�־,ͬʱ���÷����ж�
//======================================================================
//uint_8 uart_get_re_int(uint_8 uartNo);
typedef  uint_8 (*fun_uart_get_re_int)(uint_8 uartNo);
volatile fun_uart_get_re_int uart_get_re_int;

//void uart_deint(uint_8 uartNo);
typedef  void (*fun_uart_deint)(uint_8 uartNo);
volatile fun_uart_deint uart_deint;

#endif     //��ֹ�ظ����壨 ��β��

//=====================================================================
//������
//��1�����ǿ�����Դ���룬�ڱ������ṩ��Ӳ��ϵͳ����ͨ������Ϸ��׸���ᣬ
//    ����֮������ӭָ����
//��2������ʹ�÷Ǳ�����Ӳ��ϵͳ���û�����ֲ����ʱ������ϸ�����Լ���Ӳ��ƥ�䡣
//
//���ݴ�ѧǶ��ʽϵͳ���������о�����0512-65214835��http://sumcu.suda.edu.cn
