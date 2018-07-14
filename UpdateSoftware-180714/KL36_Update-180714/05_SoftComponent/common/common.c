//===========================================================================
//�ļ����ƣ�common.h
//���ܸ�Ҫ������Ҫ��Դ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2012-10-12 V1.0
//         2013-02-02 V2.0(WYH)
//===========================================================================

#include "common.h"    //��������Ҫ��ͷ�ļ�
#include "adc.h"
#include "flash.h"
#include "gpio.h"
#include "i2c.h"
#include "lptmr.h"
#include "spi.h"
#include "tsi.h"
#include "uart.h"
#include "uecom.h"
#include "valueType.h"

static  uint_32  int_disable_level; // ��̬��������ǰ�ж�Ƕ�ײ���

//=========================================================================
//�������ƣ�init_critical
//����˵������
//�������أ���
//���ܸ�Ҫ����ʼ���ٽ������ʿ���
//=========================================================================
void  init_critical(void) 
{
	ENABLE_INTERRUPTS;
	int_disable_level = 0;
}

//=========================================================================
//�������ƣ�enter_critical
//����˵������
//�������أ���
//���ܸ�Ҫ�������ٽ���
//=========================================================================
void  enter_critical(void)
{
	if (0 == int_disable_level)
	{
		DISABLE_INTERRUPTS;
	}
	int_disable_level++;
}

//=========================================================================
//�������ƣ�exit_critical
//����˵������
//�������أ���
//���ܸ�Ҫ���뿪�ٽ���
//=========================================================================
void  exit_critical(void) 
{
	if (int_disable_level)
	{
		int_disable_level--;
		if (0 == int_disable_level)
		{
			ENABLE_INTERRUPTS;
		}
	}
}

//============================================================================
//�������ƣ�enable_irq
//�������أ���
//����˵����irq��irq��, ����ģ����жϺŶ�����MKL25Z.h��
//���ܸ�Ҫ��ʹ��irq�ж� 
//============================================================================
void enable_irq (IRQn_Type irq)
{
	//ȷ��irq��Ϊ��Ч��irq��
	if (irq > (IRQn_Type)32)    irq=(IRQn_Type)32;

	NVIC_EnableIRQ(irq);

}

//============================================================================
//�������ƣ�disable_irq
//�������أ���      
//����˵����irq��irq��, ����ģ����жϺŶ�����MKL25Z.h��
//���ܸ�Ҫ����ֹirq�ж� 
//============================================================================
void disable_irq (IRQn_Type irq)
{

	//ȷ��irq��Ϊ��Ч��irq��
	if (irq > (IRQn_Type)32)    irq=(IRQn_Type)32;

	NVIC_DisableIRQ(irq);
}

//============================================================================
//�������ƣ�set_irq_priority
//�������أ���      
//����˵����irq��irq��,����ģ����жϺŶ�����MKL25Z.h��
//         prio�����ȼ�
//���ܸ�Ҫ������irq�жϺ����ȼ� 
//============================================================================
void set_irq_priority (IRQn_Type irq, uint_16 prio)
{   
	uint_8 err = 0;

	if (irq > (IRQn_Type)32)
	{
		err = 1;
	}
	if (prio > 3)
	{
		err = 1;
	}
	if (err != 1)
	{
		NVIC_SetPriority(irq, prio);
	}
}

//======================================================================
//�������ƣ�Delay_us
//�������أ���
//����˵������
//���ܸ�Ҫ����ʱ - ΢�뼶
//======================================================================
void Delay_us(uint16_t u16us)
{
	uint32_t u32ctr;
	for(u32ctr = 0; u32ctr < ((48000 / 1000 / 11) * u16us); u32ctr++)
	{
		__asm ("NOP");
	}
}

//======================================================================
//�������ƣ�Delay_us
//�������أ���
//����˵������
//���ܸ�Ҫ����ʱ - ���뼶
//======================================================================
void Delay_ms(uint16_t u16ms)
{
	vuint_16 i;
	vuint_32 j;
	for(i = 0; i<u16ms; i++)
	{
		for(j = 0; j<4200; j++)
			__asm ("NOP");
#if ENABLE_WDOG == 1
		if(i%30==0)
			wdog_feed();           //�����Ź�ι����
#endif
	}
}

//======================================================================
//�������ƣ�funPointerInit
//�������أ���
//����˵������
//���ܸ�Ҫ������ָ���ʼ����Ӳ�������ѹ̻���bootloader�����У��û�������ֻ��ʹ��ָ��ָ��������������
//======================================================================
void funPointerInit()
{
	//��1����������
	uint_8 *src;
	struct funInfo info;

	//��2����ȡflash43����ָ������
	src=(uint_8 *)(25*1024+24);//25�������ţ�24��ƫ����
	copyN((uint_8*)&info,src,sizeof(struct funInfo));
	//��3��ָ�뺯����ֵ
	//��3.1��bootloader����ӿں���
	systemTimeGet = (fun_systemTimeGet)info.fun_systemTimeGet;
	systemTimeSet = (fun_systemTimeSet)info.fun_systemTimeSet;
	systemInfoGet = (fun_systemInfoGet)info.fun_systemInfoGet;
	//��3.2��adc��������ָ�룬2��
	adc_init = (fun_adc_init)info.fun_adc_init;
	adc_read = (fun_adc_read)info.fun_adc_read;
	//��3.3��flash����ָ�룬7��
	flash_init          = (fun_flash_init)info.fun_flash_init;
	flash_erase         = (fun_flash_erase)info.fun_flash_erase;
	flash_write         = (fun_flash_write)info.fun_flash_write;
	flash_read_logic    = (fun_flash_read_logic)info.fun_flash_read_logic;
	flash_read_physical = (fun_flash_read_physical)info.fun_flash_read_physical;
	flash_protect       = (fun_flash_protect)info.fun_flash_protect;
	flash_isempty       = (fun_flash_isempty)info.fun_flash_isempty;
	//��3.4��gpio��������ָ�룬11��
	gpio_init           = (fun_gpio_init)info.fun_gpio_init;
	gpio_set            = (fun_gpio_set)info.fun_gpio_set;
	gpio_get            = (fun_gpio_get)info.fun_gpio_get;
	gpio_reverse        = (fun_gpio_reverse)info.fun_gpio_reverse;
	gpio_pull           = (fun_gpio_pull)info.fun_gpio_pull;
	gpio_enable_int     = (fun_gpio_enable_int)info.fun_gpio_enable_int;
	gpio_disable_int    = (fun_gpio_disable_int)info.fun_gpio_disable_int;
	gpio_get_int        = (fun_gpio_get_int)info.fun_gpio_get_int;
	gpio_clear_int      = (fun_gpio_clear_int)info.fun_gpio_clear_int;
	gpio_clear_allint   = (fun_gpio_clear_allint)info.fun_flash_erase;
	gpio_drive_strength = (fun_gpio_drive_strength)info.fun_gpio_drive_strength;
	//��3.5��i2c��������ָ�룬8��
	I2C_init           = (fun_I2C_init)info.fun_I2C_init;
	I2C_read1          = (fun_I2C_read1)info.fun_I2C_read1;
	I2C_readN          = (fun_I2C_readN)info.fun_I2C_readN;
	I2C_receN          = (fun_I2C_receN)info.fun_I2C_receN;
	I2C_write1         = (fun_I2C_write1)info.fun_I2C_write1;
	I2C_writeN         = (fun_I2C_writeN)info.fun_I2C_writeN;
	I2C_enable_re_int  = (fun_I2C_enable_re_int)info.fun_I2C_enable_re_int;
	I2C_disable_re_int = (fun_I2C_disable_re_int)info.fun_I2C_disable_re_int;
	//��3.6��lptmr��������ָ�룬4��
	lptmr_init        = (fun_lptmr_init)info.fun_lptmr_init;
	lptmr_reset       = (fun_lptmr_reset)info.fun_lptmr_reset;
	enable_lptmr_int  = (fun_enable_lptmr_int)info.fun_enable_lptmr_int;
	disable_lptmr_int = (fun_disable_lptmr_int)info.fun_disable_lptmr_int;
	//��3.7��spi��������ָ�룬7��
	SPI_init           = (fun_SPI_init)info.fun_SPI_init;
	SPI_send1          = (fun_SPI_send1)info.fun_SPI_send1;
	SPI_sendN          = (fun_SPI_sendN)info.fun_SPI_sendN;
	SPI_receive1       = (fun_SPI_receive1)info.fun_SPI_receive1;
	SPI_receiveN       = (fun_SPI_receiveN)info.fun_SPI_receiveN;
	SPI_enable_re_int  = (fun_SPI_enable_re_int)info.fun_SPI_enable_re_int;
	SPI_disable_re_int = (fun_SPI_disable_re_int)info.fun_SPI_disable_re_int;
	//��3.8��tsi��������ָ�룬6��
	tsi_init           = (fun_tsi_init)info.fun_tsi_init;
	tsi_get_value16    = (fun_tsi_get_value16)info.fun_tsi_get_value16;
	tsi_set_threshold  = (fun_tsi_set_threshold)info.fun_tsi_set_threshold;
	tsi_enable_re_int  = (fun_tsi_enable_re_int)info.fun_tsi_enable_re_int;
	tsi_disable_re_int = (fun_tsi_disable_re_int)info.fun_tsi_disable_re_int;
	tsi_softsearch     = (fun_tsi_softsearch)info.fun_tsi_softsearch;
	//��3.9��uart����ָ�룬10��
	uart_init           = (fun_uart_init)info.fun_uart_init;
	uart_send1          = (fun_uart_send1)info.fun_uart_send1;
	uart_sendN          = (fun_uart_sendN)info.fun_uart_sendN;
	uart_send_string    = (fun_uart_send_string)info.fun_uart_send_string;
	uart_re1            = (fun_uart_re1)info.fun_uart_re1;
	uart_reN            = (fun_uart_reN)info.fun_uart_reN;
	uart_enable_re_int  = (fun_uart_enable_re_int)info.fun_uart_enable_re_int;
	uart_disable_re_int = (fun_uart_disable_re_int)info.fun_uart_disable_re_int;
	uart_get_re_int     = (fun_uart_get_re_int)info.fun_uart_get_re_int;
	uart_deint          = (fun_uart_deint)info.fun_uart_deint;
   	//��3.10��uecom��������ָ�룬21��
    uecom_power            = (fun_uecom_power)info.fun_uecom_power;
    uecom_init             = (fun_uecom_init)info.fun_uecom_init;
    uecom_linkBase         = (fun_uecom_linkBase)info.fun_uecom_linkBase;
    uecom_linkCS           = (fun_uecom_linkCS)info.fun_uecom_linkCS;
    uecom_send             = (fun_uecom_send)info.fun_uecom_send;
    uecom_interrupt        = (fun_uecom_interrupt)info.fun_uecom_interrupt;
    uecom_baseInfo         = (fun_uecom_baseInfo)info.fun_uecom_baseInfo;
    uecom_modelInfo        = (fun_uecom_modelInfo)info.fun_uecom_modelInfo;
    uecom_gnssSwitch       = (fun_uecom_gnssSwitch)info.fun_uecom_gnssSwitch;
    uecom_gnssGetInfo      = (fun_uecom_gnssGetInfo)info.fun_uecom_gnssGetInfo;
    uecom_httpGet          = (fun_uecom_httpGet)info.fun_uecom_httpGet;
    uecom_default_delay_ms = (fun_uecom_default_delay_ms)info.fun_uecom_default_delay_ms;
}

//======================================================================
//�������ƣ�cpy_nvic
//�������أ���
//����˵������
//���ܸ�Ҫ���ж�����������RAM�����û����򼴿�ʵ���жϷ������̵��Զ���
//======================================================================
void cpy_nvic(void)
{
#define NVIC_NUM_VECTORS        (16 + 32)     // CORE + MCU Peripherals
#define NVIC_RAM_VECTOR_ADDRESS 0x20000400    //�û�����RAM��ʼ��ַ

	uint32_t *old_vectors = (uint32_t *)SCB->VTOR;    //ȡ��ϵͳ���ƿ���VTOR�ټĴ�����ֵ
	uint32_t *vectors = (uint32_t*)NVIC_RAM_VECTOR_ADDRESS;
	for (int i = 0; i < NVIC_NUM_VECTORS; i++) {    //��48���ж������������ڴ��ַ��
		vectors[i] = old_vectors[i];
	}
	SCB->VTOR = (uint32_t)NVIC_RAM_VECTOR_ADDRESS;    //����VTOR�Ĵ���ָ���µĵ�ַ
}

//======================================================================
//�������ƣ�setVector
//�������أ���
//����˵����IRQn���ж������ţ�vector�жϷ������̺���
//���ܸ�Ҫ����̬�޸�ָ�����ж��������жϷ�������
//======================================================================
void setVector(IRQn_Type IRQn, uint32_t vector)
{
#define NVIC_USER_IRQ_OFFSET          16

	if(IRQn == SysTick_IRQn || IRQn == UART2_IRQn)  //�������޸�Systick��Uart2�ж�
		return;

	uint32_t *vectors = (uint32_t *)SCB->VTOR;

	vectors[(int32_t)IRQn + NVIC_USER_IRQ_OFFSET] = vector;
}
