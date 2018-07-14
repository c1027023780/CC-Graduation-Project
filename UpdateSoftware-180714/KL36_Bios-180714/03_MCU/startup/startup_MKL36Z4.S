/* ---------------------------------------------------------------------------------------*/
/*  @file:    startup_MKL36Z4.s                                                           */
/*  @purpose: CMSIS Cortex-M0P Core Device Startup File                                   */
/*            MKL36Z4                                                                     */
/*  @version: 2.1                                                                         */
/*  @date:    2014-10-14                                                                  */
/*  @build:   b150225                                                                     */
/* ---------------------------------------------------------------------------------------*/
/*                                                                                        */
/* Copyright (c) 1997 - 2015 , Freescale Semiconductor, Inc.                              */
/* All rights reserved.                                                                   */
/*                                                                                        */
/* Redistribution and use in source and binary forms, with or without modification,       */
/* are permitted provided that the following conditions are met:                          */
/*                                                                                        */
/* o Redistributions of source code must retain the above copyright notice, this list     */
/*   of conditions and the following disclaimer.                                          */
/*                                                                                        */
/* o Redistributions in binary form must reproduce the above copyright notice, this       */
/*   list of conditions and the following disclaimer in the documentation and/or          */
/*   other materials provided with the distribution.                                      */
/*                                                                                        */
/* o Neither the name of Freescale Semiconductor, Inc. nor the names of its               */
/*   contributors may be used to endorse or promote products derived from this            */
/*   software without specific prior written permission.                                  */
/*                                                                                        */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND        */
/* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED          */
/* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE                 */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR       */
/* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES         */
/* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;           */
/* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON         */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT                */
/* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS          */
/* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/*****************************************************************************/
/* Version: GCC for ARM Embedded Processors                                  */
/*****************************************************************************/
    .syntax unified
    .arch armv6-m

    .section .isr_vector, "a"
    .align 2
    .globl __isr_vector
__isr_vector:
    .long   __StackTop                                      /* Top of Stack */
    .long   Reset_Handler                                   /* Reset Handler */
    .long   NMI_Handler                                     /* NMI Handler*/
    .long   HardFault_Handler                               /* Hard Fault Handler*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   SVC_Handler                                     /* SVCall Handler*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   PendSV_Handler                                  /* PendSV Handler*/
    .long   SysTick_Handler                                 /* SysTick Handler*/

                                                            /* External Interrupts*/
    .long   DMA0_IRQHandler                                 /* DMA channel 0 transfer complete and error interrupt*/
    .long   DMA1_IRQHandler                                 /* DMA channel 1 transfer complete and error interrupt*/
    .long   DMA2_IRQHandler                                 /* DMA channel 2 transfer complete and error interrupt*/
    .long   DMA3_IRQHandler                                 /* DMA channel 3 transfer complete and error interrupt*/
    .long   Reserved20_IRQHandler                           /* Reserved interrupt*/
    .long   FTFA_IRQHandler                                 /* FTFA command complete and read collision*/
    .long   LVD_LVW_IRQHandler                              /* Low-voltage detect, low-voltage warning*/
    .long   LLWU_IRQHandler                                 /* Low Leakage Wakeup*/
    .long   I2C0_IRQHandler                                 /* I2C0 interrupt*/
    .long   I2C1_IRQHandler                                 /* I2C1 interrupt*/
    .long   SPI0_IRQHandler                                 /* SPI0 single interrupt vector for all sources*/
    .long   SPI1_IRQHandler                                 /* SPI1 single interrupt vector for all sources*/
    .long   UART0_IRQHandler                                /* UART0 status and error*/
    .long   UART1_IRQHandler                                /* UART1 status and error*/
    .long   UART2_IRQHandler                                /* UART2 status and error*/
    .long   ADC0_IRQHandler                                 /* ADC0 interrupt*/
    .long   CMP0_IRQHandler                                 /* CMP0 interrupt*/
    .long   TPM0_IRQHandler                                 /* TPM0 single interrupt vector for all sources*/
    .long   TPM1_IRQHandler                                 /* TPM1 single interrupt vector for all sources*/
    .long   TPM2_IRQHandler                                 /* TPM2 single interrupt vector for all sources*/
    .long   RTC_IRQHandler                                  /* RTC alarm interrupt*/
    .long   RTC_Seconds_IRQHandler                          /* RTC seconds interrupt*/
    .long   PIT_IRQHandler                                  /* PIT single interrupt vector for all channels*/
    .long   I2S0_IRQHandler                                 /* I2S0 Single interrupt vector for all sources*/
    .long   Reserved40_IRQHandler                           /* Reserved interrupt*/
    .long   DAC0_IRQHandler                                 /* DAC0 interrupt*/
    .long   TSI0_IRQHandler                                 /* TSI0 interrupt*/
    .long   MCG_IRQHandler                                  /* MCG interrupt*/
    .long   LPTMR0_IRQHandler                               /* LPTMR0 interrupt*/
    .long   LCD_IRQHandler                                  /* Segment LCD interrupt*/
    .long   PORTA_IRQHandler                                /* PORTA pin detect*/
    .long   PORTC_PORTD_IRQHandler                          /* Single interrupt vector for PORTC and PORTD pin detect*/

    .size    __isr_vector, . - __isr_vector

/* Flash Configuration */
    .section .FlashConfig, "a"
    .long 0xFFFFFFFF
    .long 0xFFFFFFFF
    .long 0xFFFFFFFF
    .long 0xFFFFFFFE


    .section .FunConfig, "a"
    .align 2
    .globl FunConfig
FunConfig:
    /*Bios基本信息区，总长24字节*/
    .byte 'B','i','o','s','-','K','L','3','6',0,0,0  /*Bios型号:Bios-KL36   ,长12字节*/
    .byte 'V','1','.','0'                            /*Bios版本号：V1.0      ,长4个字节*/
    .byte '2','0','1','8','0','7','1','4'            /*生产日期：20180714     ,长8个字节*/

    /*bootloader对外接口函数*/
    /*（1）系统时间配置/设置，2个*/
    .long   systemTimeGet
    .long   systemTimeSet
    .long   systemInfoGet
    /*（2）adc构件，2个*/
    .long   adc_init
    .long   adc_read
    /*（3）flash构件，7个*/
    .long   flash_init
    .long   flash_erase
    .long   flash_write
    .long   flash_read_logic
    .long   flash_read_physical
    .long   flash_protect
    .long   flash_isempty
    /*（4）gpio构件，11个*/
    .long   gpio_init
    .long   gpio_set
    .long   gpio_get
    .long   gpio_reverse
    .long   gpio_pull
    .long   gpio_enable_int
    .long   gpio_disable_int
    .long   gpio_get_int
    .long   gpio_clear_int
    .long   gpio_clear_allint
    .long   gpio_drive_strength
    /*（5）i2c构件，9个*/
    .long   I2C_init
    .long   I2C_read1
    .long   I2C_readN
    .long   I2C_rece1;
    .long   I2C_receN
    .long   I2C_write1
    .long   I2C_writeN
    .long   I2C_enable_re_int
    .long   I2C_disable_re_int
    /*（6）lptmr构件，4个*/
    .long   lptmr_init
    .long   lptmr_reset
    .long   enable_lptmr_int
    .long   disable_lptmr_int
    /*（7）spi构件，7个*/
    .long   SPI_init
    .long   SPI_send1
    .long   SPI_sendN
    .long   SPI_receive1
    .long   SPI_receiveN
    .long   SPI_enable_re_int
    .long   SPI_disable_re_int
    /*（8）tsi构件，6个*/
    .long   tsi_init
    .long   tsi_get_value16
    .long   tsi_set_threshold
    .long   tsi_enable_re_int
    .long   tsi_disable_re_int
    .long   tsi_softsearch
    /*（9）uart构件，10个*/
    .long   uart_init
    .long   uart_send1
    .long   uart_sendN
    .long   uart_send_string
    .long   uart_re1
    .long   uart_reN
    .long   uart_enable_re_int
    .long   uart_disable_re_int
    .long   uart_get_re_int
    .long   uart_deint
    //（2.10）uecom构件函数指针，12个
    .long   uecom_power
    .long   uecom_init
    .long   uecom_linkBase
    .long   uecom_linkCS
    .long   uecom_send
    .long   uecom_interrupt
    .long   uecom_baseInfo
    .long   uecom_modelInfo
    .long   uecom_gnssSwitch
    .long   uecom_gnssGetInfo
    .long   uecom_httpGet
    .long   uecom_default_delay_ms

    .size    FunConfig, . - FunConfig

    .text
    .thumb

/* Reset Handler */

    .thumb_func
    .align 2
    .globl   Reset_Handler
    .weak    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
    cpsid   i               /* Mask interrupts */
#ifndef __NO_SYSTEM_INIT
    bl SystemInit
#endif
    cpsie   i               /* Unmask interrupts */
/*     Loop to copy data from read only memory to RAM. The ranges
 *      of copy from/to are specified by following symbols evaluated in
 *      linker script.
 *      __etext: End of code section, i.e., begin of data sections to copy from.
 *      __data_start__/__data_end__: RAM address range that data should be
 *      copied to. Both must be aligned to 4 bytes boundary.  */

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

    subs    r3, r2
    ble     .LC0

.LC1:
    subs    r3, 4
    ldr    r0, [r1,r3]
    str    r0, [r2,r3]
    bgt    .LC1
.LC0:

#define __STARTUP_CLEAR_BSS
#ifdef __STARTUP_CLEAR_BSS
/*     This part of work usually is done in C library startup code. Otherwise,
 *     define this macro to enable it in this startup.
 *
 *     Loop to zero out BSS section, which uses following symbols
 *     in linker script:
 *      __bss_start__: start of BSS section. Must align to 4
 *      __bss_end__: end of BSS section. Must align to 4
 */
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__

    subs    r2, r1
    ble .LC3

    movs    r0, 0
.LC2:
    str r0, [r1, r2]
    subs    r2, 4
    bge .LC2
.LC3:
#endif
#ifndef __START
#define __START _start
#endif
    bl    __START
    .pool
    .size Reset_Handler, . - Reset_Handler

    .align	1
    .thumb_func
    .weak DefaultISR
    .type DefaultISR, %function
DefaultISR:
    ldr	r0, =DefaultISR
    bx r0
    .size DefaultISR, . - DefaultISR

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro def_irq_handler	handler_name
    .weak \handler_name
    .set  \handler_name, DefaultISR
    .endm

/* Exception Handlers */
    def_irq_handler    NMI_Handler
    def_irq_handler    HardFault_Handler
    def_irq_handler    SVC_Handler
    def_irq_handler    PendSV_Handler
    def_irq_handler    SysTick_Handler
    def_irq_handler    DMA0_IRQHandler
    def_irq_handler    DMA1_IRQHandler
    def_irq_handler    DMA2_IRQHandler
    def_irq_handler    DMA3_IRQHandler
    def_irq_handler    Reserved20_IRQHandler
    def_irq_handler    FTFA_IRQHandler
    def_irq_handler    LVD_LVW_IRQHandler
    def_irq_handler    LLWU_IRQHandler
    def_irq_handler    I2C0_IRQHandler
    def_irq_handler    I2C1_IRQHandler
    def_irq_handler    SPI0_IRQHandler
    def_irq_handler    SPI1_IRQHandler
    def_irq_handler    UART0_IRQHandler
    def_irq_handler    UART1_IRQHandler
    def_irq_handler    UART2_IRQHandler
    def_irq_handler    ADC0_IRQHandler
    def_irq_handler    CMP0_IRQHandler
    def_irq_handler    TPM0_IRQHandler
    def_irq_handler    TPM1_IRQHandler
    def_irq_handler    TPM2_IRQHandler
    def_irq_handler    RTC_IRQHandler
    def_irq_handler    RTC_Seconds_IRQHandler
    def_irq_handler    PIT_IRQHandler
    def_irq_handler    I2S0_IRQHandler
    def_irq_handler    Reserved40_IRQHandler
    def_irq_handler    DAC0_IRQHandler
    def_irq_handler    TSI0_IRQHandler
    def_irq_handler    MCG_IRQHandler
    def_irq_handler    LPTMR0_IRQHandler
    def_irq_handler    LCD_IRQHandler
    def_irq_handler    PORTA_IRQHandler
    def_irq_handler    PORTC_PORTD_IRQHandler

    .end
