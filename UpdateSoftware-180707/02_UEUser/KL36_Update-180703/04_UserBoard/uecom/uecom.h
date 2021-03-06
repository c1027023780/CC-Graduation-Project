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
//#include "uart.h"
//#include "string.h"
//#include "gpio.h"

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
//=======================宏定义结束=====================================

//=====================================================================
//函数名称：uecom_init
//函数返回： 0：成功；1：与UE模块串口通讯失败；4：IMEI查询失败；5：IMSI查询失败；
//                   11:建立TCP连接失败
//参数说明： dest：返回数据的地址，总长74字节，IMEI,15个字节，0~14；
//                    IMSI,15个字节，15~29；
//                    IP:管理服务器的IP地址
//                    PORT:管理服务器的端口号
//功能概要：uecom模块初始化。本函数在完成MCU与通讯模块的基本通讯功能之后，执行附着核心网
//        操作，并与传入的IP地址和端口号建立TCP连接。在执行本构件中的其他函数之前必须
//        调用本函数或者uecom_fast_init函数。函数耗时约15秒。
//内部调用：uecom_fast_init，uecom_config
//修改日期：2017.08.11,QHJ,CC；2017.09.25，WB；2018.05.23，WB；
//备        注：返回值不连续，是为了所有AHL版本的返回信息一致
//=====================================================================
//uint_8 uecom_init(uint_8 *dest,uint_8 *IP,uint_8* PORT);
typedef  uint_8 (*fun_uecom_init)(uint_8 *dest,uint_8 *IP,uint_8* PORT);
volatile fun_uecom_init uecom_init;

//=====================================================================
//函数名称：uecom_fast_init
//函数返回： 0：成功；1：与UE模块串口通讯失败；2：UE模块的波特率设置失败；3：关闭回显失败；
//参数说明： 无
//功能概要：uecom模块快速初始化。本函数完成MCU与通讯模块的基本通讯功能，在执行本构件中的
//        其他函数之前必须调用本函数或者uecom_init函数。函数耗时约12秒。
//内部调用：uart_init，uecom_sendCmd
//修改日期：2017.09.28，WB；
//=====================================================================
//uint_8 uecom_fast_init(void);
typedef  uint_8 (*fun_uecom_fast_init)(void);
volatile fun_uecom_fast_init uecom_fast_init;

//=====================================================================
//函数名称：uecom_deInit
//函数返回：无
//参数说明：无
//功能概要：UE反初始化。关闭串口中断，取消串口的引脚复用，并关闭UE模块的供电。
//内部调用：uart_enable_re_int，uecom_switch
//修改日期：2017.09.22,QHJ,CC；2017.09.25，WB；
//=====================================================================
//void uecom_deInit(void);
typedef  void (*fun_uecom_deInit)(void);
volatile fun_uecom_deInit uecom_deInit;

//=====================================================================
//函数名称：uecom_switch
//函数返回： 0：操作成功；1：操作失败
//参数说明： state:UE模块供电状态。可取值为UECOM_OFF：关闭。
//                    UECOM_ON：关闭。UECOM_REBOOT：重启。
//功能概要：控制通信模块供电状态
//内部调用：gpio_init，uecom_delay_ms
//修改日期：2017.09.25,WB,CC
//=====================================================================
//uint_8 uecom_switch(uint_8 state);
typedef  void (*fun_uecom_switch)(uint_8 state);
volatile fun_uecom_switch uecom_switch;

//=====================================================================
//函数名称：uecom_send
//函数返回：  0：发送成功；1：发送失败；2：返回超时（可能未正确开启发送模式）
//参数说明： data:待发送数据缓存区，传入参数
//		           length:待发送数据的长度
//功能概要：将数据通过已经建立的TCP/UDP通道发送出去
//内部调用：uint_to_str，uart_send_string，uecom_delay_ms，uart_sendN，
//       uart_init，uart_enable_re_int，uart_disable_re_int，
//       uart_re1，uart_send1，uart_enable_re_int
//修改日期：2017.08.11,QHJ,CC；2017.09.25，WB；
//=====================================================================
//uint_8 uecom_send(uint_16 length, uint_8 *data);
typedef  uint_8 (*fun_uecom_send)(uint_16 length, uint_8 *data);
volatile fun_uecom_send uecom_send;

//==================================================================
//函数名称：uecom_getIMEI
//函数返回： 0：获取IMEI成功；1：获取IMEI失败
//参数说明：dest：存放返回的IMEI号，15位
//功能概要：获取设备IMEI号
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//==================================================================
//uint_8 uecom_getIMEI(uint_8 *dest);
typedef  uint_8 (*fun_uecom_getIMEI)(uint_8 *dest);
volatile fun_uecom_getIMEI uecom_getIMEI;

//==================================================================
//函数名称：uecom_getIMSI
//函数返回：0：获取IMSI成功；1：获取IMSI失败
//参数说明：dest：存放返回的IMSI号，15位
//功能概要：获取设备IMSI号
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//==================================================================
//uint_8 uecom_getIMSI(uint_8 *dest);
typedef  uint_8 (*fun_uecom_getIMSI)(uint_8 *dest);
volatile fun_uecom_getIMSI uecom_getIMSI;

//===================================================================
//函数名称：uecom_getFirmVer
//函数返回：0：获取固件版本号成功；1：获取固件版本号失败
//参数说明：firmVer：存放返回的固件版本号，25位
//功能概要：获取固件版本号，用于统一接口，并没有实现
//内部调用：无
//修改日期：2017.08.11,QHJ,CC
//==================================================================
//uint_8 uecom_getFirmVer(uint_8 *firmVer);
typedef  uint_8 (*fun_uecom_getFirmVer)(uint_8 *firmVer);
volatile fun_uecom_getFirmVer uecom_getFirmVer;

//=====================================================================
//函数名称：uecom_getSignalPower
//函数返回：0：获取基站信号强度成功；1：获取基站信号强度失败
//参数说明：signalPower：存放返回的基站信号强度号，百分比格式
//功能概要：获取基站信号强度
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
//uint_8 uecom_getSignalPower(uint_8 *signalPower);
typedef  uint_8 (*fun_uecom_getSignalPower)(uint_8 *signalPower);
volatile fun_uecom_getSignalPower uecom_getSignalPower;

//=====================================================================
//函数名称：uecom_getBSNum
//函数返回：0：获取基站号成功；1：获取基站号失败
//参数说明：bsNum：存放返回的基站号，2位
//功能概要：仅用于统一接口
//内部调用：无
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
//uint_8 uecom_getBSNum(uint_32 *bsNum);
typedef  uint_8 (*fun_uecom_getBSNum)(uint_32 *bsNum);
volatile fun_uecom_getBSNum uecom_getBSNum;

//=====================================================================
//函数名称：uecom_getUEIP
//函数返回：0：获取基站分配UE的IP地址成功；1：获取基站分配UE的IP地址失败
//参数说明：ip：存放返回基站分配UE的IP地址，15位
//功能概要：获取基站分配UE的IP地址
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
//uint_8 uecom_getUEIP(uint_8 *ip);
typedef  uint_8 (*fun_uecom_getUEIP)(uint_8 *ip);
volatile fun_uecom_getUEIP uecom_getUEIP;

//=====================================================================
//函数名称：uecom_config
//函数返回：  0：成功建立TCP连接；
//       1：建立TCP连接失败
//参数说明： IP:待连接服务器的IP地址
//        port:待连接服务器的端口号
//功能概要：与指定的服务器端口建立TCP连接
//内部调用：uecom_sendCmd，uart_send_string，delay_ms
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
//uint_8 uecom_config(uint_8 *ip,uint_8* port);
typedef  uint_8 (*fun_uecom_config)(uint_8 *ip,uint_8* port);
volatile fun_uecom_config uecom_config;

//====================================================================
//函数名称：uecom_getCurCommunity
//函数返回：0：获取当前小区成功；1：获取当前小区失败
//参数说明：data：存放返回的小区描述信息。格式为:mcc(移动国家号码),mnc(移动网络号码),
//                   lac(位置区域码),cell id(小区号),bsic(基站标示码)。例如:460,000,5280,e467,32
//                   注：其中lac和cell id为十六进制表示。data数组最少占25个字节
//功能概要：获得当前的小区信息
//修改日期：【2018-03-09】增（WB）
//=====================================================================
//uint_8 uecom_getCurCommunity(uint_8 *data);
typedef  uint_8 (*fun_uecom_getCurCommunity)(uint_8 *data);
volatile fun_uecom_getCurCommunity uecom_getCurCommunity;

//====================================================================
//函数名称：http_get
//函数返回：0：获得get请求成功；1：初始化http失败；2：传递url参数失败；3：设置网络失败；
//                   4：开启网络失败；5：建立连接失败；6：发送请求失败；7：获得返回失败；
//参数说明：data：存放返回的小区描述信息。格式为:mcc(移动国家号码),mnc(移动网络号码),
//                   lac(位置区域码),cell id(小区号),bsic(基站标示码)。例如:460,000,5280,e467,32
//                   注：其中lac和cell id为十六进制表示。data数组最少占25个字节
//功能概要：获得当前的小区信息
//修改日期：【2018-03-09】增（WB）
//=====================================================================
//uint_8 http_get(uint_8 ip[],uint_8 port[],uint_8 url[],uint_8 result[]);
typedef  uint_8 (*fun_http_get)(uint_8 ip[],uint_8 port[],uint_8 url[],uint_8 result[]);
volatile fun_http_get http_get;

//====================================================================
//函数名称：uecom_interrupt
//函数返回：无
//参数说明：ch：串口中断接收到的数据；length：0表示未收到gprs数据，非0表示接收到的数据长度
//                   recvData：存储接收到的gprs数据
//功能概要：本函数需要放在串口中断中。并需要传入串口中断接收到的数据。本构件的功能实现依赖
//                   于本函数。
//修改日期：【2018-03-09】增（WB）
//=====================================================================
//void uecom_interrupt(uint_8 ch,uint_16 *length,uint_8 recvData[]);
typedef  void (*fun_uecom_interrupt)(uint_8 ch,uint_16 *length,uint_8 recvData[]);
volatile fun_uecom_interrupt uecom_interrupt;

//======================================================================
//函数名称：uecom_default_delay_ms
//函数返回：无
//参数说明：无
//功能概要：延时 - 毫秒级
//======================================================================
//void uecom_default_delay_ms(uint_16 mseconds);
typedef  void (*fun_uecom_default_delay_ms)(uint_16 mseconds);
volatile fun_uecom_default_delay_ms uecom_default_delay_ms;

//=====================================================================
//函数名称：gnss_init
//函数返回： 无
//参数说明： 无
//功能概要：控制通信模块供电状态
//修改日期：2018.03.05,WB
//=====================================================================
//void gnss_init(uint_8 way);
typedef  void (*fun_gnss_init)(uint_8 way);
volatile fun_gnss_init gnss_init;

//=====================================================================
//函数名称：gnss_get_time
//函数返回：0：获得UTC时间成功；1：发送AT指令失败；2：获得UTC时间失败。
//参数说明： 初始为不少于14字节的字符串，存储当前的UTC时间，格式为：年月日时分秒。
//                    如：20180305055948。本地为东八区，小时应该+8
//功能概要：获得当前的UTC时间
//修改日期：2018.03.05,WB
//=====================================================================
//uint_8 gnss_get_time(char * time);
typedef  uint_8 (*fun_gnss_get_time)(char * time);
volatile fun_gnss_get_time gnss_get_time;

//=====================================================================
//函数名称：gnss_get_speed
//函数返回：0：获得速度成功；1：发送AT指令失败；2：获得速度失败。
//参数说明： 初始为不少于10字节的字符串，存储当前的速度。单位为：节（1节=0.5144444米/秒）
//功能概要：获得当前的速度
//修改日期：2018.03.05,WB
//=====================================================================
//uint_8 gnss_get_speed(double * speed);
typedef  uint_8 (*fun_gnss_get_speed)(double * speed);
volatile fun_gnss_get_speed gnss_get_speed;

//=====================================================================
//函数名称：gnss_get_location
//函数返回：0：获得位置成功；1：发送AT指令失败；2：获得位置失败。
//参数说明： latitude ：存储解析出的纬度信息（double类型）；
//                    longitude：存储解析出的经度信息（double类型）；
//功能概要：获得当前所在的位置
//修改日期：2018.03.05,WB
//=====================================================================
//uint_8 gnss_get_location(double *latitude, double *longitude);
typedef  uint_8 (*fun_gnss_get_location)(double *latitude, double *longitude);
volatile fun_gnss_get_location gnss_get_location;

//=====================================================================
//函数名称：gnss_get_attitude
//函数返回：0：获得海拔成功；1：发送AT指令失败；2：获得海拔失败。
//参数说明： 初始为不少于10字节的字符串，存储当前的海拔高度。单位为：米
//功能概要：获得当前的海拔高度
//修改日期：2018.03.05,WB
//=====================================================================
//uint_8 gnss_get_attitude(double *attitude);
typedef  uint_8 (*fun_gnss_get_attitude)(double *attitude);
volatile fun_gnss_get_attitude gnss_get_attitude;

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
