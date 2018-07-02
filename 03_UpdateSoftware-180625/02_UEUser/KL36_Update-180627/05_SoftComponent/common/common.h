//=====================================================================
//�ļ����ƣ�common.h
//���ܸ�Ҫ������Ҫ��ͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2017-04-07   V1.0
//=====================================================================
#ifndef _COMMON_H    //��ֹ�ظ����壨_COMMON_H  ��ͷ)
#define _COMMON_H

//1��оƬ�Ĵ���ӳ���ļ����������ں������ļ�
#include "MKL36Z4.h"      // ����оƬͷ�ļ�
#include "core_cm0plus.h"
#include "core_cmFunc.h"
#include "core_cmInstr.h"
#include "system_MKL36Z4.h"

//2�����忪�����ж�
#define ENABLE_INTERRUPTS   __enable_irq()  //�����ж�
#define DISABLE_INTERRUPTS  __disable_irq()  //�����ж�

//3��λ�����꺯������λ����λ����üĴ���һλ��״̬��
#define BCLR(bit,Register)  ((Register) &= ~(1<<(bit)))  //��Ĵ�����һλ
#define BGET(bit,Register)  (((Register) >> (bit)) & 1)  //��üĴ���һλ��״̬
#define BSET(bit,Register)  ((Register) |= (1<<(bit)))    //�üĴ�����һλ

//4���ض�������������ͣ����ͱ����궨�壩
typedef unsigned char        uint_8;   // �޷���8λ�����ֽ�
typedef unsigned short int   uint_16;  // �޷���16λ������
typedef unsigned long int    uint_32;  // �޷���32λ��������
typedef unsigned long long   uint_64;
typedef char                 int_8;    // �з���8λ��
typedef short int            int_16;   // �з���16λ��
typedef int                  int_32;   // �з���32λ��
typedef long long            int_64;
//���Ż�����
typedef volatile uint_8      vuint_8;  // ���Ż��޷���8λ�����ֽ�
typedef volatile uint_16     vuint_16; // ���Ż��޷���16λ������
typedef volatile uint_32     vuint_32; // ���Ż��޷���32λ��������
typedef volatile int_8       vint_8;   // ���Ż��з���8λ��
typedef volatile int_16      vint_16;  // ���Ż��з���16λ��
typedef volatile int_32      vint_32;  // ���Ż��з���32λ��

//5. ����ϵͳʹ�õ�ʱ��Ƶ��
#define  SYSTEM_CLK_KHZ   SystemCoreClock/1000     // оƬϵͳʱ��Ƶ��(KHz)
#define  BUS_CLK_KHZ      SYSTEM_CLK_KHZ/2         // оƬ����ʱ��Ƶ��(KHz)

//============================================================================
//�������ƣ�enable_irq
//�������أ���
//����˵����irq��irq��, ����ģ����жϺŶ�����MKL25Z.h��
//���ܸ�Ҫ��ʹ��irq�ж�
//============================================================================
void enable_irq (IRQn_Type irq);

//============================================================================
//�������ƣ�disable_irq
//�������أ���
//����˵����irq��irq��, ����ģ����жϺŶ�����MKL25Z.h��
//���ܸ�Ҫ����ֹirq�ж�
//============================================================================
void disable_irq (IRQn_Type irq);

//======================================================================
//�������ƣ�Delay_us
//�������أ���
//����˵������
//���ܸ�Ҫ����ʱ - ΢�뼶
//======================================================================
void Delay_us(uint16_t u16us);

//======================================================================
//�������ƣ�Delay_us
//�������أ���
//����˵������
//���ܸ�Ҫ����ʱ - ���뼶
//======================================================================
void Delay_ms(uint16_t u16ms);

//===========================================================================
//�������ƣ�SecAdd1
//�������أ���
//����˵����*p:Ϊָ��һ��ʱ��������p[3]
//���ܸ�Ҫ���뵥Ԫ+1��������ʱ�ֵ�Ԫ��00:00:00-23:59:59)
//===========================================================================
void SecAdd1(uint_8 *p);

#pragma  pack(push,1)
struct funInfo{
	//bootloader�����ṩ�Ľӿں���
	uint_32 fun_systemTimeGet;

	//adc��������ָ�룬2��
	uint_32 fun_adc_init;
	uint_32 fun_adc_read;

	//flash����ָ�룬7��
	uint_32 fun_flash_init;
	uint_32 fun_flash_erase;
	uint_32 fun_flash_write;
	uint_32 fun_flash_read_logic;
	uint_32 fun_flash_read_physical;
	uint_32 fun_flash_protect;
	uint_32 fun_flash_isempty;

	//gpio��������ָ�룬11��
	uint_32 fun_gpio_init;
	uint_32 fun_gpio_set;
	uint_32 fun_gpio_get;
	uint_32 fun_gpio_reverse;
	uint_32 fun_gpio_pull;
	uint_32 fun_gpio_enable_int;
	uint_32 fun_gpio_disable_int;
	uint_32 fun_gpio_get_int;
	uint_32 fun_gpio_clear_int;
	uint_32 fun_gpio_clear_allint;
	uint_32 fun_gpio_drive_strength;

	//lptmr��������ָ�룬4��
	uint_32 fun_lptmr_init;
	uint_32 fun_lptmr_reset;
	uint_32 fun_enable_lptmr_int;
	uint_32 fun_disable_lptmr_int;

	//spi��������ָ�룬7��
	uint_32 fun_SPI_init;
	uint_32 fun_SPI_send1;
	uint_32 fun_SPI_sendN;
	uint_32 fun_SPI_receive1;
	uint_32 fun_SPI_receiveN;
	uint_32 fun_SPI_enable_re_int;
	uint_32 fun_SPI_disable_re_int;

	//tsi��������ָ�룬6��
	uint_32 fun_tsi_init;
	uint_32 fun_tsi_get_value16;
	uint_32 fun_tsi_set_threshold;
	uint_32 fun_tsi_enable_re_int;
	uint_32 fun_tsi_disable_re_int;
	uint_32 fun_tsi_softsearch;

	//uart����ָ�룬10��
	uint_32 fun_uart_init;
	uint_32 fun_uart_send1;
	uint_32 fun_uart_sendN;
	uint_32 fun_uart_send_string;
	uint_32 fun_uart_re1;
	uint_32 fun_uart_reN;
	uint_32 fun_uart_enable_re_int;
	uint_32 fun_uart_disable_re_int;
	uint_32 fun_uart_get_re_int;
	uint_32 fun_uart_deint;

	//uecom��������ָ�룬21��
	uint_32 fun_uecom_init;
	uint_32 fun_uecom_fast_init;
	uint_32 fun_uecom_deInit;
	uint_32 fun_uecom_switch;
	uint_32 fun_uecom_send;
	uint_32 fun_uecom_getIMEI;
	uint_32 fun_uecom_getIMSI;
	uint_32 fun_uecom_getFirmVer;
	uint_32 fun_uecom_getSignalPower;
	uint_32 fun_uecom_getBSNum;
	uint_32 fun_uecom_getUEIP;
	uint_32 fun_uecom_config;
	uint_32 fun_uecom_getCurCommunity;
	uint_32 fun_http_get;
	uint_32 fun_uecom_interrupt;
	uint_32 fun_uecom_default_delay_ms;
	uint_32 fun_gnss_init;
	uint_32 fun_gnss_get_time;
	uint_32 fun_gnss_get_speed;
	uint_32 fun_gnss_get_location;
	uint_32 fun_gnss_get_attitude;
};
#pragma  pack(pop)

//======================================================================
//�������ƣ�funPointerInit
//�������أ���
//����˵������
//���ܸ�Ҫ������ָ���ʼ����Ӳ�������ѹ̻���bootloader�����У��û�������ֻ��ʹ��ָ��ָ��������������
//======================================================================
void funPointerInit();

//======================================================================
//�������ƣ�cpy_nvic
//�������أ���
//����˵������
//���ܸ�Ҫ���ж�����������RAM�����û����򼴿�ʵ���жϷ������̵��Զ���
//======================================================================
void cpy_nvic(void);

//======================================================================
//�������ƣ�setVector
//�������أ���
//����˵����IRQn���ж������ţ�vector�жϷ������̺���
//���ܸ�Ҫ����̬�޸�ָ�����ж��������жϷ�������
//======================================================================
void setVector(IRQn_Type IRQn, uint32_t vector);

//Bootloader������û������ṩ��ϵͳʱ���ѯ����
typedef  void (*fun_systemTimeGet)(uint_64 *tick);
volatile fun_systemTimeGet systemTimeGet;

#endif //��ֹ�ظ����壨_COMMON_H  ��β)
