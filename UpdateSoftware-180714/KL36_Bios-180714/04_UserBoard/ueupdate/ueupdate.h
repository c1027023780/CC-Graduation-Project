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

//插入帧,总字节486
#pragma  pack(push,1)
struct insertData
{
	uint_16 frameNum;     //总帧数
	uint_16 curFrame;     //当前帧数
	uint_8 operate;       //操作，0：复制，1：插入
	uint_8 insertNum;       //插入代码个数，最大70
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

//======================================================================
//函数名称：ueupdate_init
//函数参数：无
//函数返回：无
//功能概要：实现终端程序更新功能初始化，本构件使用串口作为更新方式。
//       具体操作为串口和flash模块的初始化和帧标志初始化
//======================================================================
uint_8 ueupdate_init(uint_8 userSector);

//======================================================================
//函数名称：ueupdate_back
//函数参数：buf:返回上位机数据;len:返回上位机数据长度
//函数返回：0：有数据返回 ; 1:无数据返回
//功能概要：根据接收到的上位机数据返回上位机终端执行状况
//      命令字                       对应的状态或操作
//  上位机->终端：
//       0            更新操作提示
//       1            写flash操作
//       2            检查帧标志
//       3            更新操作
//  终端->上位机：
//       1            终端数据接收成功，返回当前帧号
//       2            检查操作，提示上位机丢失帧
//======================================================================
uint_8 ueupdate_back(uint_8 *buf,uint_16 *len);

//======================================================================
//函数名称：ueupdate_recv
//函数参数：buf:接收到的上位机数据;len:接收到的上位机数据长度
//函数返回：0：上位机数据接收并操作成功;
//       1:解帧失败;2:总帧数错误;3:提示代码更新完毕;4:未知命令
//功能概要：接收上位机发送的数据，并进行数据解析，按照上位机命令进行对应的操作。
//      命令字                       对应的状态或操作
//  上位机->终端：
//       0            更新操作提示
//       1            写flash操作
//       2            检查帧标志
//       3            更新操作
//  终端->上位机：
//       1            终端数据接收成功，返回当前帧号
//       2            检查操作，提示上位机丢失帧
//======================================================================
uint_8 ueupdate_recv(uint_8 *buf,uint_16 len);

#endif
