//=========================================================
// src/Interrupts.c: generated by Hardware Configurator
//
// This file will be regenerated when saving a document.
// leave the sections inside the "$[...]" comment tags alone
// or they will be overwritten!
//=========================================================

// USER INCLUDES
#include <SI_EFM8BB1_Register_Enums.h>
#include "debugpins.h"
#include "PetitModbusPort.h"

// externs flags
extern volatile union
{
	struct
	{
		uint8_t t1Flag :1;
		uint8_t vinSmFlag :1;
		uint8_t WDTsmFlag :1;
	} v;
	uint8_t b;
} exec_flags;
extern bool cprif;

#define WDT_RESET() (WDTCN = 0xA5)

//-----------------------------------------------------------------------------
// CMP0_ISR
//-----------------------------------------------------------------------------
//
// CMP0 ISR Content goes here. Remember to clear flag bits:
// CMP0CN0::CPFIF (Comparator Falling-Edge Flag)
// CMP0CN0::CPRIF (Comparator Rising-Edge Flag)
//
//-----------------------------------------------------------------------------
SI_INTERRUPT (CMP0_ISR, CMP0_IRQn)
{
	static bool firstInterrupt = true;
	// state machine immediate off
	// clear falling edge flag (should not trigger to but whatever)
	if (CMP0CN0 & CMP0CN0_CPFIF__BMASK)
	{
		CMP0CN0 &= ~(CMP0CN0_CPFIF__BMASK);
	}
	// clear rising edge flag
	// run the Vin state machine to shut down the IoT device
	if (CMP0CN0 & CMP0CN0_CPRIF__BMASK)
	{
		CMP0CN0 &= ~(CMP0CN0_CPRIF__BMASK);
		// interrupt fires once on startup
		if (firstInterrupt)
			firstInterrupt = false;
		else
		{
			cprif = true;
			exec_flags.v.vinSmFlag = true;
		}
	}
}

//-----------------------------------------------------------------------------
// TIMER0_ISR (timer 0, low)
//-----------------------------------------------------------------------------
//
// TIMER0 ISR Content goes here. Remember to clear flag bits:
// TCON::TF0 (Timer 0 Overflow Flag)
//
// ISR for modbus message timeout
//
// Modbus implements a 1.75ms timeout between messages.  Clearing the data in
// the modbus buffer invalidates it.
//-----------------------------------------------------------------------------
SI_INTERRUPT (TIMER0_ISR, TIMER0_IRQn)
{
	TCON_TF0 = 0;
	// timer off
	// reset timer counter (should be a define)
	TCON_TR0 = false;
	TL0 = (0x20 << TL0_TL0__SHIFT);

	TIMER0_PIN_ON();

	// clear the modbus receiver
	PetitRxCounter = 0;
	PetitRxRemaining = PETITMODBUS_RXTX_BUFFER_SIZE;
	Petit_Rx_Ptr = &(PetitRxTxBuffer[0]);
	PetitExpectedReceiveCount = 0;

	// transceiver on receive mode
	P0_B3 = false;

	TIMER0_PIN_OFF();
}

//-----------------------------------------------------------------------------
// TIMER1_ISR (timer 0, high)
//-----------------------------------------------------------------------------
//
// TIMER1 ISR Content goes here. Remember to clear flag bits:
// TCON::TF1 (Timer 1 Overflow Flag)
//
// ISR for for timing (1 ms)
//-----------------------------------------------------------------------------
SI_INTERRUPT (TIMER1_ISR, TIMER1_IRQn)
{
	static unsigned char t1c = 0;
	TCON_TF1 = 0;

	TH0 = (0x80 << TH0_TH0__SHIFT);

	TIMER1_PIN_ON();

	exec_flags.v.t1Flag = true;

	// run every 8ms
	if ((t1c & 7) == 0)
	{
		exec_flags.v.WDTsmFlag = true;
		exec_flags.v.vinSmFlag = true;
		WDT_RESET();
	}
	t1c++;

	TIMER1_PIN_OFF();
}

//-----------------------------------------------------------------------------
// UART0_ISR
//-----------------------------------------------------------------------------
//
// UART0 ISR Content goes here. Remember to clear flag bits:
// RI::SCON0 (UART 0 Receive Flag)
// TI::SCON0 (UART 0 Transmit Flag)
//
// ISR for for UART
//-----------------------------------------------------------------------------
SI_INTERRUPT (UART0_ISR, UART0_IRQn)
{
	//Buffer and clear flags immediately so we don't miss an interrupt while processing
	uint8_t flags = SCON0 & (SCON0_RI__BMASK | SCON0_TI__BMASK);
	SCON0 &= ~flags;

	UART0_PIN_ON();

	if (PetitRxRemaining && Petit_RxTx_State == PETIT_RXTX_IDLE && 
			(flags & SCON0_RI__SET))
	{
		char read = SBUF0;
		*Petit_Rx_Ptr++ = read;
		PetitRxRemaining--;
		PetitRxCounter++;
		// reset silent period timer
		TL0 = (0x20 << TL0_TL0__SHIFT);
		TCON_TR0 = true;
	}

	if (Petit_RxTx_State == PETIT_RXTX_TX && (flags & SCON0_TI__SET))
	{
		if (Petit_Tx_Buf_Size != 0)
		{
			SBUF0 = *Petit_Tx_Ptr++;
			Petit_Tx_Buf_Size--;
		}
		else
		{
			// Petit Modbus Tx Complete
			Petit_RxTx_State = PETIT_RXTX_IDLE;
			P0_B3 = false;
		}
	}

	UART0_PIN_OFF();
}
//-----------------------------------------------------------------------------
// ADC0EOC_ISR
//-----------------------------------------------------------------------------
//
// ADC0EOC ISR Content goes here. Remember to clear flag bits:
// ADC0CN0::ADINT (Conversion Complete Interrupt Flag)
//
//-----------------------------------------------------------------------------
SI_INTERRUPT (ADC0EOC_ISR, ADC0EOC_IRQn)
{
	char conv_count;
	ADC0CN0_ADINT = 0;					// clear interrupt flag

	ADC0_PIN_ON();

	conv_count = PetitRegisters[2] >> 10 & 0x3F;
	conv_count++;
	PetitRegisters[2] = (uint16_t) conv_count << 10 | ADC0 >> 6;

	ADC0_PIN_OFF();
}

