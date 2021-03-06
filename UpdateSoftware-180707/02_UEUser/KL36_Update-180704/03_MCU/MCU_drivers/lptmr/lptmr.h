//===========================================================================
//文件名称：lptmr.c
//功能概要：lptmr底层驱动构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2017-5-23   V1.0
//===========================================================================
#ifndef _LPTMR_H
#define _LPTMR_H
#include "common.h"

//LPTMR时钟源
#define LPOCLK     1
#define IRC32KCLK  2
#define IRC4MCLK   3
#define OSCERCLK   4

//确认中断标志
#define CONFIRM_LPTMR_FLAG  ((LPTMR0_CSR & LPTMR_CSR_TCF_MASK) == \
                                                        LPTMR_CSR_TCF_MASK)
//清中断标志
#define CLEAR_LPTMR_FLAG    (LPTMR0_CSR |= LPTMR_CSR_TCF_MASK)

//============================================================================
//函数名称：lptmr_init
//函数返回：无
//参数说明：time  定时器中断的时间间隔，单位为毫秒。
//功能概要：LPTMR模块初始化，配置LPTMR工作的时钟源，中断时间为1秒。时钟源可以配置为
//1KHz的LPO时钟、32KHz的IRC时钟、4MHz的IRC时钟和8Mhz 的OSCERCLK时钟，并且时钟源分
//频范围大，最小分频为1，最大分频65536。当计数器的值与比较值相等时就会产生中断。
//调用举例：lptmr_init(LPOCLK);配置LPTMR模块时钟源为LPOCLK时钟
//极限时钟频率：1000/65536Hz~8MHz
//极限中断时间间隔：1/8000000s~65.536*65536s，最小约为125ns，最大约为50天
//为统一所有时钟源，中断间隔统一设置成1s
//调整时间的方法：1KHzLPO时钟为例，即时钟频率为1000Hz，设置中断时间1秒的方法如下：
//不分频的情况，LPTMR0_CMR写1000即可；
//2分频的情况，那么获得的LPTMR的时钟为500Hz，故LPTMR0_CMR只需写500即可；
//其他以此类推
//============================================================================
//void lptmr_init(uint_32 time);
typedef  void (*fun_lptmr_init)(uint_32 time);
volatile fun_lptmr_init lptmr_init;

//============================================================================
//函数名称：lptmr_reset
//函数返回：无
//参数说明：无
//功能概要：LPTMR计时器清零
//============================================================================
//void lptmr_reset(void);
typedef  void (*fun_lptmr_reset)(void);
volatile fun_lptmr_reset lptmr_reset;

//============================================================================
//函数名称：enable_lptmr_int
//函数返回：无
//参数说明：无
//功能概要：开启LPT定时器中断
//调用举例：enable_lptmr_int();使能LPTMR模块中断及定时器中断
//============================================================================
//void enable_lptmr_int(void);
typedef  void (*fun_enable_lptmr_int)(void);
volatile fun_enable_lptmr_int enable_lptmr_int;

//============================================================================
//函数名称：disable_lptmr_int
//函数返回：无
//参数说明：无
//功能概要：关闭LPT定时器中断
//调用举例：disable_lptmr_int();关闭LPTMR模块中断及定时器
//============================================================================
//void disable_lptmr_int(void);
typedef  void (*fun_disable_lptmr_int)(void);
volatile fun_disable_lptmr_int disable_lptmr_int;

#endif

//===========================================================================
//声明：
//（1）我们开发的源代码，在本中心提供的硬件系统测试通过，真诚奉献给社会，不足之处，
//     欢迎指正。
//（2）对于使用非本中心硬件系统的用户，移植代码时，请仔细根据自己的硬件匹配。
//
//苏州大学飞思卡尔嵌入式中心（苏州华祥信息科技有限公司）
//技术咨询：0512-65214835  http://sumcu.suda.edu.cn
//业务咨询：0512-87661670,18915522016  http://www.hxtek.com.cn
