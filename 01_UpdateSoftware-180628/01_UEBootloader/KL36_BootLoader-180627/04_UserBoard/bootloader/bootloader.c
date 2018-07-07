//=====================================================================
//文件名称：bootloader.c
//功能概要：bootloader构件源文件
//=====================================================================

#include "bootloader.h"

typedef  void (*pFunction)(void);  //指向用户程序复位中断

void bootloader_goToUserApp(volatile uint_32 app_start_addr)
{
    volatile pFunction jump_to_application;  //函数指针
    uint_32 jump_addr;

    jump_addr = *(volatile uint_32*)(app_start_addr + 4);  //RESET中断

    //由于采用了bootloader, 故程序的jump_addr地址应该在 [USERBASE, UPDATEBASE)范围内
    if(app_start_addr != 0xfffffffful)
    {
    	jump_to_application = (pFunction)jump_addr;

        __set_MSP(*(volatile uint32_t*)app_start_addr); //设置主栈

        jump_to_application();  //跳转用户程序
    }
}
