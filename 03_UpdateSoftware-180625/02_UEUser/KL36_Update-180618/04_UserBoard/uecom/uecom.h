//=====================================================================
//文件名称：uecom.h
//功能概要：UE驱动构件头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：【2017-06-04】   V1.0
//       【2018-03-09】  V1.1 增加uecom_getCurCommunity函数
//=====================================================================

#ifndef _UECOM_H    //防止重复定义（_COMMON_H  开头)
#define _UECOM_H

#include "common.h"   //包含公共要素头文件
#include "uart.h"
#include "string.h"
#include "lcd.h"
#include "gpio.h"
#include "wdog.h"    //包含wdog.h头文件

#define UART_UE                         UART_1
#define UECOM_OFF                 0
#define UECOM_ON                  1
#define UECOM_REBOOT         2
#define POWER_CONTROL      (PTE_NUM|22)

extern uint_16 gRecvLength;        //接收到的GPRS数据，为0时表示没有接收到，否则表示接收到的数据长度
extern uint_8 gRecvBuf[600];   //存储接收到的GPRS数据，最大1000个字节（因为帧结构占掉约24个字节）
//=====================================================================
//函数名称：uecom_switch
//函数返回： 0：操作成功；1：操作失败
//参数说明： state:UE模块供电状态。可取值为UECOM_OFF：关闭。
//        UECOM_ON：关闭。UECOM_REBOOT：重启。
//功能概要：控制通信模块供电状态
//内部调用：gpio_init，delay_ms
//修改日期：2017.09.25,WB,CC
//=====================================================================
uint_8 uecom_switch(uint_8 state);

//=====================================================================
//函数名称：uecom_init
//函数返回： 0：成功；1：与UE模块串口通讯失败；2：UE模块的波特率设置失败；3：关闭回显失败；
//        4：IMEI查询失败；5：IMSI查询失败；6：设置模块的工作类型为B类失败；
//        7：设置联网方式为net失败；8：激活PDP失败；9：获得模块分得的IP地址失败；
//        10：AT指令返回OK，但是并没有获取到IP地址；11:建立TCP连接失败
//参数说明： dest：返回数据的地址，总长74字节，IMEI,15个字节，0~14；
//             IMSI,15个字节，15~29；firmVer，25个字节，30~54；
//             signalPower，2个字节，55~56；bsNum，2个字节，57~58；
//             UEIP，15个字节，59~73
//       IP:管理服务器的IP地址
//       PORT:管理服务器的端口号
//功能概要：uecom模块初始化。本函数在完成MCU与通讯模块的基本通讯功能之后，执行附着核心网
//        操作，并与传入的IP地址和端口号建立TCP连接。在执行本构件中的其他函数之前必须
//        调用本函数或者uecom_fast_init函数。
//内部调用：uart_init，uecom_sendCmd，uecom_getIMEI，uecom_getIMSI，
//          uecom_link，uecom_config
//修改日期：2017.08.11,QHJ,CC；2017.09.25，WB；
//=====================================================================
uint_8 uecom_init(uint_8 *dest,uint_8 *IP,uint_8* PORT);

//=====================================================================
//函数名称：uecom_fast_init
//函数返回： 0：成功；1：与UE模块串口通讯失败；2：UE模块的波特率设置失败；3：关闭回显失败；
//参数说明： 无
//功能概要：uecom模块快速初始化。本函数完成MCU与通讯模块的基本通讯功能，在执行本构件中的
//        其他函数之前必须调用本函数或者uecom_init函数。
//内部调用：uart_init，uecom_sendCmd
//修改日期：2017.09.28，WB；
//=====================================================================
uint_8 uecom_fast_init(void);

//=====================================================================
//函数名称：uecom_deInit
//函数返回：无
//参数说明：无
//功能概要：UE反初始化。关闭串口中断，取消串口的引脚复用，并关闭UE模块的供电。
//内部调用：uart_enable_re_int，uecom_switch
//修改日期：2017.09.22,QHJ,CC；2017.09.25，WB；
//=====================================================================
void uecom_deInit(void);

//=====================================================================
//函数名称：uecom_send
//函数返回：  0：发送成功；1：发送失败；2：返回超时（可能未正确开启发送模式）
//参数说明： data:待发送数据缓存区，传入参数
//                   length:待发送数据的长度
//功能概要：将数据通过已经建立的TCP/UDP通道发送出去
//内部调用：uint_to_str，uart_send_string，delay_ms，uart_sendN，
//       uart_init，uart_enable_re_int，uart_disable_re_int，
//       uart_re1，uart_send1，uart_enable_re_int
//修改日期：2017.08.11,QHJ,CC；2017.09.25，WB；
//=====================================================================
uint_8 uecom_send(uint_16 length, uint_8 *data);

//==================================================================
//函数名称：uecom_getIMEI
//函数返回： 0：获取IMEI成功；1：获取IMEI失败
//参数说明：dest：存放返回的IMEI号，15位
//功能概要：获取设备IMEI号
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_getIMEI(uint_8 *dest);

//==================================================================
//函数名称：uecom_getIMSI
//函数返回：0：获取IMSI成功；1：获取IMSI失败
//参数说明：dest：存放返回的IMSI号，15位
//功能概要：获取设备IMSI号
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_getIMSI(uint_8 *dest);

//===================================================================
//函数名称：uecom_getFirmVer
//函数返回：0：获取固件版本号成功；1：获取固件版本号失败
//参数说明：firmVer：存放返回的固件版本号，25位
//功能概要：获取固件版本号，用于统一接口，并没有实现
//内部调用：无
//修改日期：2017.08.11,QHJ,CC
//==================================================================
uint_8 uecom_getFirmVer(uint_8 *firmVer);

//=====================================================================
//函数名称：uecom_getSignalPower
//函数返回：0：获取基站信号强度成功；1：获取基站信号强度失败
//参数说明：signalPower：存放返回的基站信号强度号，百分比格式
//功能概要：获取基站信号强度
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_getSignalPower(uint_8 *signalPower);

//=====================================================================
//函数名称：uecom_getBSNum
//函数返回：0：获取基站号成功；1：获取基站号失败
//参数说明：bsNum：存放返回的基站号，2位
//功能概要：仅用于统一接口
//内部调用：无
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_getBSNum(uint_32 *bsNum);

//=====================================================================
//函数名称：uecom_getUEIP
//函数返回：0：获取基站分配UE的IP地址成功；1：获取基站分配UE的IP地址失败
//参数说明：ip：存放返回基站分配UE的IP地址，15位
//功能概要：获取基站分配UE的IP地址
//内部调用：uecom_sendCmd
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_getUEIP(uint_8 *ip);

//====================================================================
//函数名称：uecom_getCurCommunity
//函数返回：0：获取当前小区成功；1：获取当前小区失败
//参数说明：data：存放返回的小区描述信息。格式为:mcc(移动国家号码),mnc(移动网络号码),
//                   lac(位置区域码),cell id(小区号),bsic(基站标示码)。例如:460,000,5280,e467,32
//                   注：其中lac和cell id为十六进制表示。data数组最少占25个字节
//功能概要：获得当前的小区信息
//修改日期：【2018-03-09】增（WB），
//=====================================================================
uint_8 uecom_getCurCommunity(uint_8 *data);

//=====================================================================
//函数名称：uecom_config
//函数返回：  0：成功建立TCP连接；
//       1：建立TCP连接失败
//参数说明： IP:待连接服务器的IP地址
//        port:待连接服务器的端口号
//功能概要：与指定的服务器端口建立TCP连接
//内部调用：uecom_sendCmd，uart_send_string，Delay_ms
//修改日期：2017.08.11,QHJ,CC
//=====================================================================
uint_8 uecom_config(uint_8 *ip,uint_8* port);

//====待删除
//=====================================================================
//函数名称：uecom_recv
//函数返回：0：接收到了通过TCP/UDP发来的数据；1：正在组帧；2接收数据错误
//参数说明：*data:将接收到的数据存放到该数组串中;
//                *dataLen:存储接收到的数据长度
//功能概要：将接收到的数据进行解帧，并将有效数据存入data数组之中，由串口的uart中断调用。
//内部调用：uart_init，uart_disable_re_int,uart_re1，
//修改日期：2017.08.11,QHJ,CC；2017.09.25，WB；
//=====================================================================
uint_8 uecom_recv(uint_8 ch,uint_16 *dataLen, uint_8 *data);
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
