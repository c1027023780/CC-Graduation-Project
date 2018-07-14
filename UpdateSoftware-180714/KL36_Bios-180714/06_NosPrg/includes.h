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
#include "i2c.h"
#include "lptmr.h"
#include "spi.h"
#include "systick.h"
#include "tsi.h"
#include "uart.h"
#include "bootloader.h"
#include "light.h"
#include "uecom.h"
#include "ueupdate.h"
#include "timeStamp.h"


//（2）【不动】全局变量一处声明多处使用处理方法代码段
//    为下面声明全局变量，使用前缀G_VAR_PREFIX宏定义做准备
//    GLOBLE_VAR在main.c中有宏定义
#ifdef GLOBLE_VAR		 //main.c文件中包含本头文件时，全局变量不加“extern”前缀
#define G_VAR_PREFIX
#else                    //其他文件中包含本头文件时，全局变量自动加“extern”前缀
#define G_VAR_PREFIX extern
#endif


//（3）【根据实际需要增删】全局变量声明，一律加前缀G_VAR_PREFIX，仅此一次声明即可
//    全局变量命名一律以g开头，尽可能保证全工程内唯一性，并且一定要注明其含义
//    （声明时不准赋值）
G_VAR_PREFIX vuint_8 gFunFlag;      //主函数功能标识
G_VAR_PREFIX uint_8 gRecvBuf[500]; //串口接收数据缓冲区
G_VAR_PREFIX uint_16 gRecvLen;     //串口接收数据长度
G_VAR_PREFIX uint_64 gTime;

//（4）【根据实际需要增删】全局使用的宏定义常数，使用全大写
#define SECTORNO 25                 //函数指针和更新状态所处扇区
#define USERBASE (SECTORNO+1)*1024  //用户代码位置 26扇区开始
#define UPDATEADDRESS 0x200003F8

#endif
