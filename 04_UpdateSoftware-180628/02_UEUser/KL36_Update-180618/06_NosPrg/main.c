//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//==============================================================

//��������GLOBLE_VAR�궨�弰������ͷ�ļ�
#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ���������ȫ�ֱ���������ʹ������
#include "includes.h"   //������ͷ�ļ�

//������ʵ����Ҫ��ɾ��main.cʹ�õ��ڲ�����������
void funPointerInit();
void cpy_nvic(void);
void setVector(IRQn_Type IRQn, uint32_t vector);

typedef  uint_64 (*fun_getTime)(void);
volatile fun_getTime getTime;

//��������һ������¿�����Ϊ����Ӵ˿�ʼ���У�ʵ�������������̼���壩
int main(void)
{
    //��1��������ʵ����Ҫ��ɾ������������ʹ�õı���������ʱ��׼��ֵ��
	uint_32 mCount;
	uint_64 ct;

    //��2���������� �����ж�
    DISABLE_INTERRUPTS;

    cpy_nvic();
    funPointerInit();

    gpio_init(LIGHT_RED,GPIO_OUTPUT,LIGHT_OFF);
    flash_init();

    //��3�� ������ʵ����Ҫ��ɾ�����йر�������ֵ
	mCount = 0;
    //��3.1����������ʹ�õı�������ֵ

    //��3.2����ȫ�ֱ�������ֵ

    //��6��������ʵ����Ҫ��ɾ�� ʹ��ģ���ж� v
    //��7�������䡿�����ж�
    ENABLE_INTERRUPTS;
    //��8�����LCD��Ҫ��ʾ�����ݲ���ʾ

    //��ѭ����ʼ======================================================
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
        //��ѭ������======================================================
    }
}

void cpy_nvic(void)
{
    #define NVIC_NUM_VECTORS        (16 + 32)     // CORE + MCU Peripherals
	#define NVIC_RAM_VECTOR_ADDRESS 0x20000400

    uint32_t *old_vectors = (uint32_t *)SCB->VTOR;    //ȡ��ϵͳ���ƿ���VTOR�ټĴ�����ֵ
    uint32_t *vectors = (uint32_t*)NVIC_RAM_VECTOR_ADDRESS;   //���ڴ��ַ��ʼ��ַ��0x1FFFF000
    for (int i = 0; i < NVIC_NUM_VECTORS; i++) {    //��48���ж������������ڴ��ַ��
        vectors[i] = old_vectors[i];
    }
    SCB->VTOR = (uint32_t)NVIC_RAM_VECTOR_ADDRESS;    //����VTOR�Ĵ���ָ���µĵ�ַ
}

void setVector(IRQn_Type IRQn, uint32_t vector)
{
    #define NVIC_USER_IRQ_OFFSET          16
	uint32_t *vectors = (uint32_t *)SCB->VTOR;

	vectors[(int32_t)IRQn + NVIC_USER_IRQ_OFFSET] = vector;
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

    getTime = (fun_getTime)info.fun_getTime;
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
