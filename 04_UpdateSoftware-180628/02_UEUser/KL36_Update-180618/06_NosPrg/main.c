//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//==============================================================

//【不动】GLOBLE_VAR宏定义及包含总头文件
#define GLOBLE_VAR  //只需在main.c中定义一次，用来处理全局变量声明与使用问题
#include "includes.h"   //包含总头文件

//【根据实际需要增删】main.c使用的内部函数声明处
void funPointerInit();
void cpy_nvic(void);
void setVector(IRQn_Type IRQn, uint32_t vector);

typedef  uint_64 (*fun_getTime)(void);
volatile fun_getTime getTime;

//主函数，一般情况下可以认为程序从此开始运行（实际上有启动过程见书稿）
int main(void)
{
    //（1）【根据实际需要增删】声明主函数使用的变量（声明时不准赋值）
	uint_32 mCount;
	uint_64 ct;

    //（2）【不动】 关总中断
    DISABLE_INTERRUPTS;

    cpy_nvic();
    funPointerInit();

    gpio_init(LIGHT_RED,GPIO_OUTPUT,LIGHT_OFF);
    flash_init();

    //（3） 【根据实际需要增删】给有关变量赋初值
	mCount = 0;
    //（3.1）给主函数使用的变量赋初值

    //（3.2）给全局变量赋初值

    //（6）【根据实际需要增删】 使能模块中断 v
    //（7）【不变】开总中断
    ENABLE_INTERRUPTS;
    //（8）获得LCD需要显示的数据并显示

    //主循环开始======================================================
    for(;;)
    {
		mCount++;
		if(mCount > 0xfffff)
		{
			mCount = 0;
			gpio_reverse(LIGHT_RED);
			//light_change(LIGHT_RED);

			ct = getTime();
		}
        //主循环结束======================================================
    }
}

void cpy_nvic(void)
{
    #define NVIC_NUM_VECTORS        (16 + 32)     // CORE + MCU Peripherals
	#define NVIC_RAM_VECTOR_ADDRESS 0x20000400

    uint32_t *old_vectors = (uint32_t *)SCB->VTOR;    //取得系统控制块中VTOR①寄存器的值
    uint32_t *vectors = (uint32_t*)NVIC_RAM_VECTOR_ADDRESS;   //②内存地址起始地址：0x1FFFF000
    for (int i = 0; i < NVIC_NUM_VECTORS; i++) {    //将48个中断向量拷贝到内存地址中
        vectors[i] = old_vectors[i];
    }
    SCB->VTOR = (uint32_t)NVIC_RAM_VECTOR_ADDRESS;    //设置VTOR寄存器指向新的地址
}

void setVector(IRQn_Type IRQn, uint32_t vector)
{
    #define NVIC_USER_IRQ_OFFSET          16
	uint32_t *vectors = (uint32_t *)SCB->VTOR;

	vectors[(int32_t)IRQn + NVIC_USER_IRQ_OFFSET] = vector;
}

void funPointerInit()
{
	//（1）变量声明
	uint_8 *src;
	struct funInfo info;

	//（2）获取flash29扇区指针数据
    src=(uint_8 *)(29*1024);
    memcpy((uint_8*)&info,src,sizeof(struct funInfo));
    //（3）指针函数赋值
    //（3.1）gpio构件
    gpio_init = (fun_gpio_init)info.fun_gpio_init;
    gpio_set = (fun_gpio_set)info.fun_gpio_set;
    gpio_get = (fun_gpio_get)info.fun_gpio_get;
    gpio_reverse = (fun_gpio_reverse)info.fun_gpio_reverse;
    gpio_pull = (fun_gpio_pull)info.fun_gpio_pull;
    gpio_enable_int = (fun_gpio_enable_int)info.fun_gpio_enable_int;
    gpio_disable_int = (fun_gpio_disable_int)info.fun_gpio_disable_int;
    gpio_get_int = (fun_gpio_get_int)info.fun_gpio_get_int;
    gpio_clear_int = (fun_gpio_clear_int)info.fun_gpio_clear_int;
    gpio_clear_allint = (fun_gpio_clear_allint)info.fun_flash_erase;
    gpio_drive_strength = (fun_gpio_drive_strength)info.fun_gpio_drive_strength;
//    //（3.2）uart构件
//    uart_init = (fun_uart_init)info.fun_uart_init;
//    uart_send1 = (fun_uart_send1)info.fun_uart_send1;
//    uart_sendN = (fun_uart_sendN)info.fun_uart_sendN;
//    uart_send_string = (fun_uart_send_string)info.fun_uart_send_string;
//    uart_re1 = (fun_uart_re1)info.fun_uart_re1;
//    uart_reN = (fun_uart_reN)info.fun_uart_reN;
//    uart_enable_re_int = (fun_uart_enable_re_int)info.fun_uart_enable_re_int;
//    uart_disable_re_int = (fun_uart_disable_re_int)info.fun_uart_disable_re_int;
//    uart_get_re_int = (fun_uart_get_re_int)info.fun_uart_get_re_int;
//    uart_deint = (fun_uart_deint)info.fun_uart_deint;
//    //（3.3）flash构件
//    flash_init = (fun_flash_init)info.fun_flash_init;
//    flash_erase = (fun_flash_erase)info.fun_flash_erase;
//    flash_write = (fun_flash_write)info.fun_flash_write;
//    flash_read_logic = (fun_flash_read_logic)info.fun_flash_read_logic;
//    flash_read_physical = (fun_flash_read_physical)info.fun_flash_read_physical;
//    flash_protect = (fun_flash_protect)info.fun_flash_protect;
//    flash_isempty = (fun_flash_isempty)info.fun_flash_isempty;
    //（3.4）light构件
    light_init = (fun_light_init)info.fun_light_init;
    light_control = (fun_light_control)info.fun_light_control;
    light_change = (fun_light_change)info.fun_light_change;

    getTime = (fun_getTime)info.fun_getTime;
}

/*
 知识要素：
 1.main.c是一个具体的实例，执行相关程序流程不会涉及任何环境，芯片问题。
 该文件所有代码均不会涉及具体的硬件和环境，它通过调用相关构件来实现与硬件
 系统的交互。
 2.本文件共有两类代码，一类为【根据实际需要增删】，此类代码根据具体
 项目需求进行更改；另一类为【不动】，此类代码与具体项目无关，是通用的，
 无需根据具体项目进行更改。
 3.本文件对宏GLOBLE_VAR进行了定义， 所以在包含"includes.h"头文件时
 会定义全局变量，在其他文件中包含"includes.h"头文件时，仅声明该头文件中
 的全局变量。
 */
