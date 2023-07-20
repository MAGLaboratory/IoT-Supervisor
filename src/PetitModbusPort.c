#include "PetitModbusPort.h"
#include <SI_EFM8BB1_Register_Enums.h>

void PetitTimerStart()
{
	TL0 = (0x20 << TL0_TL0__SHIFT);
	TCON_TR0 = true;
	return;
}

void PetitTimerStop()
{
	TCON_TR0 = false;
	TL0 = (0x20 << TL0_TL0__SHIFT);
	return;
}

void PetitTxPinOn()
{
	P0_B3 = true;
	return;
}

void PetitTxPinOff()
{
	P0_B3 = false;
	return;
}

void PetitUartBegin()
{
	P0_B3 = true;
	// no need to check here, this function should only be called by 
	// petitmodbus on the first character anyway
	unsigned char tx;
	PetitUartTxBufferPop(&tx);
	SBUF = tx;
}

void PetitUartTxBufferReset();
{
}

void PetitUartRxBufferReset();
{
}
