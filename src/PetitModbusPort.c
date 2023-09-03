//==============================================================================
// PetitModbusPort.c
// This file contains the functions that are supposed to be defined to port
// PetitModbus to this specific microcontroller.
//==============================================================================

#include "PetitModbusPort.h"
#include <SI_EFM8BB1_Register_Enums.h>
#include "IoT_Supervisor.h"
#include "ModbusMiddleWare.h"

/*
 * Function Name: PetitPortTimerStart
 * @Description:
 *   starts the inter-byte timer.  if the timer fires, the rx buffer is invalid
 */
void PetitPortTimerStart()
{
	t0Count = 0;
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
	t0Count = 0;
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
void PetitPortTxBegin(pu8_t tx)
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
pu8_t PetitPortRegWrite(pu8_t Address, pu16_t Data)
{
	// check if you can access this
	if (Address > eMMW_HR_CFG &&
			(cfgSmS != eCFG_Cache || cfgSmS != eCFG_Commit))
	{
		return 0;
	}
	if (Address == eMMW_HR_STA)
	{
		if (Data == C_WDT_PET)
		{
			mbWDTpet = true;
			mbWDTen = true;
		}
		else if (Data == C_WDT_DIS)
		{
			mbWDTen = false;
		}
		else
		{
			return 0;
		}
	}
	if (Address == eMMW_HR_CFG)
	{
		pw = Data;
		pw_flag = true;
	}
	if (Address == eMMW_HR_MB)
	{
		if (Data & 0xFF > 0)
		{
			cfg.sid = Data & 0xFF;
		}
		if (Data >> 8 > 0)
		{
			cfg.baud = Data >> 8;
		}
	}
	if (Address == eMMW_HR_WDT)
	{
		if (Data > 0)
		{
			cfg.wdto = Data;
		}
	}
	if (Address == eMMW_HR_PW)
	{
		if (Data != 0 && Data != C_CMD_COMMIT && Data != C_CMD_CANCEL)
		{
			cfg.pw = Data;
		}
	}

	return 1;
}

/*
 * Function Name: PetitPortRegRead
 * @Description: reads the registers
 * @Return: the number of registers read
 *   0 if an error occurred during processing
 *   1 if everything went smoothly
 */
pu8_t PetitPortRegRead(pu8_t Address, pu16_t* Data)
{
	// check if you can access this
	if (Address > eMMW_HR_CFG &&
			(cfgSmS != eCFG_Cache || cfgSmS != eCFG_Commit))
	{
		return 0;
	}
	if (Address == eMMW_HR_STA)
	{
		*Data = sv_dev_sta.b;
	}
	if (Address == eMMW_HR_CFG)
	{
		*Data = cfgSmS;
	}
	if (Address == eMMW_HR_MB)
	{
		*Data = cfg.baud << 8 | cfg.sid;
	}
	if (Address == eMMW_HR_WDT)
	{
		*Data = MB_WD_TIMEOUT;
	}
	if (Address == eMMW_HR_PW)
	{
		*Data = cfg.pw;
	}
	return 1;
}

/*
 * Function Name: PetitPortInputRegRead
 * @Description: reads input registers
 * @Return: the number of registers read
 *   0 if an error occurred during processing
 *   1 if everything went smoothly
 */
// this function is basically here as an example, it is not used.
/*
pu8_t PetitPortInputRegRead(pu8_t Address, pu16_t* Data)
{
	return 1;
}
*/


