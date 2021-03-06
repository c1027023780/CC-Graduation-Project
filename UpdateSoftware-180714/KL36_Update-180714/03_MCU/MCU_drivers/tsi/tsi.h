//===========================================================================
//文件名称：tsi.h
//功能概要：KL25 tsi底层驱动程序头文件
//版权所有：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版本更新：2012-11-25  V1.0   初始版本
//       2015-03-18 v2.1
//===========================================================================

#ifndef TSI_H           //防止重复定义（ 开头)
#define TSI_H          
//1 头文件
#include "common.h"      //包含公共要素头文件

//2 宏定义    

//根据串口实际硬件引脚，确定以下宏常量值。
//在此工程中，我们只使用TSI0的第5通道，TSI0_CH宏定义为5

//TSI0_CH：0=PTB0脚，1=PTA0脚，2=PTA1脚，3=PTA2脚，4=PTA3脚，5=PTA4脚，
//         6=PTB1脚，7=PTB2脚，8=PTB3脚，9=PTB16脚，10=PTB17脚，11=PTB18脚，
//         12=PTB19脚，13=PTC0脚，14=PTC1脚，15=PTC2脚
//#define TSI0_CH 5    //SD-FSL-KL25-EVB板上使用PTA4脚


//3 函数声明
//=====================================================================
//函数名称：tsi_init
//功能概要：初始化TSI模块 ,KL25只有一个TSI模块
//参数说明：chnlIDs:8位无符号数，TSI模块所使用的通道号，其取值为0~15
//     threshold:阈值
//函数返回： 无
//=====================================================================
//void tsi_init(uint_8 chnlID,uint_32 threshold);
typedef  uint_8 (*fun_tsi_init)(uint_8 chnlID,uint_32 threshold);
volatile fun_tsi_init tsi_init;

//=====================================================================
//函数名称：tsi_get_value16
//功能概要：获取TSI通道的计数值
//参数说明：无
//函数返回： 获取TSI通道的计数值
//=====================================================================
//uint_16 tsi_get_value16(void);
typedef  uint_16 (*fun_tsi_get_value16)(void);
volatile fun_tsi_get_value16 tsi_get_value16;

//=====================================================================
//函数名称：tsi_set_threshold1
//功能概要：设定指定通道的阈值
//参数说明：low:   设定阈值下限   ，     取值范围为0~65535
//         high:  设定阈值上限   ，    取值范围为0~65535
//函数返回： 无
//=====================================================================
//void tsi_set_threshold(uint_16 low, uint_16 high);
typedef  void (*fun_tsi_set_threshold)(uint_16 low, uint_16 high);
volatile fun_tsi_set_threshold tsi_set_threshold;

//=====================================================================
//函数名称：tsi_enable_re_int
//功能概要：开TSI中断,关闭软件触发扫描,开中断控制器IRQ中断
//参数说明：无
//函数返回：无
//=====================================================================
//void tsi_enable_re_int(void);
typedef  void (*fun_tsi_enable_re_int)(void);
volatile fun_tsi_enable_re_int tsi_enable_re_int;

//=====================================================================
//函数名称：tsi_disable_re_int
//参数说明：无
//函数返回：无
//功能概要：关TSI中断,开软件触发扫描,关中断控制器IRQ中断
//=====================================================================
//void tsi_disable_re_int(void);
typedef  void (*fun_tsi_disable_re_int)(void);
volatile fun_tsi_disable_re_int tsi_disable_re_int;

//=====================================================================
//函数名称：tsi_softsearch
//功能概要：开启一次软件扫描
//参数说明：无
//函数返回：无
//=====================================================================
//void tsi_softsearch(void);
typedef  void (*fun_tsi_softsearch)(void);
volatile fun_tsi_softsearch tsi_softsearch;

#endif  //防止重复定义（ 结尾)
