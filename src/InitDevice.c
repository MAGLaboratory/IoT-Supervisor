//=========================================================
// src/InitDevice.c: generated by Hardware Configurator
//
// This file will be regenerated when saving a document.
// leave the sections inside the "$[...]" comment tags alone
// or they will be overwritten!
//=========================================================

// USER INCLUDES
#include <SI_EFM8BB1_Register_Enums.h>
#include "InitDevice.h"

// USER PROTOTYPES
// USER FUNCTIONS

// $[Library Includes]
// [Library Includes]$

//==============================================================================
// enter_DefaultMode_from_RESET
//==============================================================================
extern void enter_DefaultMode_from_RESET(void)
{
	// $[Config Calls]
	WDT_0_enter_DefaultMode_from_RESET();
	PORTS_0_enter_DefaultMode_from_RESET();
	PORTS_1_enter_DefaultMode_from_RESET();
	PBCFG_0_enter_DefaultMode_from_RESET();
	ADC_0_enter_DefaultMode_from_RESET();
	CMP_0_enter_DefaultMode_from_RESET();
	CLOCK_0_enter_DefaultMode_from_RESET();
	TIMER01_0_enter_DefaultMode_from_RESET();
	TIMER16_2_enter_DefaultMode_from_RESET();
	TIMER_SETUP_0_enter_DefaultMode_from_RESET();
	UART_0_enter_DefaultMode_from_RESET();
	INTERRUPT_0_enter_DefaultMode_from_RESET();
	// [Config Calls]$

}

extern void WDT_0_enter_DefaultMode_from_RESET(void)
{
	// $[Watchdog Timer Init Variable Declarations]
	uint32_t i;
	bool ea;
	// [Watchdog Timer Init Variable Declarations]$

	// $[WDTCN - Watchdog Timer Control]
	// Deprecated
	// [WDTCN - Watchdog Timer Control]$

	// $[WDTCN_2 - Watchdog Timer Control]

	// Feed WDT timer before disabling (Erratum WDT_E102)
	WDTCN = 0xA5;

	// Add 2 LFO cycle delay before disabling WDT (Erratum WDT_E102)
	for (i = 0; i < (2 * 3062500UL) / (10000 * 3); i++)
	{
		NOP();
	}

	// Disable WDT before changing interval
	ea = IE_EA;
	IE_EA = 0;
	WDTCN = 0xDE;
	WDTCN = 0xAD;
	IE_EA = ea;

	// Delay 2 LFO cycles to ensure WDT is completely disabled
	for (i = 0; i < (2 * 3062500UL) / (10000 * 3); i++)
	{
		NOP();
	}

	// Change WDT interval
	WDTCN = 3;

	// Start WDT
	WDTCN = 0xA5;

	// [WDTCN_2 - Watchdog Timer Control]$

}

extern void PORTS_0_enter_DefaultMode_from_RESET(void)
{
	// $[P0 - Port 0 Pin Latch]
	/***********************************************************************
	 - P0.0 is high. Set P0.0 to drive or float high
	 - P0.1 is high. Set P0.1 to drive or float high
	 - P0.2 is high. Set P0.2 to drive or float high
	 - P0.3 is low. Set P0.3 to drive low
	 - P0.4 is high. Set P0.4 to drive or float high
	 - P0.5 is high. Set P0.5 to drive or float high
	 - P0.6 is high. Set P0.6 to drive or float high
	 - P0.7 is high. Set P0.7 to drive or float high
	 ***********************************************************************/
	P0 = P0_B0__HIGH | P0_B1__HIGH | P0_B2__HIGH | P0_B3__LOW | P0_B4__HIGH
			| P0_B5__HIGH | P0_B6__HIGH | P0_B7__HIGH;
	// [P0 - Port 0 Pin Latch]$

	// $[P0MDOUT - Port 0 Output Mode]
	/***********************************************************************
	 - P0.0 output is open-drain
	 - P0.1 output is open-drain
	 - P0.2 output is open-drain
	 - P0.3 output is push-pull
	 - P0.4 output is push-pull
	 - P0.5 output is open-drain
	 - P0.6 output is open-drain
	 - P0.7 output is push-pull
	 ***********************************************************************/
	P0MDOUT = P0MDOUT_B0__OPEN_DRAIN | P0MDOUT_B1__OPEN_DRAIN
			| P0MDOUT_B2__OPEN_DRAIN | P0MDOUT_B3__PUSH_PULL
			| P0MDOUT_B4__PUSH_PULL | P0MDOUT_B5__OPEN_DRAIN
			| P0MDOUT_B6__OPEN_DRAIN | P0MDOUT_B7__PUSH_PULL;
	// [P0MDOUT - Port 0 Output Mode]$

	// $[P0MDIN - Port 0 Input Mode]
	/***********************************************************************
	 - P0.0 pin is configured for digital mode
	 - P0.1 pin is configured for analog mode
	 - P0.2 pin is configured for digital mode
	 - P0.3 pin is configured for digital mode
	 - P0.4 pin is configured for digital mode
	 - P0.5 pin is configured for digital mode
	 - P0.6 pin is configured for digital mode
	 - P0.7 pin is configured for digital mode
	 ***********************************************************************/
	P0MDIN = P0MDIN_B0__DIGITAL | P0MDIN_B1__ANALOG | P0MDIN_B2__DIGITAL
			| P0MDIN_B3__DIGITAL | P0MDIN_B4__DIGITAL | P0MDIN_B5__DIGITAL
			| P0MDIN_B6__DIGITAL | P0MDIN_B7__DIGITAL;
	// [P0MDIN - Port 0 Input Mode]$

	// $[P0SKIP - Port 0 Skip]
	/***********************************************************************
	 - P0.0 pin is skipped by the crossbar
	 - P0.1 pin is skipped by the crossbar
	 - P0.2 pin is skipped by the crossbar
	 - P0.3 pin is skipped by the crossbar
	 - P0.4 pin is not skipped by the crossbar
	 - P0.5 pin is not skipped by the crossbar
	 - P0.6 pin is skipped by the crossbar
	 - P0.7 pin is not skipped by the crossbar
	 ***********************************************************************/
	P0SKIP = P0SKIP_B0__SKIPPED | P0SKIP_B1__SKIPPED | P0SKIP_B2__SKIPPED
			| P0SKIP_B3__SKIPPED | P0SKIP_B4__NOT_SKIPPED
			| P0SKIP_B5__NOT_SKIPPED | P0SKIP_B6__SKIPPED
			| P0SKIP_B7__NOT_SKIPPED;
	// [P0SKIP - Port 0 Skip]$

	// $[P0MASK - Port 0 Mask]
	// [P0MASK - Port 0 Mask]$

	// $[P0MAT - Port 0 Match]
	// [P0MAT - Port 0 Match]$

}

extern void PBCFG_0_enter_DefaultMode_from_RESET(void)
{
	// $[XBR2 - Port I/O Crossbar 2]
	/***********************************************************************
	 - Weak Pullups enabled 
	 - Crossbar enabled
	 ***********************************************************************/
	XBR2 = XBR2_WEAKPUD__PULL_UPS_ENABLED | XBR2_XBARE__ENABLED;
	// [XBR2 - Port I/O Crossbar 2]$

	// $[PRTDRV - Port Drive Strength]
	// [PRTDRV - Port Drive Strength]$

	// $[XBR0 - Port I/O Crossbar 0]
	/***********************************************************************
	 - UART TX, RX routed to Port pins P0.4 and P0.5
	 - SPI I/O unavailable at Port pins
	 - SMBus 0 I/O unavailable at Port pins
	 - CP0 unavailable at Port pin
	 - Asynchronous CP0 routed to Port pin
	 - CP1 unavailable at Port pin
	 - Asynchronous CP1 unavailable at Port pin
	 - SYSCLK unavailable at Port pin
	 ***********************************************************************/
	XBR0 = XBR0_URT0E__ENABLED | XBR0_SPI0E__DISABLED | XBR0_SMB0E__DISABLED
			| XBR0_CP0E__DISABLED | XBR0_CP0AE__ENABLED | XBR0_CP1E__DISABLED
			| XBR0_CP1AE__DISABLED | XBR0_SYSCKE__DISABLED;
	// [XBR0 - Port I/O Crossbar 0]$

	// $[XBR1 - Port I/O Crossbar 1]
	// [XBR1 - Port I/O Crossbar 1]$

}

extern void CLOCK_0_enter_DefaultMode_from_RESET(void)
{
	// $[CLKSEL - Clock Select]
	/***********************************************************************
	 - Clock derived from the Internal High-Frequency Oscillator
	 - SYSCLK is equal to selected clock source divided by 16
	 ***********************************************************************/
	CLKSEL = CLKSEL_CLKSL__HFOSC | CLKSEL_CLKDIV__SYSCLK_DIV_16;
	// [CLKSEL - Clock Select]$

}

extern void CMP_0_enter_DefaultMode_from_RESET(void)
{
	// $[CMP0MX - Comparator 0 Multiplexer Selection]
	/***********************************************************************
	 - External pin CMP0P.8
	 - External pin CMP0N.1
	 ***********************************************************************/
	uint8_t delay;

	CMP0MX = CMP0MX_CMXP__LDO_OUT | CMP0MX_CMXN__CMP0N1;
	// Delay to allow the comparator inputs to settle
	for (delay = 0; delay < 0x20; delay++)
		;
	// [CMP0MX - Comparator 0 Multiplexer Selection]$

	// $[CMP0MD - Comparator 0 Mode]
	/***********************************************************************
	 - Comparator rising-edge interrupt enabled
	 ***********************************************************************/
	CMP0MD |= CMP0MD_CPRIE__RISE_INT_ENABLED;
	// [CMP0MD - Comparator 0 Mode]$

	// $[CMP0CN0 - Comparator 0 Control 0]
	/***********************************************************************
	 - Comparator enabled
	 ***********************************************************************/
	CMP0CN0 |= CMP0CN0_CPEN__ENABLED;
	// [CMP0CN0 - Comparator 0 Control 0]$

}

extern void TIMER01_0_enter_DefaultMode_from_RESET(void)
{
	// $[Timer Initialization]
	//Save Timer Configuration
	uint8_t TCON_save;
	TCON_save = TCON;
	//Stop Timers
	TCON &= ~TCON_TR0__BMASK & ~TCON_TR1__BMASK;

	// [Timer Initialization]$

	// $[TH0 - Timer 0 High Byte]
	/***********************************************************************
	 - Timer 0 High Byte = 0x80
	 ***********************************************************************/
	TH0 = (0x80 << TH0_TH0__SHIFT);
	// [TH0 - Timer 0 High Byte]$

	// $[TL0 - Timer 0 Low Byte]
	/***********************************************************************
	 - Timer 0 Low Byte = 0x20
	 ***********************************************************************/
	TL0 = (0x20 << TL0_TL0__SHIFT);
	// [TL0 - Timer 0 Low Byte]$

	// $[TH1 - Timer 1 High Byte]
	/***********************************************************************
	 - Timer 1 High Byte = 0xEC
	 ***********************************************************************/
	TH1 = (0xEC << TH1_TH1__SHIFT);
	// [TH1 - Timer 1 High Byte]$

	// $[TL1 - Timer 1 Low Byte]
	/***********************************************************************
	 - Timer 1 Low Byte = 0xEC
	 ***********************************************************************/
	TL1 = (0xEC << TL1_TL1__SHIFT);
	// [TL1 - Timer 1 Low Byte]$

	// $[Timer Restoration]
	//Restore Timer Configuration
	TCON |= (TCON_save & TCON_TR0__BMASK) | (TCON_save & TCON_TR1__BMASK);

	// [Timer Restoration]$

}

extern void TIMER_SETUP_0_enter_DefaultMode_from_RESET(void)
{
	// $[CKCON0 - Clock Control 0]
	/***********************************************************************
	 - System clock divided by 12
	 - Counter/Timer 0 uses the clock defined by the prescale field, SCA
	 - Timer 2 high byte uses the clock defined by T2XCLK in TMR2CN0
	 - Timer 2 low byte uses the clock defined by T2XCLK in TMR2CN0
	 - Timer 3 high byte uses the clock defined by T3XCLK in TMR3CN0
	 - Timer 3 low byte uses the clock defined by T3XCLK in TMR3CN0
	 - Timer 1 uses the system clock
	 ***********************************************************************/
	CKCON0 = CKCON0_SCA__SYSCLK_DIV_12 | CKCON0_T0M__PRESCALE
			| CKCON0_T2MH__EXTERNAL_CLOCK | CKCON0_T2ML__EXTERNAL_CLOCK
			| CKCON0_T3MH__EXTERNAL_CLOCK | CKCON0_T3ML__EXTERNAL_CLOCK
			| CKCON0_T1M__SYSCLK;
	// [CKCON0 - Clock Control 0]$

	// $[TMOD - Timer 0/1 Mode]
	/***********************************************************************
	 - Mode 3, Two 8-bit Counter/Timers
	 - Mode 2, 8-bit Counter/Timer with Auto-Reload
	 - Timer Mode
	 - Timer 0 enabled when TR0 = 1 irrespective of INT0 logic level
	 - Timer Mode
	 - Timer 1 enabled when TR1 = 1 irrespective of INT1 logic level
	 ***********************************************************************/
	TMOD = TMOD_T0M__MODE3 | TMOD_T1M__MODE2 | TMOD_CT0__TIMER
			| TMOD_GATE0__DISABLED | TMOD_CT1__TIMER | TMOD_GATE1__DISABLED;
	// [TMOD - Timer 0/1 Mode]$

	// $[TCON - Timer 0/1 Control]
	/***********************************************************************
	 - Start Timer 1 running
	 ***********************************************************************/
	TCON |= TCON_TR1__RUN;
	// [TCON - Timer 0/1 Control]$

}

extern void INTERRUPT_0_enter_DefaultMode_from_RESET(void)
{
	// $[EIE1 - Extended Interrupt Enable 1]
	/***********************************************************************
	 - Enable interrupt requests generated by the ADINT flag
	 - Disable ADC0 Window Comparison interrupt
	 - Enable interrupt requests generated by the comparator 0 CPRIF or CPFIF
	 flags
	 - Disable CP1 interrupts
	 - Disable all Port Match interrupts
	 - Disable all PCA0 interrupts
	 - Disable all SMB0 interrupts
	 - Disable Timer 3 interrupts
	 ***********************************************************************/
	EIE1 = EIE1_EADC0__ENABLED | EIE1_EWADC0__DISABLED | EIE1_ECP0__ENABLED
			| EIE1_ECP1__DISABLED | EIE1_EMAT__DISABLED | EIE1_EPCA0__DISABLED
			| EIE1_ESMB0__DISABLED | EIE1_ET3__DISABLED;
	// [EIE1 - Extended Interrupt Enable 1]$

	// $[EIP1 - Extended Interrupt Priority 1]
	// [EIP1 - Extended Interrupt Priority 1]$

	// $[IE - Interrupt Enable]
	/***********************************************************************
	 - Enable each interrupt according to its individual mask setting
	 - Disable external interrupt 0
	 - Disable external interrupt 1
	 - Disable all SPI0 interrupts
	 - Enable interrupt requests generated by the TF0 flag
	 - Enable interrupt requests generated by the TF1 flag
	 - Disable Timer 2 interrupt
	 - Enable UART0 interrupt
	 ***********************************************************************/
	IE = IE_EA__ENABLED | IE_EX0__DISABLED | IE_EX1__DISABLED
			| IE_ESPI0__DISABLED | IE_ET0__ENABLED | IE_ET1__ENABLED
			| IE_ET2__DISABLED | IE_ES0__ENABLED;
	// [IE - Interrupt Enable]$

	// $[IP - Interrupt Priority]
	// [IP - Interrupt Priority]$

}

extern void enter_DefaultMode_2_from_DefaultMode(void)
{

}

extern void PMU_0_enter_DefaultMode_2_from_DefaultMode(void)
{

}

extern void UART_0_enter_DefaultMode_from_RESET(void)
{
	// $[SCON0 - UART0 Serial Port Control]
	/***********************************************************************
	 - UART0 reception enabled
	 - RI is set and an interrupt is generated only when the stop bit is
	 logic 1
	 ***********************************************************************/
	SCON0 |= SCON0_REN__RECEIVE_ENABLED | SCON0_MCE__MULTI_ENABLED;
	// [SCON0 - UART0 Serial Port Control]$

}

extern void ADC_0_enter_DefaultMode_from_RESET(void)
{
	// $[ADC0CN1 - ADC0 Control 1]
	/***********************************************************************
	 - Enable the common mode buffer
	 ***********************************************************************/
	ADC0CN1 = ADC0CN1_ADCMBE__CM_BUFFER_ENABLED;
	// [ADC0CN1 - ADC0 Control 1]$

	// $[ADC0MX - ADC0 Multiplexer Selection]
	/***********************************************************************
	 - Select ADC0.1
	 ***********************************************************************/
	ADC0MX = ADC0MX_ADC0MX__ADC0P1;
	// [ADC0MX - ADC0 Multiplexer Selection]$

	// $[ADC0CF - ADC0 Configuration]
	/***********************************************************************
	 - SAR Clock Divider = 0x0F
	 - ADC0 operates in 10-bit or 12-bit mode 
	 - The on-chip PGA gain is 0.5
	 - Normal Track Mode
	 ***********************************************************************/
	ADC0CF = (0x0F << ADC0CF_ADSC__SHIFT) | ADC0CF_AD8BE__NORMAL
			| ADC0CF_ADGN__GAIN_0P5 | ADC0CF_ADTM__TRACK_NORMAL;
	// [ADC0CF - ADC0 Configuration]$

	// $[ADC0AC - ADC0 Accumulator Configuration]
	/***********************************************************************
	 - Left justified. No shifting applied
	 - Disable 12-bit mode
	 - ADC0H:ADC0L contain the result of the latest conversion when Burst
	 Mode is disabled
	 - Perform and Accumulate 1 conversion 
	 ***********************************************************************/
	ADC0AC = ADC0AC_ADSJST__LEFT_NO_SHIFT | ADC0AC_AD12BE__12_BIT_DISABLED
			| ADC0AC_ADAE__ACC_DISABLED | ADC0AC_ADRPT__ACC_1;
	// [ADC0AC - ADC0 Accumulator Configuration]$

	// $[ADC0TK - ADC0 Burst Mode Track Time]
	// [ADC0TK - ADC0 Burst Mode Track Time]$

	// $[ADC0PWR - ADC0 Power Control]
	/***********************************************************************
	 - Burst Mode Power Up Time = 0x0F
	 - Disable low power mode
	 - Low power mode enabled 
	 - Select bias current mode 3 
	 ***********************************************************************/
	ADC0PWR = (0x0F << ADC0PWR_ADPWR__SHIFT) | ADC0PWR_ADLPM__LP_BUFFER_DISABLED
			| ADC0PWR_ADMXLP__LP_MUX_VREF_ENABLED | ADC0PWR_ADBIAS__MODE3;
	// [ADC0PWR - ADC0 Power Control]$

	// $[ADC0GTH - ADC0 Greater-Than High Byte]
	// [ADC0GTH - ADC0 Greater-Than High Byte]$

	// $[ADC0GTL - ADC0 Greater-Than Low Byte]
	// [ADC0GTL - ADC0 Greater-Than Low Byte]$

	// $[ADC0LTH - ADC0 Less-Than High Byte]
	// [ADC0LTH - ADC0 Less-Than High Byte]$

	// $[ADC0LTL - ADC0 Less-Than Low Byte]
	// [ADC0LTL - ADC0 Less-Than Low Byte]$

	// $[ADC0CN0 - ADC0 Control 0]
	/***********************************************************************
	 - Enable ADC0 
	 - ADC0 conversion initiated on overflow of Timer 2
	 ***********************************************************************/
	ADC0CN0 &= ~ADC0CN0_ADCM__FMASK;
	ADC0CN0 |= ADC0CN0_ADEN__ENABLED | ADC0CN0_ADCM__TIMER2;
	// [ADC0CN0 - ADC0 Control 0]$

}

extern void TIMER16_2_enter_DefaultMode_from_RESET(void)
{
	// $[Timer Initialization]
	// Save Timer Configuration
	uint8_t TMR2CN0_TR2_save;
	TMR2CN0_TR2_save = TMR2CN0 & TMR2CN0_TR2__BMASK;
	// Stop Timer
	TMR2CN0 &= ~(TMR2CN0_TR2__BMASK);
	// [Timer Initialization]$

	// $[TMR2CN0 - Timer 2 Control]
	// [TMR2CN0 - Timer 2 Control]$

	// $[TMR2H - Timer 2 High Byte]
	/***********************************************************************
	 - Timer 2 High Byte = 0xE7
	 ***********************************************************************/
	TMR2H = (0xE7 << TMR2H_TMR2H__SHIFT);
	// [TMR2H - Timer 2 High Byte]$

	// $[TMR2L - Timer 2 Low Byte]
	/***********************************************************************
	 - Timer 2 Low Byte = 0x14
	 ***********************************************************************/
	TMR2L = (0x14 << TMR2L_TMR2L__SHIFT);
	// [TMR2L - Timer 2 Low Byte]$

	// $[TMR2RLH - Timer 2 Reload High Byte]
	/***********************************************************************
	 - Timer 2 Reload High Byte = 0xE7
	 ***********************************************************************/
	TMR2RLH = (0xE7 << TMR2RLH_TMR2RLH__SHIFT);
	// [TMR2RLH - Timer 2 Reload High Byte]$

	// $[TMR2RLL - Timer 2 Reload Low Byte]
	/***********************************************************************
	 - Timer 2 Reload Low Byte = 0x14
	 ***********************************************************************/
	TMR2RLL = (0x14 << TMR2RLL_TMR2RLL__SHIFT);
	// [TMR2RLL - Timer 2 Reload Low Byte]$

	// $[TMR2CN0]
	/***********************************************************************
	 - Start Timer 2 running
	 ***********************************************************************/
	TMR2CN0 |= TMR2CN0_TR2__RUN;
	// [TMR2CN0]$

	// $[Timer Restoration]
	// Restore Timer Configuration
	TMR2CN0 |= TMR2CN0_TR2_save;
	// [Timer Restoration]$

}

extern void VREF_0_enter_DefaultMode_from_RESET(void)
{

}

extern void PORTS_1_enter_DefaultMode_from_RESET(void)
{
	// $[P1 - Port 1 Pin Latch]
	// [P1 - Port 1 Pin Latch]$

	// $[P1MDOUT - Port 1 Output Mode]
	// [P1MDOUT - Port 1 Output Mode]$

	// $[P1MDIN - Port 1 Input Mode]
	// [P1MDIN - Port 1 Input Mode]$

	// $[P1SKIP - Port 1 Skip]
	// [P1SKIP - Port 1 Skip]$

	// $[P1MASK - Port 1 Mask]
	// [P1MASK - Port 1 Mask]$

	// $[P1MAT - Port 1 Match]
	// [P1MAT - Port 1 Match]$

}

