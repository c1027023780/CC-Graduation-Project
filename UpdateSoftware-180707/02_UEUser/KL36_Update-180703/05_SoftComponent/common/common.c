//===========================================================================
//文件名称：common.h
//功能概要：公共要素源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2012-10-12 V1.0
//         2013-02-02 V2.0(WYH)
//===========================================================================

#include "common.h"    //包含公共要素头文件
#include "adc.h"
#include "flash.h"
#include "gpio.h"
#include "lptmr.h"
#include "spi.h"
#include "tsi.h"
#include "uart.h"
#include "uecom.h"
//#include "string.h"
#include "valueType.h"

static  uint_32  int_disable_level; // 静态变量，当前中断嵌套层数

//=========================================================================
//函数名称：init_critical
//参数说明：无
//函数返回：无
//功能概要：初始化临界区访问控制
//=========================================================================
void  init_critical(void) 
{
	ENABLE_INTERRUPTS;
	int_disable_level = 0;
}

//=========================================================================
//函数名称：enter_critical
//参数说明：无
//函数返回：无
//功能概要：进入临界区
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
//函数名称：exit_critical
//参数说明：无
//函数返回：无
//功能概要：离开临界区
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
//函数名称：enable_irq
//函数返回：无
//参数说明：irq：irq号, 各个模块的中断号定义在MKL25Z.h中
//功能概要：使能irq中断 
//============================================================================
void enable_irq (IRQn_Type irq)
{
	//确定irq号为有效的irq号
	if (irq > (IRQn_Type)32)    irq=(IRQn_Type)32;

	NVIC_EnableIRQ(irq);

}

//============================================================================
//函数名称：disable_irq
//函数返回：无      
//参数说明：irq：irq号, 各个模块的中断号定义在MKL25Z.h中
//功能概要：禁止irq中断 
//============================================================================
void disable_irq (IRQn_Type irq)
{

	//确定irq号为有效的irq号
	if (irq > (IRQn_Type)32)    irq=(IRQn_Type)32;

	NVIC_DisableIRQ(irq);
}

//============================================================================
//函数名称：set_irq_priority
//函数返回：无      
//参数说明：irq：irq号,各个模块的中断号定义在MKL25Z.h中
//         prio：优先级
//功能概要：设置irq中断和优先级 
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
//函数名称：Delay_us
//函数返回：无
//参数说明：无
//功能概要：延时 - 微秒级
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
//函数名称：Delay_us
//函数返回：无
//参数说明：无
//功能概要：延时 - 毫秒级
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
			wdog_feed();           //【看门狗喂狗】
#endif
	}
}

//======================================================================
//函数名称：funPointerInit
//函数返回：无
//参数说明：无
//功能概要：函数指针初始化，硬件驱动已固化在bootloader程序中，用户程序中只需使用指针指向驱动函数即可
//======================================================================
void funPointerInit()
{
	//（1）变量声明
	uint_8 *src;
	struct funInfo info;

	//（2）获取flash43扇区指针数据
	src=(uint_8 *)(33*1024);
	copyN((uint_8*)&info,src,sizeof(struct funInfo));
	//（3）指针函数赋值
	//（3.1）bootloader对外接口函数
	systemTimeGet = (fun_systemTimeGet)info.fun_systemTimeGet;
	//（3.2）adc构件函数指针，2个
	adc_init = (fun_adc_init)info.fun_adc_init;
	adc_read = (fun_adc_read)info.fun_adc_read;
	//（3.3）flash函数指针，7个
	flash_init = (fun_flash_init)info.fun_flash_init;
	flash_erase = (fun_flash_erase)info.fun_flash_erase;
	flash_write = (fun_flash_write)info.fun_flash_write;
	flash_read_logic = (fun_flash_read_logic)info.fun_flash_read_logic;
	flash_read_physical = (fun_flash_read_physical)info.fun_flash_read_physical;
	flash_protect = (fun_flash_protect)info.fun_flash_protect;
	flash_isempty = (fun_flash_isempty)info.fun_flash_isempty;
	//（3.4）gpio构件函数指针，11个
	gpio_init = (fun_gpio_init)info.fun_gpio_init;
	gpio_set = (fun_gpio_set)info.fun_gpio_set;
	gpio_get = (fun_gpio_get)info.fun_gpio_get;
	gpio_reverse = (fun_gpio_reverse)info.fun_gpio_reverse;
	gpio_pull = (fun_gpio_pull)info.fun_gpio_pull;
	gpio_enable_int = (fun_gpio_enable_int)info.fun_gpio_enable_int;
	gpio_disable_int = (fun_gpio_disable_int)info.fun_gpio_disable_int;
	gpio_get_int = (fun_gpio_get_int)info.fun_gpio_get_int;
	gpio_clear_int = (fun_gpio_clear_int)info.fun_gpio_clear_int;
	gpio_clear_allint = (fun_gpio_clear_allint)info.fun_flash_erase;
	gpio_drive_strength = (fun_gpio_drive_strength)info.fun_gpio_drive_strength;
	//（3.5）lptmr构件函数指针，4个
	lptmr_init = (fun_lptmr_init)info.fun_lptmr_init;
	lptmr_reset = (fun_lptmr_reset)info.fun_lptmr_reset;
	enable_lptmr_int = (fun_enable_lptmr_int)info.fun_enable_lptmr_int;
	disable_lptmr_int = (fun_disable_lptmr_int)info.fun_disable_lptmr_int;
	//（3.6）spi构件函数指针，7个
	SPI_init = (fun_SPI_init)info.fun_SPI_init;
	SPI_send1 = (fun_SPI_send1)info.fun_SPI_send1;
	SPI_sendN = (fun_SPI_sendN)info.fun_SPI_sendN;
	SPI_receive1 = (fun_SPI_receive1)info.fun_SPI_receive1;
	SPI_receiveN = (fun_SPI_receiveN)info.fun_SPI_receiveN;
	SPI_enable_re_int = (fun_SPI_enable_re_int)info.fun_SPI_enable_re_int;
	SPI_disable_re_int = (fun_SPI_disable_re_int)info.fun_SPI_disable_re_int;
	//（3.7）tsi构件函数指针，6个
	tsi_init = (fun_tsi_init)info.fun_tsi_init;
	tsi_get_value16 = (fun_tsi_get_value16)info.fun_tsi_get_value16;
	tsi_set_threshold = (fun_tsi_set_threshold)info.fun_tsi_set_threshold;
	tsi_enable_re_int = (fun_tsi_enable_re_int)info.fun_tsi_enable_re_int;
	tsi_disable_re_int = (fun_tsi_disable_re_int)info.fun_tsi_disable_re_int;
	tsi_softsearch = (fun_tsi_softsearch)info.fun_tsi_softsearch;
	//（3.8）uart函数指针，10个
	uart_init = (fun_uart_init)info.fun_uart_init;
	uart_send1 = (fun_uart_send1)info.fun_uart_send1;
	uart_sendN = (fun_uart_sendN)info.fun_uart_sendN;
	uart_send_string = (fun_uart_send_string)info.fun_uart_send_string;
	uart_re1 = (fun_uart_re1)info.fun_uart_re1;
	uart_reN = (fun_uart_reN)info.fun_uart_reN;
	uart_enable_re_int = (fun_uart_enable_re_int)info.fun_uart_enable_re_int;
	uart_disable_re_int = (fun_uart_disable_re_int)info.fun_uart_disable_re_int;
	uart_get_re_int = (fun_uart_get_re_int)info.fun_uart_get_re_int;
	uart_deint = (fun_uart_deint)info.fun_uart_deint;
   	//（3.9）uecom构件函数指针，21个
    uecom_init = (fun_uecom_init)info.fun_uecom_init;
    uecom_fast_init = (fun_uecom_fast_init)info.fun_uecom_fast_init;
    uecom_deInit = (fun_uecom_deInit)info.fun_uecom_deInit;
    uecom_switch = (fun_uecom_switch)info.fun_uecom_switch;
    uecom_send = (fun_uecom_send)info.fun_uecom_send;
    uecom_getIMEI = (fun_uecom_getIMEI)info.fun_uecom_getIMEI;
    uecom_getIMSI = (fun_uecom_getIMSI)info.fun_uecom_getIMSI;
    uecom_getFirmVer = (fun_uecom_getFirmVer)info.fun_uecom_getFirmVer;
    uecom_getSignalPower = (fun_uecom_getSignalPower)info.fun_uecom_getSignalPower;
    uecom_getBSNum = (fun_uecom_getBSNum)info.fun_uecom_getBSNum;
    uecom_getUEIP = (fun_uecom_getUEIP)info.fun_uecom_getUEIP;
    uecom_config = (fun_uecom_config)info.fun_uecom_config;
    uecom_getCurCommunity = (fun_uecom_getCurCommunity)info.fun_uecom_getCurCommunity;
    http_get = (fun_http_get)info.fun_http_get;
    uecom_interrupt = (fun_uecom_interrupt)info.fun_uecom_interrupt;
    uecom_default_delay_ms = (fun_uecom_default_delay_ms)info.fun_uecom_default_delay_ms;
    gnss_init = (fun_gnss_init)info.fun_gnss_init;
    gnss_get_time = (fun_gnss_get_time)info.fun_gnss_get_time;
    gnss_get_speed = (fun_gnss_get_speed)info.fun_gnss_get_speed;
    gnss_get_location = (fun_gnss_get_location)info.fun_gnss_get_location;
    gnss_get_attitude = (fun_gnss_get_attitude)info.fun_gnss_get_attitude;

}

//======================================================================
//函数名称：cpy_nvic
//函数返回：无
//参数说明：无
//功能概要：中断向量表复制至RAM区，用户程序即可实现中断服务例程的自定义
//======================================================================
void cpy_nvic(void)
{
#define NVIC_NUM_VECTORS        (16 + 32)     // CORE + MCU Peripherals
#define NVIC_RAM_VECTOR_ADDRESS 0x20000800    //用户程序RAM起始地址

	uint32_t *old_vectors = (uint32_t *)SCB->VTOR;    //取得系统控制块中VTOR①寄存器的值
	uint32_t *vectors = (uint32_t*)NVIC_RAM_VECTOR_ADDRESS;
	for (int i = 0; i < NVIC_NUM_VECTORS; i++) {    //将48个中断向量拷贝到内存地址中
		vectors[i] = old_vectors[i];
	}
	SCB->VTOR = (uint32_t)NVIC_RAM_VECTOR_ADDRESS;    //设置VTOR寄存器指向新的地址
}

//======================================================================
//函数名称：setVector
//函数返回：无
//参数说明：IRQn：中断向量号，vector中断服务例程函数
//功能概要：动态修改指定的中断向量的中断服务例程
//======================================================================
void setVector(IRQn_Type IRQn, uint32_t vector)
{
#define NVIC_USER_IRQ_OFFSET          16
	uint32_t *vectors = (uint32_t *)SCB->VTOR;

	vectors[(int32_t)IRQn + NVIC_USER_IRQ_OFFSET] = vector;
}
