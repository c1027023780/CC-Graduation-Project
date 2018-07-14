//=====================================================================
//文件名称：uecom.h
//功能概要：UE驱动构件头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2018-05-23   V2.0
//备        注：适用于M6220
//=====================================================================

#ifndef _UECOM_H    //防止重复定义（_COMMON_H  开头)
#define _UECOM_H

# include <stdio.h>
# include <stdlib.h>

#include "common.h"   //包含公共要素头文件
#include "uart.h"
#include "string.h"
#include "gpio.h"

//=======================构件使用步骤开始================================
//（1）根据实际项目需要配置本文件中的宏。
//（2）将本构件中的uecom_interrupt函数放入相应的串口中断中，并设置合适的入口参数。
//（3）调用uecom_init函数完成初始化.此过程约耗时15秒。
//（4）在有发送请求时，调用uecom_send函数发送数据。
//（5）通过判断uecom_interrupt的length是否为零判断是否接收到数据。
//（6）若需要使用gnss（定位）功能，则应首先调用gnss_init，然后再使用其他gnss相关函数。
//（7）其他功能请参考本文件中的接口说明。
//=======================构件使用步骤结束================================

//=======================宏定义开始=====================================
#define UART_UE                            UART_1                //定义本构件与模块通信使用的串口
#define POWER_CONTROL         (PTE_NUM|22)   //控制通信模块电源的引脚
//定义延时函数。在操作系统情况下，可改为让出使用权的延时。
#define uecom_delay_ms(x)		Delay_ms(x)
//若要使用内部默认延时，请注释上一行代码，并取消此处的注释
//#define uecom_delay_ms(x)		uecom_default_delay_ms(x)

//==========uecom_switch的传入参数=======================================
#define UECOM_OFF                    0                           //关闭电源
#define UECOM_ON                     1                           //开启电源
#define UECOM_REBOOT            2                           //重启电源

//==========使用到的结构体的定义=============================================
typedef struct UecomGnssInfo{
	uint_8 time[15];         //UTC时间
	double latitude;        //纬度
	double longitude;    //经度
	double speed;         //速度
	double attitude;      //海拔
} UecomGnssInfo;
//=======================宏定义结束=====================================

//=====================================================================
//函数名称：uecom_power
//函数返回：无
//参数说明：state:通信模组电源控制命令。可取值为（使用宏常数）：
//         UECOM_OFF：关闭通信模组电源；
//         UECOM_ON：打开通信模组电源；
//         UECOM_REBOOT：重启通信模组（先关闭，延时，再开启）。
//功能概要：控制通信模组供电状态
//修改日期：【20180610】
//=====================================================================
//void uecom_power(uint_8 state);        //控制通信模组的电源
typedef  void (*fun_uecom_power)(uint_8 state);
volatile fun_uecom_power uecom_power;

//=====================================================================
//函数名称：uecom_init
//函数返回： 0：成功；1：与UE模组串口通讯失败；2：获取SIM卡的IMSI号失败；
//参数说明： 无
//功能概要：uecom模组初始化。（1）建立与通讯模组的串口通讯；（2）获取SIM卡的IMSI号。
//修改日期：【20180706】WB；
//=====================================================================
//uint_8 uecom_init(void);                   //初始化通信模组
typedef  uint_8 (*fun_uecom_init)(void);
volatile fun_uecom_init uecom_init;

//=====================================================================
//函数名称： uecom_ linkBase
//函数返回： 0：成功建立TCP连接；1：连接不上铁塔；
//参数说明：无
//功能概要：与网络运营商的基站（铁塔）建立连接
//修改日期：【20180706】WB；
//=====================================================================
//uint_8 uecom_linkBase(void);         //与基站建立连接
typedef  uint_8 (*fun_uecom_linkBase)(void);
volatile fun_uecom_linkBase uecom_linkBase;

//=====================================================================
//函数名称：uecom_linkCS
//函数返回： 0：成功建立TCP连接；1：建立TCP连接失败
//参数说明： IP:待连接服务器的IP地址； port:待连接服务器的端口号
//功能概要：与指定的服务器和端口建立TCP连接。
//修改日期：【20180706】WB；
//=====================================================================
//uint_8 uecom_linkCS(uint_8 *ip,uint_8* port);          //与云服务器建立连接
typedef  uint_8 (*fun_uecom_linkCS)(uint_8 *ip,uint_8* port);
volatile fun_uecom_linkCS uecom_linkCS;

//=====================================================================
//函数名称：uecom_send
//函数返回：  0：发送成功；1：发送失败；2：返回超时（可能未正确开启发送模式）
//参数说明： data:待发送数据缓存区，传入参数
//		           length:待发送数据的长度
//功能概要：将数据通过已经建立的TCP/UDP通道发送出去。最多500个字节。
//修改日期：【20180706】WB；
//=====================================================================
//uint_8 uecom_send(uint_16 length, uint_8 *data);    //发送数据
typedef  uint_8 (*fun_uecom_send)(uint_16 length, uint_8 *data);
volatile fun_uecom_send uecom_send;

//====================================================================
//函数名称：uecom_interrupt
//函数返回：无
//参数说明：ch：串口中断接收到的数据；length：接收到的网络数据长度；
//                recvData：存储接收到的网络数据
//功能概要：本函数需要放在串口中断中。并需要传入串口中断接收到的数据。
//  本构件的所有功能实现均依赖该api。本api实现的功能：
//  （1）接收网络发送来的数据;（2）构件内部使用本api与模块进行数据交互
//修改日期：【20180309】WB；
//=====================================================================
//void uecom_interrupt(uint_8 ch,uint_16 *length,uint_8 recvData[]);    //应放置在串口中的函数
typedef void(*fun_uecom_interrupt)(uint_8 ch,uint_16 *length,uint_8 recvData[]);
volatile fun_uecom_interrupt uecom_interrupt;

//====================================================================
//函数名称：uecom_baseLocation
//函数返回：0：获取基站信息成功；1：获取信号强度失败；2：获取基站号失败
//参数说明：retData：存储返回的信息,最少分配20个字节。
//          信息组成：信号强度(1个字节)+基站号（19个字节）
//功能概要：获取与基站相关的信息：信号强度和基站号
//修改日期：【20180309】WB；
//=====================================================================
//uint_8 uecom_baseInfo (uint_8 retData [20]);                                     //获得基站的相关信息
typedef uint_8(*fun_uecom_baseInfo)(uint_8 retData [20]);
volatile fun_uecom_baseInfo uecom_baseInfo;

//====================================================================
//函数名称：uecom_ modelInfo
//函数返回：0：获取模组信息成功；1：获取模组信息失败
//参数说明：retData：存储返回的信息,最少分配40个字节。
//          信息组成：IMSI (20个字节)+IMEI（20个字节）
//功能概要：获得需要与模块相关的信息，包括：IMEI号，IMSI号
//修改日期：【20180309】WB；
//=====================================================================
//uint_8 uecom_modelInfo (uint_8 retData[40]);                                   //获得通信模组的相关信息
typedef uint_8(*fun_uecom_modelInfo)(uint_8 retData [40]);
volatile fun_uecom_modelInfo uecom_modelInfo;

//=====================================================================
//函数名称：uecom_ gnssSwitch
//函数返回：0：操作GNSS成功；1：操作GNSS失败
//参数说明：state：设置GNSS的开关状态。0：关闭；1：热启动；2：温启动；3：冷启动；
//          建议默认使用冷启动。
//功能概要：设置GNSS的状态。开启或关闭GNSS，并设定开启方式。
//修改日期：【20180707】WB；
//=====================================================================
//uint_8 uecom_gnssSwitch (uint_8 state);
typedef uint_8(*fun_uecom_gnssSwitch)(uint_8 state);
volatile fun_uecom_gnssSwitch uecom_gnssSwitch;

//====================================================================
//函数名称：uecom_ gnssGetInfo
//函数返回：0：获取信息成功；1：与通信模组通信失败；2：获取UTC时间失败；
//                 3：获取定位信息失败；4：获取速度信息失败；5：获取海拔信息失败
//参数说明：data：存储获得的GNSS相关信息。采用结构体的方式，共包含4个成员：
//          time（15个字节的uint_8数组）,例如:“20180706155132”表示2018年7月6日15:51:32
//          latitude（double类型），纬度信息；longitude（double），经度信息；
//          speed（double类型），速度，单位为：米每秒。
//          attitude（double类型），海拔高度，单位为：米
//功能概要：获得与GNSS定位相关的信息。
//=====================================================================
//uint_8 uecom_gnssGetInfo (UecomGnssInfo *data);     // UecomGnssInfo为存储gnss信息的结构体数据
typedef uint_8(*fun_uecom_gnssGetInfo)(UecomGnssInfo *data);
volatile fun_uecom_gnssGetInfo uecom_gnssGetInfo;

//====================================================================
//函数名称：http_get
//函数返回：0：获得get请求成功；1：初始化http失败；2：传递url参数失败；3：设置网络失败；
//                4：开启网络失败；5：建立连接失败；6：发送请求失败；7：获得返回失败；
//参数说明：ip:目标服务器地址；port :目标地址；url:get请求的内容。result:get请求返回的结果，
//                数组长度由预计返回的长度（用户应已知返回内容）*1.5来决定。
//功能概要：发起http的get请求，并将返回结果存储在result中
//修改日期：【2018-03-09】WB；
//=====================================================================
//uint_8 uecom_httpGet (uint_8 ip[],uint_8 port[],uint_8 url[],uint_8 result[]);
typedef uint_8(*fun_uecom_httpGet)(uint_8 ip[],uint_8 port[],uint_8 url[],uint_8 result[]);
volatile fun_uecom_httpGet uecom_httpGet;

//======================================================================
//函数名称：uecom_default_delay_ms
//函数返回：无
//参数说明：无
//功能概要：延时 - 毫秒级
//======================================================================
//void uecom_default_delay_ms(uint_16 mseconds);
typedef void(*fun_uecom_default_delay_ms)(uint_16 mseconds);
volatile fun_uecom_default_delay_ms uecom_default_delay_ms;

#endif
/*
知识要素：
1、通信模块采用的是串口通信，波特率为115200（有可能随着硬件版本升级而有所改变）
 注意：在开启串口中断之前，不可以关闭串口中断，否则程序会跑飞。
2、通讯模块采用AT指令进行控制
3、通讯模块的供电引脚在本文件中已给出，电压范围为：3.4V - 4.2V。3.9V较佳
4、初始化通讯模块时需要传入管理服务器IP地址和端口号
5、信号强度对于模块联网很重要。一般情况，信号强度应在百分之二十才可以实现通信
6、IMSI和IMEI是该通信模块的两个重要标志。IMSI号是该模块的卡号，是通信公司分配的
用来记录信息并缴费的凭证。IMEI号是该模块的编号，是模块的生产商决定的。
7、本模块采用的TCP通信方式，相比于UDP方式，该方式更加安全可靠。
8、通信模块附着核心网和发送数据所用的时间与信号强度成反比。信号强度越强，所用时间越少。

 */
