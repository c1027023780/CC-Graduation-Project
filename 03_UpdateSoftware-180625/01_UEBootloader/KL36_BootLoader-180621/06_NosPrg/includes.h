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
#include "gpio.h"
#include "uart.h"
#include "systick.h"
#include "flash.h"
#include "light.h"
#include "bootloader.h"
#include "ueupdate.h"
#include "string.h"
#include "printf.h"


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
	//flash函数指针，7个
	uint_32 fun_flash_init;
	uint_32 fun_flash_erase;
	uint_32 fun_flash_write;
	uint_32 fun_flash_read_logic;
	uint_32 fun_flash_read_physical;
	uint_32 fun_flash_protect;
	uint_32 fun_flash_isempty;

	//light函数指针，3个
	uint_32 fun_light_init;
	uint_32 fun_light_control;
	uint_32 fun_light_change;

	uint_32 fun_getTime;
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
