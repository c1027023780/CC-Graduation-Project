//=====================================================================
//文件名称：isr.c
//功能概要： 中断底层驱动构件源文件
//版权所有：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//更新记录：2017-04-07   V1.0
//=====================================================================
#include "includes.h"

extern uint_64 getTime(void);

//内部函数声明
uint_8 CreateFrame(uint_8 Data,uint_8 * buffer,uint_16 *len);

//========================中断函数服务例程===============================

void SysTick_Handler(void)
{
	gTime++;
}

//==================================================================
//ISR：UART0_IRQHandler（UART0中断服务例程）
//进入条件：串口0接收到数据
//功能概要：（1）清中断标志
//       （2）串口0返回接收到的数据
//==================================================================
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

//==================================================================
//ISR：UART1_IRQHandler（UART1中断服务例程）
//进入条件：串口1接收到数据
//功能概要：（1）清中断标志
//       （2）终端UE通信模块接收管理服务器数据
//==================================================================
void UART1_IRQHandler(void)
{
	uint_8 ch;
	uint_8 flag;
	DISABLE_INTERRUPTS;      //关总中断
	//-------------------------------
	if(uart_get_re_int(UART_1))
	{
		ch = uart_re1 (UART_1, &flag);    //调用接收一个字节的函数，清接收中断位
		if(flag)
		{
			uart_send1(UART_1, ch);     //向原串口发回一个字节
		}
	}
	//-------------------------------
	ENABLE_INTERRUPTS;       //开总中断
}

//==================================================================
//ISR：UART2_IRQHandler（UART2中断服务例程）
//进入条件：串口2接收到数据
//功能概要：（1）清中断标志
//       （2）串口2返回接收到的数据
//==================================================================
void UART2_IRQHandler(void)
{
	uint_8 i;
	uint_8 ch;
	uint_8 flag;
	uint_8* updateFlag;
	static uint_8 buf[7];

	//关总中断
	DISABLE_INTERRUPTS;      //关总中断
	//-------------------------------
	if(uart_get_re_int(UART_2))
	{
		ch = uart_re1 (UART_2, &flag);    //调用接收一个字节的函数，清接收中断位
		if(flag)
		{
			//保存最新的7个字节数据
			for(i = 6; i > 0;i--)
			{
				buf[i] = buf[i-1];
			}
			buf[0] = ch;
			//握手命令
			if(strncmp(buf,"ekahs",5) == 0)
			{
				uart_send_string(UART_2,"shake:GEC-6220");
			}
			//更新命令
			else if(strncmp(buf,"etadpu",6) == 0)
			{
//				flash_erase(SECTORNO-1);
//				flash_write(SECTORNO-1,0,6,"update");
				updateFlag = (uint_8 *)UPDATEADDRESS;
				memcpy(updateFlag,"update\0",7);

				NVIC_SystemReset();
			}
			//组装更新帧数据
			if(CreateFrame(ch,gRecvBuf,&gRecvLen) == 0)
			{
				gFunFlag = 2;  //接收到上位机更新数据
			}
		}
	}
	//-------------------------------
	ENABLE_INTERRUPTS;       //开总中断
}


//===========================================================================
//函数名称：CreateFrame
//功能概要：组建数据帧，将待组帧数据加入到数据帧中
//参数说明：Data：                   待组帧数据
//          buffer:        数据帧变量
//函数返回：组帧状态    0:组帧成功;1:开始组帧；2：帧头错误；3：组帧失败
//备注：十六进制数据帧格式
//             帧头 +帧长 + 命令 + 数据 + 校验 + 帧尾
//             2B  2B   1B   nB   2B   2B
//===========================================================================
#define FrameHeadH    (0x50)         //帧头     ASCII码对应P
#define FrameHeadL    (0x50)         //帧头     ASCII码对应P
#define FrameTaiH     (0x43)         //帧尾     ASCII码对应C
#define FrameTaiL     (0x43)         //帧尾     ASCII码对应C

uint_8 CreateFrame(uint_8 Data,uint_8 * buffer,uint_16 *len)
{
	static uint_16 dataLen = 0;  //帧字节计数器
	static uint_16 frameLen = 0; //帧长
	uint_8 frameFlag;            //组帧状态

	frameFlag = 1;
	switch(dataLen)
	{
	case 0:  //第一个数据
		if(Data == FrameHeadH)
		{
			buffer[dataLen++] = Data;
			frameFlag = 1;    //开始组帧
		}
		break;
	case 1:  //第二个数据
		if(Data == FrameHeadL)
		{
			buffer[dataLen++] = Data;
		}
		else
		{
			dataLen = 0;
			frameFlag = 2;    //帧头错误
		}
		break;
	case 2:  //第三个数据
		buffer[dataLen++] = Data;
		frameLen = Data<<8;
		break;
	case 3:  //第四个数据
		buffer[dataLen++] = Data;
		frameLen += Data;
		break;
	default:
		buffer[dataLen++] = Data;
		if(frameLen == dataLen)
		{
			if(buffer[frameLen-2] == FrameTaiH && buffer[frameLen-1] == FrameTaiL)
			{
				frameFlag = 0; //组帧成功
				*len = frameLen;
				dataLen = 0;
				frameLen = 0;
			}
			else
			{
				frameFlag = 3; //组帧失败
				dataLen = 0;
				frameLen = 0;
			}
		}
		break;
	}

	return frameFlag;
}
