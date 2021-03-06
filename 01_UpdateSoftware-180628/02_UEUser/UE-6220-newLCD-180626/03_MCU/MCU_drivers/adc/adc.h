//=====================================================================
//文件名称：adc.h
//功能概要：ADC底层驱动构件头文件
//制作单位：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版    本：2017-04-19  V1.0;
//适用芯片：KL25、KL26、KL36
//=====================================================================

#ifndef _ADC_H           //防止重复定义（ 开头）
#define _ADC_H

#include "common.h"    //包含公共要素头文件


//用于定义硬件滤波次数
#define SAMPLE4   0
#define SAMPLE8   1
#define SAMPLE16  2
#define SAMPLE32  3
//定义输入模式
#define AD_DIFF       1   //差分输入
#define AD_SINGLE    0   //单端输入
//通道选择
#define MUXSEL_A     0    //选择端口的A通道
#define MUXSEL_B     1    //选择端口的B通道

//======================================================================
//函数名称：adc_init
//功能概要：初始化一个AD通道组
//参数说明：chnGroup：通道组；有宏常数：MUXSEL_A（A通道）；MUXSEL_B（B通道）
//         diff：差分选择。=1，差分；=0，单端；也可使用宏常数AD_DIFF/AD_SINGLE
//         accurary：采样精度，差分可选9-13-11-16；单端可选8-12-10-16
//         HDAve：硬件滤波次数，从宏定义中选择SAMPLE4/SAMPLE8/ SAMPLE16/
//                                  SAMPLE32
//======================================================================
void adc_init( uint_8 chnGroup,uint_8 diff,uint_8 accurary,uint_8 HDAve);

//======================================================================
//函数名称：adc_read
//功能概要：进行一个通道的一次A/D转换                                                
//参数说明：channel：见MKL36Z64VLH4芯片ADC通道输入表
//======================================================================
uint_16 adc_read(uint_8 channel);

#endif

//===========================================================================
//声明：
//（1）我们开发的源代码，在本中心提供的硬件系统测试通过，真诚奉献给社会，不足之处，
//     欢迎指正。
//（2）对于使用非本中心硬件系统的用户，移植代码时，请仔细根据自己的硬件匹配。
//
//苏州大学嵌入式社区，http://sumcu.suda.edu.cn；0512-65214835


