//======================================================================
//文件名称：ueupdate.h
//功能概要：终端程序更新构件头文件
//制作单位：苏州大学NXP嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2018-04-19,CC
//======================================================================

#ifndef _UEUPDATE_H
#define _UEUPDATE_H

#include "common.h"
#include "uart.h"
#include "flash.h"

#define USERBASE       0x2000  //用户代码基址
#define USERADDRESS    0xA000  //用户待更新代码地址
#define UPDATEADDRESS  0xD000  //更新代码基址
#define SECTORNUM   64      //扇区大小

//更新帧,总字节426
#pragma  pack(push,1)
struct copyData
{
	uint_16 frameNum;     //总帧数
	uint_16 curFrame;     //当前帧数
	uint_8 operate;       //操作，0：复制，1：插入
	uint_8 copyNum;       //复制代码个数，最大70
	uint_16 data[210];     //复制数据，每3个数据一组from,to,len
};
#pragma  pack(pop)

//插入帧,总字节486
#pragma  pack(push,1)
struct insertData
{
	uint_16 frameNum;     //总帧数
	uint_16 curFrame;     //当前帧数
	uint_8 operate;       //操作，0：复制，1：插入
	uint_8 insertNum;       //复制代码个数，最大70
	uint_16 insertInfo[40]; //插入代码的起始地址和长度,每2个数据一组，最多20组
	uint_8 data[400];     //插入数据
};
#pragma  pack(pop)

//检查帧
#pragma  pack(push,1)
struct checkData
{
	uint_16 frameNum;     //总帧数
	uint_8 loseNum;       //未接收的帧个数
	uint_16 loseFrame[40];//未接收的帧号,最多40个
};
#pragma  pack(pop)

#pragma  pack(push,1)
struct updateInfo{
	uint_8 updateFlag;  //更新标志 （3表示更新操作）
	uint_8 updateNum;   //待更新块个数，最多10个（可修改）
	uint_8 updateData[30];//待更新数据，每3个数据一组（from扇区号、to扇区号、size扇区个数）
};
#pragma  pack(pop)

//======================================================================
//函数名称：ueupdate_init
//函数参数：无
//函数返回：无
//功能概要：实现终端程序更新功能初始化，本构件使用串口作为更新方式。
//       具体操作为串口和flash模块的初始化和帧标志初始化
//======================================================================
uint_8 ueupdate_init();

//======================================================================
//函数名称：ueupdate_send
//函数参数：order:命令字节
//函数返回：0：终端更新信息发送至上位机成功;1:传入的命令字节错误
//功能概要：根据传入的命令字节来组装对应的数据，并发送至上位机告知当前更新状态
//      命令字                       对应的状态或操作
//  上位机->终端：
//       0            更新操作提示
//       1            写flash操作
//       2            检查帧标志
//       3            更新操作
//  终端->上位机：
//       4            接收上位机命令成功
//       5            当前更新操作发生错误
//       6            检查缺失数据
//       7            其他
//======================================================================
uint_8 ueupdate_send(uint_8 order,uint_8 *senddata,uint_16 *sendLen);

//======================================================================
//函数名称：ueupdate_recv
//函数参数：buf:接收到的上位机数据;len:接收到的上位机数据长度
//函数返回：0：上位机数据接收并操作成功;
//       1:数据解帧失败;2:接收到的数据不符合结构体updateData;3:总帧数错误;
//       4:还有未接收的帧;5:命令未知错误
//功能概要：接收上位机发送的数据，并进行数据解析，按照上位机命令进行对应的操作。
//      命令字                       对应的状态或操作
//  上位机->终端：
//       0            更新操作提示
//       1            写flash操作
//       2            检查帧标志
//       3            更新操作
//  终端->上位机：
//       4            接收上位机命令成功
//       5            当前更新操作发生错误
//       6            检查缺失数据
//       7            其他
//======================================================================
uint_8 ueupdate_recv(uint_8 *buf,uint_16 len);

#endif
