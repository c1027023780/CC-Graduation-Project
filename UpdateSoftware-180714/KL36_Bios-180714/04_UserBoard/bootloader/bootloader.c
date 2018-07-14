//=====================================================================
//�ļ����ƣ�bootloader.c
//���ܸ�Ҫ��bootloader����Դ�ļ�
//=====================================================================

#include "bootloader.h"

typedef  void (*pFunction)(void);  //ָ���û�����λ�ж�

void bootloader_goToUserApp(volatile uint_32 app_start_addr)
{
    volatile pFunction jump_to_application;  //����ָ��
    uint_32 jump_addr;

    jump_addr = *(volatile uint_32*)(app_start_addr + 4);  //RESET�ж�

    //���ڲ�����bootloader, �ʳ����jump_addr��ַӦ���� [USERBASE, UPDATEBASE)��Χ��
    if(app_start_addr != 0xfffffffful)
    {
    	jump_to_application = (pFunction)jump_addr;

        __set_MSP(*(volatile uint32_t*)app_start_addr); //������ջ

        jump_to_application();  //��ת�û�����
    }
}
