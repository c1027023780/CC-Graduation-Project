//=====================================================================
//文件名称：SPI.h
//功能概要：SPI头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2016-03-17 
//=====================================================================
#ifndef _SPI_H             //防止重复定义（ 开头)
#define _SPI_H

#include "common.h"      //包含公共要素头文件
//宏定义：定义SPI口号。
#define SPI_0  0     //SPI0口
#define SPI_1  1     //SPI1口

//根据SPI实际硬件引脚，确定以下宏常量值。
//在此工程中，我们只使用SPI0组中的第2个，SPI1组中的第2个，
//因此我们在此只需要将SPI_0_GROUP宏定义为2,SPI_1_GROUP宏定义为2。

//SPI_0：1=PTD0、1、2、3脚，2=PTA14、15、16、17脚，3=PTC4、5、6、7脚
#define SPI_0_GROUP  3   //SD-FSL-KL25-EVB板上使用PTA14~17脚

//SPI_1：1=PTE1、2、3、4脚，2=PTB10、11、16、17脚，3=PTD4、5、6、7脚
#define SPI_1_GROUP  3    //SD-FSL-KL25-EVB板上使用PTB10、11、16、17脚


#define SPI_baseadd(SPI_nub) (SPI_MemMapPtr)(0x40076000u+SPI_nub*0x00001000u)
//=====================================================================
//函数名称：SPI_init。
//功能说明：SPI初始化
//函数参数：No：模块号，KL25芯片取值为0、1
//       MSTR：SPI主从机选择，0选择为主机,1选择为从机。
//       BaudRate：波特率，可取12000、6000、4000、3000、1500、1000，单位：b/s
//       CPOL：CPOL=0：高有效SPI时钟（低无效）；CPOL=1：低有效SPI时钟（高无效）
//       CPHA：CPHA=0相位为0； CPHA=1相位为1；
//函数返回：无
//函数备注： CPHA=0，时钟信号的第一个边沿出现在8周期数据传输的第一个周期的中央；
//        CPHA=1，时钟信号的第一个边沿出现在8周期数据传输的第一个周期的起点。
//        CPHA=0时，通信最稳定，即接收方在1/2周期的时刻从线上取数。
//=====================================================================
//void SPI_init(uint_8 No,uint_8 MSTR,uint_16 BaudRate,uint_8 CPOL,\
//                                                         uint_8 CPHA);
typedef  void (*fun_SPI_init)(uint_8 No,uint_8 MSTR,uint_16 BaudRate,uint_8 CPOL,uint_8 CPHA);
volatile fun_SPI_init SPI_init;

//=====================================================================
//函数名称：SPI_send1.
//功能说明：SPI发送一字节数据。
//函数参数：No：模块号。 其取值为0或1
//       data：     需要发送的一字节数据。
//函数返回：0：发送失败；1：发送成功。
//=====================================================================
//uint_8 SPI_send1(uint_8 No,uint_8 data);
typedef  uint_8 (*fun_SPI_send1)(uint_8 No,uint_8 data);
volatile fun_SPI_send1 SPI_send1;

//=====================================================================
//函数名称：SPI_sendN.
//功能说明：SPI发送数据。
//函数参数：No：模块号。 其取值为0或1
//       n:     要发送的字节个数。范围为(1~255)
//       data[]:所发数组的首地址。
//函数返回：无。
//=====================================================================
//void SPI_sendN(uint_8 No,uint_8 n,uint_8 data[]);
typedef  void (*fun_SPI_sendN)(uint_8 No,uint_8 n,uint_8 data[]);
volatile fun_SPI_sendN SPI_sendN;

//=====================================================================
//函数名称：SPI_receive1.
//功能说明：SPI接收一个字节的数据
//函数参数：No：模块号。 其取值为0或1
//函数返回：接收到的数据。
//=====================================================================
//uint_8 SPI_receive1(uint_8 No);
typedef  uint_8 (*fun_SPI_receive1)(uint_8 No);
volatile fun_SPI_receive1 SPI_receive1;

//=====================================================================
//函数名称：SPI_receiveN.
//功能说明：SPI接收数据。当n=1时，就是接受一个字节的数据……
//函数参数：No：模块号。 其取值为0或1
//          n:    要发送的字节个数。范围为(1~255),
//       data[]:接收到的数据存放的首地址。
//函数返回：1：接收成功,其他情况：失败。
//=====================================================================
//uint_8 SPI_receiveN(uint_8 No,uint_8 n,uint_8 data[]);
typedef  uint_8 (*fun_SPI_receiveN)(uint_8 No,uint_8 n,uint_8 data[]);
volatile fun_SPI_receiveN SPI_receiveN;

//=====================================================================
//函数名称：SPI_enable_re_int
//功能说明：打开SPI接收中断。
//函数参数：No：模块号。 其取值为0或1。
//函数返回：无。
//=====================================================================
//void SPI_enable_re_int(uint_8 No);
typedef  void (*fun_SPI_enable_re_int)(uint_8 No);
volatile fun_SPI_enable_re_int SPI_enable_re_int;

//=====================================================================
//函数名称：SPI_disable_re_int
//功能说明：关闭SPI接收中断。
//函数参数：No：模块号。 其取值为0或1。
//函数返回：无。
//=====================================================================
//void SPI_disable_re_int(uint_8 No);
typedef  void (*fun_SPI_disable_re_int)(uint_8 No);
volatile fun_SPI_disable_re_int SPI_disable_re_int;

#endif   //防止重复定义（ 结尾)

//=====================================================================
//声明：
//（1）我们开发的源代码，在本中心提供的硬件系统测试通过，真诚奉献给社会，不足之处，
//     欢迎指正。
//（2）对于使用非本中心硬件系统的用户，移植代码时，请仔细根据自己的硬件匹配。
//
//苏州大学NXP嵌入式中心(http://sumcu.suda.edu.cn,0512-65214835)
