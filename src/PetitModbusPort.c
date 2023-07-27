//==============================================================================
// PetitModbusPort.c
// This file contains the functions that are supposed to be defined to port
// PetitModbus to this specific microcontroller.
//==============================================================================

#include "PetitModbusPort.h"
#include <SI_EFM8BB1_Register_Enums.h>

// start the inter-byte timer.  if the timer fires, the rx buffer is invalid.
void PetitPortTimerStart()
{
	TL0 = (0x20 << TL0_TL0__SHIFT);
	TCON_TR0 = true;
	return;
}

// stop the inter-byte timer.  petitmodbus calls this before calculating the CRC
void PetitPortTimerStop()
{
	TCON_TR0 = false;
	TL0 = (0x20 << TL0_TL0__SHIFT);
	return;
}

// set the 485 transciever direction pin for transmit
void PetitPortDirTx()
{
	P0_B3 = true;
	return;
}

// set the 485 transciever direction pin for receive
void PetitPortDirRx()
{
	P0_B3 = false;
	return;
}

// send the first byte to begin sending the rest of the modbus response
void PetitPortTxBegin(unsigned char tx)
{
	PetitPortDirTx();
	// no need to check here, this function should only be called by 
	// petitmodbus on the first character anyway
	SBUF0 = tx;
	return;
}
