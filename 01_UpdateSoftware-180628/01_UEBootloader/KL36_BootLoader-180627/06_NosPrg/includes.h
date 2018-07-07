//======================================================================
//文件名称：includes.h
//功能概要：应用工程总头文件
//版权所有：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版本更新：2017-02-01  V1.0，2017-09-27  V2.0
//功能描述：（1）包含用到的构件；（2）全局变量一处声明多处使用处理方法代码段；
//       （3）全局变量声明；（4）常数宏定义
//======================================================================

#ifndef _INCLUDES_H
#define _INCLUDES_H

//（1）【根据实际需要增删】包含用到的构件
#include "common.h"
#include "adc.h"
#include "flash.h"
#include "gpio.h"
#include "lptmr.h"
#include "spi.h"
#include "systick.h"
#include "tsi.h"
#include "uart.h"
#include "bootloader.h"
#include "light.h"
//#include "uecom.h"
#include "ueupdate.h"
#include "string.h"
#include "time.h"


//（2）【不动】全局变量一处声明多处使用处理方法代码段
//    为下面声明全局变量，使用前缀G_VAR_PREFIX宏定义做准备
//    GLOBLE_VAR在main.c中有宏定义
#ifdef GLOBLE_VAR		 //main.c文件中包含本头文件时，全局变量不加“extern”前缀
  #define G_VAR_PREFIX
#else                    //其他文件中包含本头文件时，全局变量自动加“extern”前缀
  #define G_VAR_PREFIX extern
#endif

#pragma  pack(push,1)
struct funInfo{
	//bootloader对外提供的接口函数
	uint_32 fun_systemTimeGet;

	//adc构建函数指针，2个
	uint_32 fun_adc_init;
	uint_32 fun_adc_read;

	//flash函数指针，7个
	uint_32 fun_flash_init;
	uint_32 fun_flash_erase;
	uint_32 fun_flash_write;
	uint_32 fun_flash_read_logic;
	uint_32 fun_flash_read_physical;
	uint_32 fun_flash_protect;
	uint_32 fun_flash_isempty;

	//gpio构件函数指针，11个
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

	//lptmr构件函数指针，4个
	uint_32 fun_lptmr_init;
	uint_32 fun_lptmr_reset;
	uint_32 fun_enable_lptmr_int;
	uint_32 fun_disable_lptmr_int;

	//spi构件函数指针，7个
	uint_32 fun_SPI_init;
	uint_32 fun_SPI_send1;
	uint_32 fun_SPI_sendN;
	uint_32 fun_SPI_receive1;
	uint_32 fun_SPI_receiveN;
	uint_32 fun_SPI_enable_re_int;
	uint_32 fun_SPI_disable_re_int;

	//tsi构件函数指针，6个
	uint_32 fun_tsi_init;
	uint_32 fun_tsi_get_value16;
	uint_32 fun_tsi_set_threshold;
	uint_32 fun_tsi_enable_re_int;
	uint_32 fun_tsi_disable_re_int;
	uint_32 fun_tsi_softsearch;

	//uart函数指针，10个
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

	//uecom构件函数指针，21个
//	uint_32 fun_uecom_init;
//	uint_32 fun_uecom_fast_init;
//	uint_32 fun_uecom_deInit;
//	uint_32 fun_uecom_switch;
//	uint_32 fun_uecom_send;
//	uint_32 fun_uecom_getIMEI;
//	uint_32 fun_uecom_getIMSI;
//	uint_32 fun_uecom_getFirmVer;
//	uint_32 fun_uecom_getSignalPower;
//	uint_32 fun_uecom_getBSNum;
//	uint_32 fun_uecom_getUEIP;
//	uint_32 fun_uecom_config;
//	uint_32 fun_uecom_getCurCommunity;
//	uint_32 fun_http_get;
//	uint_32 fun_uecom_interrupt;
//	uint_32 fun_uecom_default_delay_ms;
//	uint_32 fun_gnss_init;
//	uint_32 fun_gnss_get_time;
//	uint_32 fun_gnss_get_speed;
//	uint_32 fun_gnss_get_location;
//	uint_32 fun_gnss_get_attitude;
};
#pragma  pack(pop)

//（3）【根据实际需要增删】全局变量声明，一律加前缀G_VAR_PREFIX，仅此一次声明即可
//    全局变量命名一律以g开头，尽可能保证全工程内唯一性，并且一定要注明其含义
//    （声明时不准赋值）
G_VAR_PREFIX uint_8 gFunFlag;      //主函数功能标识
G_VAR_PREFIX uint_8 gRecvBuf[512]; //串口接收数据缓冲区
G_VAR_PREFIX uint_16 gRecvLen;     //串口接收数据长度
G_VAR_PREFIX uint_64 gTime;

//（4）【根据实际需要增删】全局使用的宏定义常数，使用全大写
#define USERBASE 0x7800  //用户代码位置 30扇区开始
#define UPDATEADDRESS  0xBC00  //更新代码基址
#define SECTORNO 29          //函数指针和更新状态所处扇区

#endif
