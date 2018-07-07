//======================================================================
//文件名称：bootloader.h
//功能概要：bootloader构件头文件
//制作单位：苏州大学NXP嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2018-01-19,CC
//======================================================================

#ifndef _BOOTLOADER_H     //防止重复定义（_LIGHT_H  开头)
#define _BOOTLOADER_H

//头文件包含
#include "common.h"

//#define USERBASE    0x2000  //用户代码基址
//#define UPDATEBASE  0x9000  //更新代码基址

void bootloader_goToUserApp(volatile uint_32 app_start_addr);

#endif
