//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//======================================================================

//【不动】GLOBLE_VAR宏定义及包含总头文件
#define GLOBLE_VAR  //只需在main.c中定义一次，用来处理全局变量声明与使用问题
#include "includes.h"   //包含总头文件

//【根据实际需要增删】main.c使用的内部函数声明处
void updateJudge();
void funToFlash();

uint_64 getTime(void);

int main(void)
{
	//（1）【根据实际需要增删】声明主函数使用的变量（声明时不准赋值）
	uint_16 i;
	uint_8 flag;
	uint_8 order;
	uint_16 mUpdateLen;
	uint_32 mCount;
	uint_8 mUpdateInfo[512];

	//（2）【不动】 关总中断
  	DISABLE_INTERRUPTS;
	//（3）【根据实际需要增删】 初始化外设模块
  	uart_init(UART_2,9600);
  	systick_init(CORE_CLOCK_DIV_16,1000);
	flash_init();
	light_init(LIGHT_GREEN,LIGHT_OFF);
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
			funToFlash(); //将各个函数指针写入flash

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
			flag = ueupdate_recv(gRecvBuf,gRecvLen);
			switch(flag)
			{
    		case 0:  order = 4;break;  //接收数据成功
    		case 1:  continue;  //不是更新数据
    		case 3:  order = 5;break;  //当前更新过程发生错误
    		case 4:  order = 6;break;  //缺失数据
    		default: order = 7;break;  //其他
			}
			if(flag == 6)  //代码更新完毕，转向用户代码
			{
				gFunFlag = 0;
				continue;
			}
			//向上位机返回数据
			ueupdate_send(order,mUpdateInfo,&mUpdateLen);
			for(i = 0;i < mUpdateLen;i++)
			{
				uart_send1(UART_2,mUpdateInfo[i]);
			}
			//uart_sendN(UART_2,mUpdateLen,mUpdateInfo); //会发不全
			gFunFlag = 10;  //非0、1、2的任意数
			break;
		default:
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

uint_64 getTime(void)
{
	return gTime;
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
	uint_8 str[7];
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
		//（2）读取flash更新状态数据
		flash_read_logic(str,SECTORNO,sizeof(struct funInfo),6);
		str[6] = 0;

		//（3）判断是否需要更新操作
		if(strcmp(str,"update") != 0)
			gFunFlag = 0;//无需更新
		else
			gFunFlag = 1;//需更新
	}
}

//=====================================================================
//函数名称：funToFlash
//函数返回：无
//参数说明：无
//功能概要：将各个驱动函数地址写入Flash，供用户程序函数指针调用
//=====================================================================
void funToFlash()
{
	//（1）变量声明
	struct funInfo info;

	//（2）函数指针赋值
	//（2.1）gpio构件
	info.fun_gpio_init           = (uint_32)&gpio_init;
	info.fun_gpio_set            = (uint_32)&gpio_set;
	info.fun_gpio_get            = (uint_32)&gpio_get;
	info.fun_gpio_pull           = (uint_32)&gpio_pull;
	info.fun_gpio_reverse        = (uint_32)&gpio_reverse;
	info.fun_gpio_enable_int     = (uint_32)&gpio_enable_int;
	info.fun_gpio_disable_int    = (uint_32)&gpio_disable_int;
	info.fun_gpio_get_int        = (uint_32)&gpio_get_int;
	info.fun_gpio_clear_int      = (uint_32)&gpio_clear_int;
	info.fun_gpio_clear_allint   = (uint_32)&gpio_clear_allint;
	info.fun_gpio_drive_strength = (uint_32)&gpio_drive_strength;
	//（2.2）uart构件
	info.fun_uart_init           = (uint_32)&uart_init;
	info.fun_uart_send1          = (uint_32)&uart_send1;
	info.fun_uart_sendN          = (uint_32)&uart_sendN;
	info.fun_uart_send_string    = (uint_32)&uart_send_string;
	info.fun_uart_re1            = (uint_32)&uart_re1;
	info.fun_uart_reN            = (uint_32)&uart_reN;
	info.fun_uart_enable_re_int  = (uint_32)&uart_enable_re_int;
	info.fun_uart_disable_re_int = (uint_32)&uart_disable_re_int;
	info.fun_uart_get_re_int     = (uint_32)&uart_get_re_int;
	info.fun_uart_deint          = (uint_32)&uart_deint;
	//（2.3）flash构件
	info.fun_flash_init          = (uint_32)&flash_init;
	info.fun_flash_erase         = (uint_32)&flash_erase;
	info.fun_flash_write         = (uint_32)&flash_write;
	info.fun_flash_read_logic    = (uint_32)&flash_read_logic;
	info.fun_flash_read_physical = (uint_32)&flash_read_physical;
	info.fun_flash_protect       = (uint_32)&flash_protect;
	info.fun_flash_isempty       = (uint_32)&flash_isempty;
	//（2.4）light构件
	info.fun_light_init    = (uint_32)&light_init;
	info.fun_light_control = (uint_32)&light_control;
	info.fun_light_change  = (uint_32)&light_change;

	info.fun_getTime = (uint_32)&getTime;

	//（3）函数指针写入flash
	flash_erase(SECTORNO);
	flash_write(SECTORNO,0,sizeof(struct funInfo),(uint_8*)&info);
}
