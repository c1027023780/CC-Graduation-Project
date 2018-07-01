//===========================================================================
//文件名称：flash.h
//功能概要：flash底层驱动构件头文件
//版权所有：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//更新记录：2013-06-06 V1.0; 2016-06-06 V6.0
//适用芯片：KL25、KL26、KW01
//===========================================================================

#ifndef _FLASH_H
#define _FLASH_H

#include "common.h"

//==========================================================================
//函数名称：flash_init
//函数返回：无
//参数说明：无
//功能概要：flash初始化
//======================================================================
//void flash_init(void);
typedef  void (*fun_flash_init)(void);
volatile fun_flash_init flash_init;

//======================================================================
//函数名称：flash_erase_sector
//函数返回：函数执行执行状态：0=正常；1=异常。
//参数说明：sect：目标扇区号（范围取决于实际芯片，例如KL25：0~127，每扇区1KB）
//功能概要：擦除flash存储器的sect扇区（每扇区1KB）
//======================================================================
//uint_8 flash_erase(uint_16 sect);
typedef  uint_8 (*fun_flash_erase)(uint_16 sect);
volatile fun_flash_erase flash_erase;

//======================================================================
//函数名称：flash_write
//函数返回：函数执行状态：0=正常；1=异常。
//参数说明：sect：扇区号（范围取决于实际芯片，例如KL36：0~63，每扇区1KB）
//        offset:写入扇区内部偏移地址（0~1020，要求为0,4，8,12，......）
//        N：写入字节数目（4~1024,要求为4，8,12,......）
//        buf：源数据缓冲区首地址
//功能概要：将buf开始的N字节写入到flash存储器的sect扇区的 offset处
//======================================================================
//uint_8 flash_write(uint_16 sect,uint_16 offset,uint_16 N,uint_8 *buf);
typedef  uint_8 (*fun_flash_write)(uint_16 sect,uint_16 offset,uint_16 N,uint_8 *buf);
volatile fun_flash_write flash_write;

//======================================================================
//函数名称：flash_read_logic
//函数返回：无
//参数说明：sect：扇区号（范围取决于实际芯片，例如KL36：0~63，每扇区1KB）
//        offset:扇区内部偏移地址（0~1020，要求为0,4，8,12，......）
//        N：读字节数目（4~1024,要求为4，8,12,......）
//        dest：读出数据存放处（传地址，目的是带出所读数据，RAM区）
//功能概要：读取flash存储器的sect扇区的 offset处开始的N字节，到RAM区dest处
//========================================================================
//void flash_read_logic(uint_8 *dest,uint_16 sect,uint_16 offset,uint_16 N);
typedef  void (*fun_flash_read_logic)(uint_8 *dest,uint_16 sect,uint_16 offset,uint_16 N);
volatile fun_flash_read_logic flash_read_logic;

//========================================================================
//函数名称：flash_read_physical
//函数返回：无
//参数说明：addr：目标地址，要求为4的倍数（例如：0x00000004）
//        N：读字节数目（0~1020,要求为4，8,12,......）
//        dest：读出数据存放处（传地址，目的是带出所读数据，RAM区）
//功能概要：读取flash指定地址的内容
//========================================================================
//void flash_read_physical(uint_8 *dest,uint_32 addr,uint_16 N);
typedef  void (*fun_flash_read_physical)(uint_8 *dest,uint_32 addr,uint_16 N);
volatile fun_flash_read_physical flash_read_physical;

//========================================================================
//函数名称：flash_protect
//函数返回：无
//参数说明：M：待保护区域的扇区号入口值，实际保护M~M+3,其中M=0,4,8,......,124
//功能概要：flash保护操作
//说        明：每调用本函数一次，保护4个扇区（M~M+3）
//========================================================================
//void flash_protect(uint_8 M);
typedef  void (*fun_flash_protect)(uint_8 M);
volatile fun_flash_protect flash_protect;

//========================================================================
//函数名称：flash_isempty
//函数返回：1=目标区域为空；0=目标区域非空。
//参数说明：所要探测的flash区域初始地址及范围
//功能概要：flash判空操作
//========================================================================
//uint_8 flash_isempty(uint_8 *buff,uint_16 N);
typedef  uint_8 (*fun_flash_isempty)(uint_8 *buff,uint_16 N);
volatile fun_flash_isempty flash_isempty;

// Flash控制器宏定义 ，内部使用
#define CCIF    (1<<7)
#define ACCERR  (1<<5)
#define FPVIOL  (1<<4)
#define MGSTAT0 (1<<0)


//Flash命令宏定义，内部使用
#define RD1SEC    0x01   // 读整个扇区
#define PGMCHK    0x02   // 写入检查
#define RDRSRC    0x03   // 读目标数据
#define PGM4      0x06   // 写入长字
#define ERSBLK    0x08   // 擦除整块Flash
#define ERSSCR    0x09   // 擦除Flash扇区
#define RD1ALL    0x40   // 读所有的块
#define RDONCE    0x41   // 只读一次
#define PGMONCE   0x43   // 只写一次
#define ERSALL    0x44   // 擦除所有块
#define VFYKEY    0x45   // 验证后门访问钥匙

//======================================================================
#endif //_FLASH_H

//======================================================================
//声明：
//（1）我们开发的源代码，在本中心提供的硬件系统测试通过，真诚奉献给社会，
//    不足之处，欢迎指正。
//（2）对于使用非本中心硬件系统的用户，移植代码时，请仔细根据自己的硬件匹配。
//
//苏州大学嵌入式系统与物联网研究所,0512-65214835  http://sumcu.suda.edu.cn

