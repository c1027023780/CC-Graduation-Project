//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//======================================================================

//【不动】GLOBLE_VAR宏定义及包含总头文件
#define GLOBLE_VAR  //只需在main.c中定义一次，用来处理全局变量声明与使用问题
#include "includes.h"   //包含总头文件

//【根据实际需要增删】main.c使用的内部函数声明处
void updateJudge();
void systemTimeGet(uint_8 *date);
void systemTimeSet(uint_64 tick);
void systemInfoGet(uint_8 *data);

int main(void)
{
	//（1）【根据实际需要增删】声明主函数使用的变量（声明时不准赋值）
	uint_16 i;
	uint_8 flag;
	uint_8 order;
	uint_16 mUpdateLen;
	vuint_32 mCount;
	uint_8 mUpdateInfo[100];

	//（2）【不动】 关总中断
	DISABLE_INTERRUPTS;
	//（3）【根据实际需要增删】 初始化外设模块
	uart_init(UART_2,115200);
	systick_init(CORE_CLOCK_DIV_16,1000);
	flash_init();
	light_init(LIGHT_GREEN,LIGHT_ON);
	ueupdate_init(USERBASE/1024);
	//（4）【根据实际需要增删】 给有关变量赋初值
	gFunFlag = 0;
	gRecvLen = 0;
	gTime = 0;
	mCount = 0;

	//（5）【根据实际需要增删】 使能模块中断
	uart_enable_re_int(UART_2);
	//（6）【不变】开总中断
	ENABLE_INTERRUPTS;   //开总中断

	updateJudge();//判断是否需要进行更新操作

	//进入主循环
	//主循环开始======================================================
	for(;;)
	{
		switch(gFunFlag)
		{
		case 0:  //无需更新数据

			light_control(LIGHT_GREEN,LIGHT_OFF);

			//跳转用户程序
			DISABLE_INTERRUPTS;
			bootloader_goToUserApp(USERBASE);
			ENABLE_INTERRUPTS;   //开总中断
			break;
		case 1:  //提示上位机进入bootloader程序，可接收更新数据
			uart_send_string(UART_2,"Start Update");
			gFunFlag = 10; //非0、1、2的任意数
			break;
		case 2:  //接收到更新数据
			gFunFlag = 10;  //非0、1、2的任意数
			//接收数据，进行更新操作
			flag = ueupdate_recv(gRecvBuf,gRecvLen);
			//根据更新接收函数的返回值来进行数据回发、代码更新等操作的判断
			switch(flag)
			{
			case 0:  //成功接收上位机数据/命令，返回数据
				if(ueupdate_back(mUpdateInfo,&mUpdateLen) == 0)
				{
					//需返回数据
					uart_sendN(UART_2,mUpdateLen,mUpdateInfo);
				}
				break;
			case 3:  //代码更新完毕，转向用户代码
				gFunFlag = 0;
				break;
			default:  //其他错误，不做处理
				break;
			}
			break;
		default:  //等待接收更新代码
			mCount++;
			if(mCount > 0x5ffff)  //绿灯闪烁，提示处于Bootloader程序
			{
				mCount = 0;
				light_change(LIGHT_GREEN);
			}
			break;
		}
	} //主循环end_for
	//主循环结束======================================================
}

//uint_64 *tick
void systemTimeGet(uint_8 *date)
{
	timeChange(gTime,date);
}

void systemTimeSet(uint_64 tick)
{
	gTime = tick;
}

void systemInfoGet(uint_8 *data)
{
	flash_read_logic(data,SECTORNO,0,24);
}

//=====================================================================
//函数名称：updateJudge
//函数返回：无
//参数说明：无
//功能概要：读取29扇区数据，判断是否需要进行更新操作
//=====================================================================
void updateJudge()
{
	//（1）变量声明
	uint_8* str;
	uint_32 msp;

	//读取用户程序中断向量表首地址，若等于RAM最高地址+1则表明用户程序存在
	flash_read_logic((uint_8*)&msp,SECTORNO+1,0,4);
	if(msp != 0x20001800) //MKL36Z64的RAM最高地址+1等于0x20001800
	{
		//没有用户程序
		gFunFlag = 10;//非0、1、2的任意数
	}
	else  //有用户程序
	{
		str = (uint_8 *)UPDATEADDRESS;
		//（3）判断是否需要更新操作
		if(strcmp(str,"update") != 0)
			gFunFlag = 0;//无需更新
		else
		{
			str[0] = 0;
			gFunFlag = 1;//需更新
		}
	}
}
