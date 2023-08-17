//==============================================================================
// PetitModbusPort.c
// This file contains the functions that are supposed to be defined to port
// PetitModbus to this specific microcontroller.
//==============================================================================

#include "PetitModbusPort.h"
#include <SI_EFM8BB1_Register_Enums.h>

/*
 * Function Name: PetitPortTimerStart
 * @Description:
 *   starts the inter-byte timer.  if the timer fires, the rx buffer is invalid
 */
void PetitPortTimerStart()
{
	TL0 = (0x20 << TL0_TL0__SHIFT);
	TCON_TR0 = true;
	return;
}

/*
 * Function Name: PetitPortTimerStop
 * @Description:
 *   stops the inter-byte timer.  petitmodbus calls this if the message has a
 *   valid servant address and size
 */
void PetitPortTimerStop()
{
	TCON_TR0 = false;
	TL0 = (0x20 << TL0_TL0__SHIFT);
	return;
}

/*
 * Function Name: PetitPortDirTx
 * @Description: sets the rs485 transciever direction pin for transmit
 */
void PetitPortDirTx()
{
	P0_B3 = true;
	return;
}

/*
 * Function Name: PetitPortDirRx
 * @Description: sets the rs485 transciever direction pin for receive
 */
void PetitPortDirRx()
{
	P0_B3 = false;
	return;
}

/*
 * Function Name: PetitPortTxBegin
 * @Description: sends the first byte to begin sending the rest of the response
 */
void PetitPortTxBegin(unsigned char tx)
{
	PetitPortDirTx();
	// no need to check here, this function should only be called by 
	// petitmodbus on the first character anyway
	SBUF0 = tx;
	return;
}

/*
 * Function Name: PetitPortRegWrite
 * @Description: writes to the registers
 * @Return: the number of registers written
 *   0 if an error occurred during processing
 *   1 if everything went smoothly
 */

/*
 * Function Name: PetitPortRegRead
 * @Description: reads the registers
 * @Return: the number of registers read
 *   0 if an error occurred during processing
 *   1 if everything went smoothly
 */
