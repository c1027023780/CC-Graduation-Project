//=====================================================================
//文件名称：isr.c
//功能概要： 中断底层驱动构件源文件
//版权所有：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//更新记录：2017-04-07   V1.0
//=====================================================================
#include "includes.h"

//========================中断函数服务例程===============================

//======================================================================
//ISR名称：LPTMR0_IRQHandler（LPTMR0中断服务例程）
//ISR功能：每500毫秒执行本例程一次，清中断标志，并完成计时。)
//         即秒+1（全局变量gcd.currentTime++）；
//         全局变量（年月日时分秒)gTime[14]跟随改变
//修改日期：2017.8.11（WYH）
//======================================================================
void LPTMR0_IRQ(void)
{
	static uint_8 count = 0;
	DISABLE_INTERRUPTS;        //关总中断
	CLEAR_LPTMR_FLAG;          //清中断标志标志
	//--------------------------------
	count++;
	if(count >= 2)
	{
		count = 0;
		gUserData.currentTime++;                    //秒+1
	}
	BSET(TSI_GENCS_TSIEN_SHIFT,TSI0_GENCS);
    //--------------------------------
    ENABLE_INTERRUPTS;       //开总中断
}


//======================================================================
//函数名称：TSI0_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：TSI中断服务例程。清中断标志，并完成TSI触摸功能
//======================================================================
void TSI0_IRQ(void)
{
	//存放数值的临时变量
	uint_16 i;
	uint_8 touchNum[10];
	DISABLE_INTERRUPTS;      //关总中断
	//-------------------------------

	if(TSI0_GENCS & TSI_GENCS_OUTRGF_MASK)//为超值中断
	{
		//获取计数值
//		i = tsi_get_value16();
		i = (TSI0_DATA & TSI_DATA_TSICNT_MASK);      //读取计数寄存器中的值;
		if(i>500)
		{
			gUserData.touchNum++;
            //LCD_ShowString(36,209,BLUE,GRAY,(uint_8 *)myItoa(gUserData.touchNum,touchNum,10));
		}
	}
	//写1清溢出中断位
	TSI0_GENCS |=  TSI_GENCS_OUTRGF_MASK;//写1清楚该位
	BCLR(TSI_GENCS_TSIEN_SHIFT,TSI0_GENCS);
	//-------------------------------
	ENABLE_INTERRUPTS;       //开总中断
}

//======================================================================
//函数名称：UART0_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：串口0接收中断服务例程
//======================================================================
void UART0_IRQHandler(void)
{
	uint_8 ch;
	uint_8 flag;
	DISABLE_INTERRUPTS;      //关总中断
	//-------------------------------
	if(uart_get_re_int(UART_0))
	{
		ch = uart_re1 (UART_0, &flag);    //调用接收一个字节的函数，清接收中断位
		if(flag)
		{
			uart_send1(UART_0, ch);     //向原串口发回一个字节
		}
	}
	//-------------------------------
	ENABLE_INTERRUPTS;       //开总中断
}

//======================================================================
//函数名称：UART1_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：串口1接收中断服务例程
//======================================================================
void UART1_IRQHandler(void)
{
	uint_8 ch;
	uint_8 flag;
	DISABLE_INTERRUPTS;      //关总中断
	//-------------------------------
    //接收一个字节
	ch = uart_re1(UART_1, &flag);   //调用接收一个字节的函数，清接收中断位
	if(flag)   //有数据
	{
		uecom_interrupt(ch,&gRecvLength,gRecvBuf);
	}

	//-------------------------------
	ENABLE_INTERRUPTS;       //开总中断
}

//======================================================================
//函数名称：UART2_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：串口2接收中断服务例程
//======================================================================
//void UART2_IRQHandler(void)
//{
//	uint_8 ch;
//	uint_8 flag;
//	//关总中断
//	DISABLE_INTERRUPTS;      //关总中断
//	//-------------------------------
//	if(uart_get_re_int(UART_2))
//	{
//		ch = uart_re1(UART_2, &flag);   //调用接收一个字节的函数，清接收中断位
//		if(flag)
//		{
////			uart_send1(UART_1, ch);     //向原串口发回一个字节
//		}
//	}
//	//-------------------------------
//	ENABLE_INTERRUPTS;       //开总中断
//}

/*
 知识要素：
 1.本文件中的中断处理函数名需要与启动文件（.s文件）中的函数名保持一致，
 如果在移植系统时，出现启动文件中的中断处理函数名和本文件中的中断处理函数
 名不一致的情况时，则应该修改启动文件中的中断处理函数名，使其与本文件中的
 中断处理函数名一致。这样就可以保证本文件的环境无关性。
 2.本文件中的中断处理函数调用的均是相关设备封装好的具体构件，在更换芯片
 时，只需保证设备的构件接口一致，即可保证本文件的相关中断处理函数不做任何
 更改，从而达到芯片无关性的要求。
 */
