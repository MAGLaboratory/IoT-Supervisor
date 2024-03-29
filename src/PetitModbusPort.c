/*******************************************************************************
 * @file PetitModbusPort.c
 * This file contains the functions that are supposed to be defined to port
 * PetitModbus to this specific microcontroller.
 ******************************************************************************/

/**
 * @defgroup IoT_SV_EFM8BB1LCK_Petit_Modbus_Port IoT Supervisor EFM8BB1LCK Petit Modbus Port
 * This section contains the functions necessary to make Petit Modbus work with
 * this board and MCU.
 * @{
 */

// Necessary Petit Modbus Includes
#include "PetitModbusPort.h"
// User Includes
#include <SI_EFM8BB1_Register_Enums.h>
#include "hardware.h"
#include "IoT_Supervisor.h"
#include "ModbusMiddleWare.h"


/**
 *  starts the inter-byte timer.  if the timer fires, the rx buffer is invalid
 */
void PetitPortTimerStart()
{
	t0Count = 0;
	TL0 = (0x20 << TL0_TL0__SHIFT);
	TCON_TR0 = true;
	return;
}

/**
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

/**
 * sets the rs485 transciever direction pin for transmit
 */
void PetitPortDirTx()
{
	XCVR_TX = true;
	dir_tx = true;
	return;
}

/**
 * sets the rs485 transciever direction pin for receive
 */
void PetitPortDirRx()
{
	XCVR_TX = false;
	dir_tx = false;
	return;
}

/**
 * sends the first byte to begin sending the rest of the response
 */
void PetitPortTxBegin(pu8_t tx)
{
	PetitPortDirTx();
	// no need to check here, this function should only be called by 
	// petitmodbus on the first character anyway
	SBUF0 = tx;
	return;
}

/**
 * writes to the registers
 *
 * this function will return if it is determined that the value written is
 * invalid.
 * @return the number of registers written
 *   0 if an error occurred during processing
 *   1 if everything went smoothly
 */
pb_t PetitPortRegWrite(pu8_t Address, pu16_t Data)
{
	// check if you can write to this
	if (Address > eMMW_HR_CFG && cfgSmS != eCFG_Cache)
	{
		return 0;
	}
	// the status register only accepts a few values, and petting the watchdog
	// automatically enables the watchdog
	// the reset source can be reset to 0 by writing 0 to this register
	if (Address == eMMW_HR_STA)
	{
		switch (Data)
		{
		case 0:
			sv_dev_sta.v.lastRstS = eLR_Init;
			break;
		case C_WDT_PET:
			mbWDTpet = true;
			mbWDTen = true;
			break;
		case C_WDT_DIS:
			mbWDTen = false;
			break;
		default:
			return 0;
		}
	}
	// you can enter whatever password you want
	if (Address == eMMW_HR_CFG)
	{
		pw = Data;
		pw_flag = true;
	}
	// it's perfectly valid to enter 0 for this since none of the bytewise
	// changes will be applied
	if (Address == eMMW_HR_MB)
	{
		if ((Data & 0xFF) > 0)
		{
			// but if you enter an SID that is too large, you will get an error
			if ((Data & 0xFF) < 248)
			{
				cfg.sid = Data & 0xFF;
			}
			else
			{
				return 0;
			}
		}
		if (Data >> 8 > 0)
		{
			if (Data >> 8 < eMMW_B_NUM)
			{
				cfg.baud = Data >> 8;
			}
		}
	}
	// the WDT timeout must be at least one minute.
	if (Address == eMMW_HR_WDT)
	{
		if (Data > 0)
		{
			cfg.wdto = Data;
		}
		else
		{
			return 0;
		}
	}
	// the new password must be valid
	if (Address == eMMW_HR_PW)
	{
		if (Data != 0 && Data != C_CMD_COMMIT && Data != C_CMD_CANCEL)
		{
			cfg.pw = Data;
		}
		else
		{
			return 0;
		}
	}

	return 1;
}

/**
 * reads the registers
 * @return: the number of registers read
 *   0 if an error occurred during processing
 *   1 if everything went smoothly
 */
pb_t PetitPortRegRead(pu8_t Address, pu16_t* Data)
{
	// check if you can access this
	if (Address > eMMW_HR_CFG &&
			(cfgSmS != eCFG_Cache && cfgSmS != eCFG_Commit))
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

/**
 * @fn PetitPortInputRegRead
 * reads input registers
 * @return: the number of registers read
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

/**
 * @fn PetitCoilRead
 * reads coils
 * @param Data should be at least the logical value of the coil, not the byte
 *   in which it resides
 * @return: the number of coils read
 *   0 if an error occurred during processing
 *   1 if everything went smoothly
 */
pb_t PetitPortCoilRead(pu16_t Address, pu8_t* Data)
{
	if (Address == 0)
	{
		*Data = sys_ok;
		return 1;
	}
	return 0;
}

/**
 * @fn PetitPortCoilWrite(pu16_t Address, pu16_t Data)
 * writes coils
 * @param Data is a bit larger here to account for the fact that modbus will
 *   use 0xFF00 to write "high" to the coil using single coil write
 * @return: the number of coils written
 *   0 if an error occurred during processing
 *   1 if everything went smoothly
 */
pb_t PetitPortCoilWrite(pu16_t Address, pu16_t Data)
{
	if (Address == 0)
	{
		if (Data)
			sys_ok = true;
		else
			sys_ok = false;
		return 1;
	}
	return 0;
}

// group IoT Supervisor EFM8BB1LCK Petit Modbus Port
/**
 * @}
 */
