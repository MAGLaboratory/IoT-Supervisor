#include "PetitModbusPort.h"
#include <SI_EFM8BB1_Register_Enums.h>

void PetitPortTimerStart()
{
	TL0 = (0x20 << TL0_TL0__SHIFT);
	TCON_TR0 = true;
	return;
}

void PetitPortTimerStop()
{
	TCON_TR0 = false;
	TL0 = (0x20 << TL0_TL0__SHIFT);
	return;
}

void PetitPortTxPinOn()
{
	P0_B3 = true;
	return;
}

void PetitPortTxPinOff()
{
	P0_B3 = false;
	return;
}


// the linker will warn about this function because it is used in an ISR,
// but the ISR will only be called after this function executes.  So it's fine.
void PetitPortUartBegin(unsigned char tx)
{
	PetitPortTxPinOn();
	// no need to check here, this function should only be called by 
	// petitmodbus on the first character anyway
	SBUF0 = tx;
	return;
}
