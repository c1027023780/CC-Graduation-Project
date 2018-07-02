/*
 ** ###################################################################
 **     Processors:          MKL36Z256VLH4
 **                          MKL36Z128VLH4
 **                          MKL36Z64VLH4
 **                          MKL36Z256VLL4
 **                          MKL36Z128VLL4
 **                          MKL36Z64VLL4
 **                          MKL36Z256VMC4
 **                          MKL36Z128VMC4
 **                          MKL36Z256VMP4
 **
 **     Compilers:           Keil ARM C/C++ Compiler
 **                          Freescale C/C++ for Embedded ARM
 **                          GNU C Compiler
 **                          GNU C Compiler - CodeSourcery Sourcery G++
 **                          IAR ANSI C/C++ Compiler for ARM
 **
 **     Reference manual:    KL36P121M48SF4RM, Rev.2, Dec 2012
 **     Version:             rev. 2.1, 2014-10-14
 **     Build:               b150225
 **
 **     Abstract:
 **         Provides a system configuration function and a global variable that
 **         contains the system frequency. It configures the device and initializes
 **         the oscillator (PLL) that is part of the microcontroller device.
 **
 **     Copyright (c) 2014 Freescale Semiconductor, Inc.
 **     All rights reserved.
 **
 **     Redistribution and use in source and binary forms, with or without modification,
 **     are permitted provided that the following conditions are met:
 **
 **     o Redistributions of source code must retain the above copyright notice, this list
 **       of conditions and the following disclaimer.
 **
 **     o Redistributions in binary form must reproduce the above copyright notice, this
 **       list of conditions and the following disclaimer in the documentation and/or
 **       other materials provided with the distribution.
 **
 **     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 **       contributors may be used to endorse or promote products derived from this
 **       software without specific prior written permission.
 **
 **     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 **     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 **     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 **     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 **     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 **     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 **     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 **     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 **     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 **     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **     http:                 www.freescale.com
 **     mail:                 support@freescale.com
 **
 **     Revisions:
 **     - rev. 1.0 (2012-12-12)
 **         Initial version.
 **     - rev. 1.1 (2013-04-05)
 **         Changed start of doxygen comment.
 **     - rev. 1.2 (2013-04-12)
 **         SystemInit function fixed for clock configuration 1.
 **         Name of the interrupt num. 31 updated to reflect proper function.
 **     - rev. 2.0 (2014-07-25)
 **         Updated to Kinetis SDK standard.
 **     - rev. 2.1 (2014-10-14)
 **         Renamed interrupt vector LPTimer to LPTMR0
 **
 ** ###################################################################
 */

/*!
 * @file MKL36Z4
 * @version 2.1
 * @date 2014-10-14
 * @brief Device specific configuration file for MKL36Z4 (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "MKL36Z4.h"



/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

//uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;
uint32_t SystemCoreClock = 48000000;

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

//

void SystemInit (void) {
	unsigned int i = 0;
	char temp_reg = 0;

	/* Watchdog disable */
#if (DISABLE_WDOG)
	/* SIM_COPC: COPT=3,COPCLKS=0,COPW=0 */
	SIM->COPC = (uint32_t)0x00u;
#endif /* (DISABLE_WDOG) */

	/* System clock initialization */
	/* SIM_CLKDIV1: OUTDIV1=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,OUTDIV4=3,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
	SIM_CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(0x00) | SIM_CLKDIV1_OUTDIV4(0x03)); /* Set the system prescalers to safe value */
	/* SIM_SCGC5: PORTA=1 */
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;   /* Enable clock gate for ports to enable pin routing */
	if ((PMC_REGSC & PMC_REGSC_ACKISO_MASK) != 0x0U) {
		/* PMC_REGSC: ACKISO=1 */
		PMC_REGSC |= PMC_REGSC_ACKISO_MASK; /* Release IO pads after wakeup from VLLS mode. */
	}
	/* SIM_CLKDIV1: OUTDIV1=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
	SIM_CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(0x00) | SIM_CLKDIV1_OUTDIV4(0x01)); /* Update system prescalers */
	/* SIM_SOPT2: ??=0,PLLFLLSEL=0 */
	SIM_SOPT2 &= (uint32_t)~(uint32_t)((SIM_SOPT2_PLLFLLSEL_MASK | 0x00020000U)); /* Select FLL as a clock source for various peripherals */
	/* SIM_SOPT1: OSC32KSEL=0 */
	SIM_SOPT1 &= (uint32_t)~(uint32_t)(SIM_SOPT1_OSC32KSEL(0x03)); /* System oscillator drives 32 kHz clock for various peripherals */
	/* SIM_SOPT2: TPMSRC=1 */
	SIM_SOPT2 = (uint32_t)((SIM_SOPT2 & (uint32_t)~(uint32_t)(
			SIM_SOPT2_TPMSRC(0x02)
	)) | (uint32_t)(
			SIM_SOPT2_TPMSRC(0x01)
	));                      /* Set the TPM clock */
	/* Switch to FEI Mode */
	/* MCG_C1: CLKS=0,FRDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=0 */
	MCG_C1 = MCG_C1_CLKS(0x00) |
			MCG_C1_FRDIV(0x00) |
			MCG_C1_IREFS_MASK |
			MCG_C1_IRCLKEN_MASK;
	/* MCG_C2: LOCRE0=0,RANGE0=0,HGO0=0,EREFS0=0,LP=0,IRCS=0 */
	MCG_C2 &= (uint8_t)~(uint8_t)(
			MCG_C2_LOCRE0_MASK |
			MCG_C2_RANGE0(0x03) |
			MCG_C2_HGO0_MASK |
			MCG_C2_EREFS0_MASK |
			MCG_C2_LP_MASK |
			MCG_C2_IRCS_MASK
	);
	/* MCG_C4: DMX32=1,DRST_DRS=1 */
	MCG_C4 = (uint8_t)((MCG_C4 & (uint8_t)~(uint8_t)(
			MCG_C4_DRST_DRS(0x02)
	)) | (uint8_t)(
			MCG_C4_DMX32_MASK |
			MCG_C4_DRST_DRS(0x01)
	));
	/* OSC0_CR: ERCLKEN=0,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
	OSC0_CR = 0x00U;
	/* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=0 */
	MCG_C5 = MCG_C5_PRDIV0(0x00);
	/* MCG_C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=0 */
	MCG_C6 = MCG_C6_VDIV0(0x00);
	while((MCG_S & MCG_S_IREFST_MASK) == 0x00U) { /* Check that the source of the FLL reference clock is the internal reference clock. */
	}
	while((MCG_S & 0x0CU) != 0x00U) {    /* Wait until output of the FLL is selected */
	}
	/*** End of PE initialization code after reset ***/

	//¿ªÆôPORTÄ£¿éÊ±ÖÓ
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK
			| SIM_SCGC5_PORTB_MASK
			| SIM_SCGC5_PORTC_MASK
			| SIM_SCGC5_PORTD_MASK
			| SIM_SCGC5_PORTE_MASK;
}

