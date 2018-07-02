//===========================================================================
//文件名称：lptmr.c
//功能概要：lptmr底层驱动构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2017-5-23   V1.0
//===========================================================================
#include "lptmr.h"

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
void lptmr_init(uint_32 time)
{
    uint_16 compare_value;

    SIM_SCGC5|=SIM_SCGC5_LPTMR_MASK;//使能LPTMR模块时钟

    SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(0x01);//选择LPO时钟
    LPTMR0_PSR |= LPTMR_PSR_PCS(1)|LPTMR_PSR_PBYP_MASK;
    compare_value=time;

    LPTMR0_CMR = LPTMR_CMR_COMPARE(compare_value);      //设置比较寄存器值
    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK; //开启LPTMR模块设置
}

//============================================================================
//函数名称：lptmr_reset
//函数返回：无
//参数说明：无
//功能概要：LPTMR计时器清零
//============================================================================
void lptmr_reset(void)
{
    LPTMR0_CSR|=LPTMR_CSR_TCF_MASK;
}

//============================================================================
//函数名称：enable_lptmr_int
//函数返回：无
//参数说明：无
//功能概要：LPTMR模块使能，开启LPTMR模块中断及定时器中断
//调用举例：enable_lptmr_int();使能LPTMR模块中断及定时器中断
//============================================================================
void enable_lptmr_int(void)
{
    LPTMR0_CSR|=LPTMR_CSR_TIE_MASK;        //开启LPTMR定时器中断
    NVIC_EnableIRQ(LPTMR0_IRQn);           //开引脚的IRQ中断
}

//============================================================================
//函数名称：disable_lptmr_int
//函数返回：无
//参数说明：无
//功能概要：LPT定时器中断除能
//调用举例：disable_lptmr_int();关闭LPTMR模块中断及定时器
//============================================================================
void disable_lptmr_int(void)
{
    LPTMR0_CSR &=~LPTMR_CSR_TIE_MASK;      //禁止LPTMR定时器中断
    NVIC_DisableIRQ(LPTMR0_IRQn);          //关引脚的IRQ中断
}

