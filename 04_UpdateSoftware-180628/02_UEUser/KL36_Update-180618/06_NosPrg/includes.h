//======================================================================
//文件名称：includes.h
//功能概要：应用工程总头文件
//版权所有：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版本更新：2017.2.1  V1.0，2018.1.6  V2.2（WYH）
//功能描述：（1）包含用到的构件；（2）全局变量一处声明多处使用处理方法代码段；
//       （3）全局变量声明；（4）常数宏定义
//======================================================================

#ifndef _INCLUDES_H    //防止重复定义（开头）
#define _INCLUDES_H

//（1）【根据实际需要增删】包含用到的构件
#include "common.h"
#include "adc.h"
#include "gpio.h"
#include "lcd.h"
#include "lptmr.h"
#include "tsi.h"
#include "uart.h"
#include "light.h"
#include "systick.h"
#include "uecom.h"
#include "ueupdate.h"
#include "timeStamp.h"
#include "flash.h"
#include "wdog.h"
//（2）【不动】全局变量一处声明多处使用处理方法代码段
//为下面声明全局变量，使用前缀G_VAR_PREFIX宏定义做准备，GLOBLE_VAR在main.c中有宏定义
#ifdef GLOBLE_VAR   //main.c文件中包含本头文件时，全局变量不加“extern”前缀
  #define G_VAR_PREFIX
#else      //其他文件中包含本头文件时，全局变量自动加“extern”前缀
  #define G_VAR_PREFIX extern
#endif



//（3）【根据实际需要增删】全局变量声明处。一律加前缀G_VAR_PREFIX，仅此一次声明即可
//    全局变量命名一律以g开头，尽可能保证全工程内唯一性，并且一定要注明其含义
//    （声明时不准赋值）
G_VAR_PREFIX struct comData gcd;  //通信帧（comData结构体在本文件中定义）
G_VAR_PREFIX uint_8 gTime[14];    //时间戳转换后的日期时间"20170927010021"
G_VAR_PREFIX uint_8 gTflag;       //液晶错误提示标志
//G_VAR_PREFIX uint_8 gRecvBuf[1024];  //收到的数据缓冲区
//G_VAR_PREFIX uint_16 gRecvLength;    //收到的数据长度，为0时表示没有收到数据

//（4）【根据实际需要增删】全局使用的宏常数，使用全大写
#define SEND_TIME   10           //发送时间（分的整倍数）
#define WAIT_RECV_SECONDS   35   //接收数据的等待秒数最大值，若该时间内未接收到数据则给UE模块断电
#define TSI_VALUE  0x00CC0011    //触摸感应阈值（实验获得）
#define SECTOR_NO  50            //Flash扇区号
#define OFFSET     0             //Flash扇区内偏移
#define CNT sizeof(struct comData)    //通信帧字节数
//#define ENABLE_WDOG	   1       //设置是否开启看门狗

//终端UE与人机交互系统HCI的通信帧结构体comData定义
#pragma  pack(push,1)   //取消4字节对齐
struct comData
{
	//【不动】这些变量为必须要有的变量
	uint_32 sn;              //帧号
	uint_8 IMSI[15];         //IMSI号
	uint_8 serverIP[15];     //服务器IP
	uint_8 serverPort[5];    //服务器端口
	uint_64 currentTime;     //发送时间（使用64位时间戳，可转换为年月日时分秒）
	uint_32 resetCount;
	uint_32 sendFrequencyMin;
	//【根据实际需要增删】此处可按需要增删变量，注意与服务器端帧结构保持一致
	uint_8 userName[20];     //用户名
	uint_8 softVer[4];       //软件版本
	uint_8 equipName[30];    //产品名称
	uint_8 equipID[20];      //产品序列号
	uint_8 equipType[20];    //产品类型
	uint_8 vendor[30];       //生产厂家
	int_32 mcuTemp;          //芯片温度
	uint_8 surpBaseInfo[20]; //备注1
	uint_8 phone[11];        //手机号
	uint_8 IMEI[15];         //IMEI号
	uint_8 signalPower;      //信号强度
	uint_16 bright;          //光线亮度（光敏传感器A/D值）
	uint_16 touchNum;        //触摸按键次数（TSI）
	uint_8 surplusInfo[20];  //备注2
	//【画瓢处1】
	//此处可添加用户自定义数据
    //uint_8 example1;
	uint_8 lbs_location[25];
};
#pragma  pack(pop)     //恢复4字节对齐

#endif                  //防止重复定义（结尾）

/*
 知识要素：
 （1）本文件共有两类代码，一类为【根据实际需要增删】，此类代码根据具体
 项目需求进行更改；另一类为【不动】，此类代码与具体项目无关，是通用的，
 无需根据具体项目进行更改。
 （2）全局变量一处声明多处使用处理方法代码段是为了解决全局变量重复声明
 的问题，宏GLOBLE_VAR仅在main.c中声明，所以只有在main.c中包含
 本头文件时才会声明全局变量，在其他文件中包含本文件时，编译时，就会自动加
 上前缀extern，表明是main函数中声明的全局变量，该文件中直接使用。
 （3）声明全局变量时，一律加前缀G_VAR_PREFIX，并只在本文件
 的“全局变量声明处”声明， 仅在此一次声明即可。 全局变量命名一律以g开头，
 尽可能保证全工程内唯一性，并且一定要注明其含义（声明时不准赋值）。
 （4）全局使用的宏常数，使用全大写
 */
