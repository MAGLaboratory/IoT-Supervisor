#include "ModbusMiddleWare.h"
#include "IoT_Supervisor.h"
#include <stdint.h>
#include <SI_EFM8BB1_Register_Enums.h>

// modbus middle ware configuration table
code const uint8_t mmw_ct[eMMW_B_NUM][eMMW_CT_NUM] =
{
	{0, 0xEC,  0, 0},
	{0, 0xD8,  0, 0},
	{0, 0xB0,  1, 0},
	{0, 0x60,  4, 1},
	{1, 0xE5, 10, 3},
	{1, 0xCB, 22, 7}
};

uint8_t PETITMODBUS_DLY_TOP = 0;
uint8_t PETITMODBUS_SLAVE_ADDRESS = 0;

// Functions

void mmw_init(uint8_t sid, uint8_t baud)
{
	PETITMODBUS_SLAVE_ADDRESS = sid;
	if (baud == 0)
	{
		return;
	}

	// stop UART
	SCON0 &= ~SCON0_REN__BMASK & ~SCON0_MCE__BMASK;
	// stop timer 1
	TMOD = (TMOD & ~TMOD_T1M__FMASK) | TMOD_T1M__MODE3;

	// decode T1 clock source
	if (mmw_ct[baud - 1][eMMW_CT_T1_PRE] == 1)
	{
		// prescaler
		CKCON0 = (CKCON0 & ~CKCON0_T1M__BMASK) | CKCON0_T1M__PRESCALE;
	}
	else
	{
		// system clock
		CKCON0 = (CKCON0 & ~CKCON0_T1M__BMASK) | CKCON0_T1M__SYSCLK;
	}

	// set timer 1 mode to mode 2 (8-bit counter/timer with auto-reload)
	TMOD = (TMOD & ~TMOD_T1M__FMASK) | TMOD_T1M__MODE2;

	// set timer 1 current value and reload value
	TH1 = (mmw_ct[baud - 1][eMMW_CT_TH1] << TH1_TH1__SHIFT);
	TL1 = (mmw_ct[baud - 1][eMMW_CT_TH1] << TL1_TL1__SHIFT);

	// maybe this code will support parity values in the future, but the
	// efm8bb1 does not support parity.

	// enable UART, interestingly enough the MCE bit is just for
	// normal interrupts
	SCON0 |= SCON0_REN__RECEIVE_ENABLED | SCON0_MCE__MULTI_ENABLED;

	// software counter values
	PETITMODBUS_DLY_TOP = mmw_ct[baud - 1][eMMW_CT_PETIT_TOP];
	T0C_TOP = mmw_ct[baud - 1][eMMW_CT_TL0_TOP];
}
