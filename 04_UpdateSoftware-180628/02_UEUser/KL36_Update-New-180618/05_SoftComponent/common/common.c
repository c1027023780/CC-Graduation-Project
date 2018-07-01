//===========================================================================
//�ļ����ƣ�common.h
//���ܸ�Ҫ������Ҫ��Դ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2012-10-12 V1.0
//         2013-02-02 V2.0(WYH)
//===========================================================================

#include "common.h"    //��������Ҫ��ͷ�ļ�

static  uint_32  int_disable_level; // ��̬��������ǰ�ж�Ƕ�ײ���

//=========================================================================
//�������ƣ�init_critical
//����˵������
//�������أ���
//���ܸ�Ҫ����ʼ���ٽ������ʿ���
//=========================================================================
void  init_critical(void) 
{
    ENABLE_INTERRUPTS;
    int_disable_level = 0;
}

//=========================================================================
//�������ƣ�enter_critical
//����˵������
//�������أ���
//���ܸ�Ҫ�������ٽ���
//=========================================================================
void  enter_critical(void)
{
    if (0 == int_disable_level)
    {
        DISABLE_INTERRUPTS;
    }
    int_disable_level++;
}

//=========================================================================
//�������ƣ�exit_critical
//����˵������
//�������أ���
//���ܸ�Ҫ���뿪�ٽ���
//=========================================================================
void  exit_critical(void) 
{
    if (int_disable_level)
    {
        int_disable_level--;
        if (0 == int_disable_level)
        {
            ENABLE_INTERRUPTS;
        }
    }
}

//============================================================================
//�������ƣ�enable_irq
//�������أ���
//����˵����irq��irq��, ����ģ����жϺŶ�����MKL25Z.h��
//���ܸ�Ҫ��ʹ��irq�ж� 
//============================================================================
void enable_irq (IRQn_Type irq)
{
    //ȷ��irq��Ϊ��Ч��irq��
    if (irq > (IRQn_Type)32)    irq=(IRQn_Type)32;

    NVIC_EnableIRQ(irq);

}

//============================================================================
//�������ƣ�disable_irq
//�������أ���      
//����˵����irq��irq��, ����ģ����жϺŶ�����MKL25Z.h��
//���ܸ�Ҫ����ֹirq�ж� 
//============================================================================
void disable_irq (IRQn_Type irq)
{

    //ȷ��irq��Ϊ��Ч��irq��
    if (irq > (IRQn_Type)32)    irq=(IRQn_Type)32;

    NVIC_DisableIRQ(irq);
}

//============================================================================
//�������ƣ�set_irq_priority
//�������أ���      
//����˵����irq��irq��,����ģ����жϺŶ�����MKL25Z.h��
//         prio�����ȼ�
//���ܸ�Ҫ������irq�жϺ����ȼ� 
//============================================================================
void set_irq_priority (IRQn_Type irq, uint_16 prio)
{   
    uint_8 err = 0;

    if (irq > (IRQn_Type)32)
    {
        err = 1;
    }
    if (prio > 3)
    {
        err = 1;
    }
    if (err != 1)
    {
        NVIC_SetPriority(irq, prio);
    }
}

//======================================================================
//�������ƣ�Delay_us
//�������أ���
//����˵������
//���ܸ�Ҫ����ʱ - ΢�뼶
//======================================================================
void Delay_us(uint16_t u16us)
{
    uint32_t u32ctr;
    for(u32ctr = 0; u32ctr < ((48000 / 1000 / 11) * u16us); u32ctr++)
    {
        __asm ("NOP");
    }
}

//======================================================================
//�������ƣ�Delay_us
//�������أ���
//����˵������
//���ܸ�Ҫ����ʱ - ���뼶
//======================================================================
void Delay_ms(uint16_t u16ms)
{
	  vuint_16 i;
	  vuint_32 j;
	  for(i = 0; i<u16ms; i++)
	  {
	    for(j = 0; j<4200; j++)
	    	 __asm ("NOP");
	   #if ENABLE_WDOG == 1
	    if(i%30==0)
			wdog_feed();           //�����Ź�ι����
		#endif
	  }

    //@AC 1ms
    //gSystemClock_d is given in KHz --> gSystemClock_d * 1000 /1000 = counts to have 1ms
//    vuint_32 u32ctr,T;
//    T = (4800)*u16ms;
//    for(u32ctr = 0; u32ctr < T; u32ctr++)
//    {
////        __asm ("NOP");
//    }
}


//===========================================================================
//�������ƣ�SecAdd1
//�������أ���
//����˵����*p:Ϊָ��һ��ʱ��������p[3]
//���ܸ�Ҫ���뵥Ԫ+1��������ʱ�ֵ�Ԫ��00:00:00-23:59:59)
//===========================================================================
void SecAdd1(uint_8 *p)
{
    *(p+2)+=1;         //��+1
    if(*(p+2)>=60)     //�����
    {
        *(p+2)=0;       //����
        *(p+1)+=1;      //��+1
        if(*(p+1)>=60)  //�����
        {
            *(p+1)=0;    //���
            *p+=1;       //ʱ+1
            if(*p>=24)   //ʱ���
            {
                *p=0;      //��ʱ
            }
        }
    }
}