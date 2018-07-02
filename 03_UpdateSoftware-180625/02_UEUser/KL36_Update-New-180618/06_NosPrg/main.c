//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//==============================================================

//��������GLOBLE_VAR�궨�弰������ͷ�ļ�
#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ���������ȫ�ֱ���������ʹ������
#include "includes.h"   //������ͷ�ļ�

//������ʵ����Ҫ��ɾ��main.cʹ�õ��ڲ�����������
void funPointerInit();

//__attribute__((section (".FunConfig")))
//��������һ������¿�����Ϊ����Ӵ˿�ʼ���У�ʵ�������������̼���壩
int main(void)
{
    //��1��������ʵ����Ҫ��ɾ������������ʹ�õı���������ʱ��׼��ֵ��
	uint_32 mCount;

    //��2���������� �����ж�
    DISABLE_INTERRUPTS;
    funPointerInit();

    systick_init(CORE_CLOCK_DIV_16,1000);
    uart_init(UART_2,9600);
    gpio_init(LIGHT_BLUE,GPIO_OUTPUT,LIGHT_OFF);
    //light_init(LIGHT_BLUE,LIGHT_ON);
    flash_init();

	SCB->VTOR=0x00007800UL;  //�ж�������ָ���û����������ж�������
    //��3�� ������ʵ����Ҫ��ɾ�����йر�������ֵ
	mCount = 0;
    //��3.1����������ʹ�õı�������ֵ

    //��3.2����ȫ�ֱ�������ֵ

    //��6��������ʵ����Ҫ��ɾ�� ʹ��ģ���ж�
	uart_enable_re_int(UART_2);
    //��7�������䡿�����ж�
    ENABLE_INTERRUPTS;
    //��8�����LCD��Ҫ��ʾ�����ݲ���ʾ


    //��ѭ����ʼ======================================================
    for(;;)
    {
		mCount++;
		if(mCount > 0x1ffff)
		{
			mCount = 0;
			//gpio_reverse(LIGHT_BLUE);
			//light_change(LIGHT_BLUE);
		}
        //��ѭ������======================================================
    }
}

void funPointerInit()
{
	//��1����������
	uint_8 *src;
	struct funInfo info;

	//��2����ȡflash29����ָ������
    src=(uint_8 *)(29*1024);
    memcpy((uint_8*)&info,src,sizeof(struct funInfo));
    //��3��ָ�뺯����ֵ
    //��3.1��gpio����
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
//    //��3.2��uart����
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
//    //��3.3��flash����
//    flash_init = (fun_flash_init)info.fun_flash_init;
//    flash_erase = (fun_flash_erase)info.fun_flash_erase;
//    flash_write = (fun_flash_write)info.fun_flash_write;
//    flash_read_logic = (fun_flash_read_logic)info.fun_flash_read_logic;
//    flash_read_physical = (fun_flash_read_physical)info.fun_flash_read_physical;
//    flash_protect = (fun_flash_protect)info.fun_flash_protect;
//    flash_isempty = (fun_flash_isempty)info.fun_flash_isempty;
    //��3.4��light����
    light_init = (fun_light_init)info.fun_light_init;
    light_control = (fun_light_control)info.fun_light_control;
    light_change = (fun_light_change)info.fun_light_change;
}

/*
 ֪ʶҪ�أ�
 1.main.c��һ�������ʵ����ִ����س������̲����漰�κλ�����оƬ���⡣
 ���ļ����д���������漰�����Ӳ���ͻ�������ͨ��������ع�����ʵ����Ӳ��
 ϵͳ�Ľ�����
 2.���ļ�����������룬һ��Ϊ������ʵ����Ҫ��ɾ�������������ݾ���
 ��Ŀ������и��ģ���һ��Ϊ������������������������Ŀ�޹أ���ͨ�õģ�
 ������ݾ�����Ŀ���и��ġ�
 3.���ļ��Ժ�GLOBLE_VAR�����˶��壬 �����ڰ���"includes.h"ͷ�ļ�ʱ
 �ᶨ��ȫ�ֱ������������ļ��а���"includes.h"ͷ�ļ�ʱ����������ͷ�ļ���
 ��ȫ�ֱ�����
 */
