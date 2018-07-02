//===========================================================================
//文件名称：power_mode.h
//功能概要：电源模式切换构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2017-5-23   V1.0
//===========================================================================
#ifndef _POWER_MODE_H
#define _POWER_MODE_H

#include "stdlib.h"
#include "common.h"
#include "lcd.h"

#define bool    _Bool
#define true    1
#define false    0



bool enter_VLPR(void);

bool enter_VLPS(void);


#endif
