//=========================================================
// src/IoT Supervisor_main.c: generated by Hardware Configurator
//
// This file will be updated when saving a document.
// leave the sections inside the "$[...]" comment tags alone
// or they will be overwritten!!
//=========================================================
// This is the internet of trash supervisor
// it has two components: voltage monitoring (because the opi doesn't have it)
// and a watchdog (because stuff crashes all the time and nobody notices)

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <SI_EFM8BB1_Register_Enums.h>                  // SFR declarations
#include "InitDevice.h"
#include "IoT_Supervisor.h"
#include "ModbusMiddleWare.h"
#include "debugpins.h"
#include "PetitModbus.h"
#include "EFM8BB1_FlashPrimitives.h"
// $[Generated Includes]
// [Generated Includes]$

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define C_MB_WD_COUNT2MIN (7500)
#define C_DEFAULT_MB_WD_TIMEOUT (15)
#define C_FLASH_CONF (0x1E00) // last page of flash
#define pFLASH_CONF ((uint8_t code*)C_FLASH_CONF) // a pointer to the flash cfg
#define C_PW_DEFAULT (0xDEFA) // default
// manually determined program memory end.
#if defined(DEBUG) && DEBUG
#define C_FOUND_PROG_END (0x0FDB) // end of program memory to check (exclusive)
#else
#define C_FOUND_PROG_END (0x0FA1) // determine me
#endif

code const uint8_t ex_cfg_header[] =
{
	'M','A','G', 	// Makers, Artists, and Gadgeteers Laboratory
	'S','V', 		// IoT Supervisor
	'0', 			// sw major
	'0', 			// sw minor
	'0' 			// header version
};
#define C_CFG_HEADER_LEN (8)

code const cfg_t default_cfg =
{
	255, // sid, the default is 255
	eMMW_B_38400, // baud
	C_DEFAULT_MB_WD_TIMEOUT, // fifteen minutes
	C_PW_DEFAULT
};
#define C_CFG_DATA_LEN (5)
#define C_CFG_C_CRC_LEN (2)
#define C_CFG_P_CRC_LEN (2)

//-----------------------------------------------------------------------------
// SiLabs_Startup() Routine
// ----------------------------------------------------------------------------
// This function is called immediately after reset, before the initialization
// code is run in SILABS_STARTUP.A51 (which runs before main() ). This is a
// useful place to disable the watchdog timer, which is enabled by default
// and may trigger before main() in some instances.
//-----------------------------------------------------------------------------
void SiLabs_Startup(void)
{
	// $[SiLabs Startup]
	// [SiLabs Startup]$
}

//-----------------------------------------------------------------------------
// "Global" Variables
//-----------------------------------------------------------------------------
volatile bool WDTsmFlag = false;
sv_dev_sta_t sv_dev_sta = {0};

uint8_t t1Count = 0;
uint8_t t1CountLast = 0;

// used in the timer 0 loop
uint8_t t0Count = 0;
uint8_t T0C_TOP = 0;
//-----------------------------------------------------------------------------
// the state machine
//-----------------------------------------------------------------------------
//                         VinLow == False
// ┌────────────────────────────────────────────────────────────┐
// │                                                            │
// │                       VinLow Interrupt    ┌────────────┐   │
// │                  ┌───────────────────────►│            │   │
// │                  │                        │    VLow    ├───┘
// │   ┌────────────┐ │                     ┌─►│            │
// │   │            ├─┘                     │  └────────────┘
// ├──►│    Init    │                       │  nReset = low
// │   │            ├─┐     VinLow Interrupt│
// │   └────────────┘ │                     │
// │   nReset = low   │                     │
// │                  │                     │  ┌────────────┐
// │                  │ VinLow == False     └──┤            │
// │                  │    2 seconds           │     OK     ├───┐
// │                  └───────────────────────►│            │   │
// │                                           └────────────┘   │
// │                                            nReset = high   │
// │                                           Modbus WDT SMEn  │
// │                 Modbus WDT Timeout                         │
// └────────────────────────────────────────────────────────────┘
// == outputs ==
bool nReset = false; // inverse logic
bool modbusWdtSmEn = false;
// == inputs ==
// comparator input flag
bool cprif = false; // set outside, cleared here
// modbus watchdog expired
bool modbusWdtExp = false; // set outside, cleared in this SM

// == constants ==
#define SCOUNTER_ONE_SECOND (125)
// 125 for one second

//-----------------------------------------------------------------------------
// modbus stuff
//-----------------------------------------------------------------------------
// flags
bool modbusRunFlag = false;

void VinSm(void)
{
	// static variables
	static uint8_t sCounter = 0;

	// determine input state
	bool VinCmp = CMP0CN0 & CMP0CN0_CPOUT__BMASK;

	// default output states
	nReset = false;
	modbusWdtSmEn = false;

	// transitions
	switch (sv_dev_sta.v.vinSmS)
	{
	case eVIN_Init:
		if (cprif == true || VinCmp == true)
		{
			sv_dev_sta.v.vinSmS = eVIN_VLow;
		}
		else if (sCounter >= SCOUNTER_ONE_SECOND * 2 - 1)
		{
			sv_dev_sta.v.vinSmS = eVIN_OK;
		}
		break;
	case eVIN_VLow:
		if (VinCmp == false)
		{
			sv_dev_sta.v.vinSmS = eVIN_Init;
		}
		break;
	case eVIN_OK:
		if (cprif == true || VinCmp == true)
		{
			sv_dev_sta.v.vinSmS = eVIN_VLow;
			sv_dev_sta.v.lastRstS = eLR_VSM;
		}
		else if (modbusWdtExp == true)
		{
			sv_dev_sta.v.vinSmS = eVIN_VLow;
			sv_dev_sta.v.lastRstS = eLR_WDT;
		}
		break;
	default:
		sv_dev_sta.v.vinSmS = eVIN_VLow;
		break;
	}

	// outputs
	switch (sv_dev_sta.v.vinSmS)
	{
	case eVIN_Init:
		sCounter++;
		modbusWdtExp = false;
		break;
	case eVIN_VLow:
		sCounter = 0;
		break;
	case eVIN_OK:
		nReset = true;
		modbusWdtSmEn = true;
		sCounter = 0;
		break;
	}

	// output signals
	RESET_P = nReset;
	nLED = nReset;

	// reset flags
	cprif = false;
}

//-----------------------------------------------------------------------------
// Modbus WDT State Machine
//-----------------------------------------------------------------------------
uint8_t MB_WD_TIMEOUT = 15;
bool mbWDTen = false;
bool mbWDTpet = false;
void mbWDTsm(void)
{
	// statics
	static mbWDTsmS_t mbWDTsmS = eMW_Ini;
	static uint16_t mbWDTc = 0;		// modbus watchdog timer counter
	static uint8_t mbWDTcM = 0;	// modbus watchdog timer counter (minute)

	// transitions
	switch (mbWDTsmS)
	{
	case eMW_Ini:
		if (mbWDTen && modbusWdtSmEn)
			mbWDTsmS = eMW_En;
		break;
	case eMW_En:
		if (!mbWDTen || mbWDTpet)
			mbWDTsmS = eMW_Ini;
		if (mbWDTcM >= MB_WD_TIMEOUT)
			mbWDTsmS = eMW_Timeout;
		break;
	case eMW_Timeout:
		mbWDTsmS = eMW_Ini;
		break;
	default:
		mbWDTsmS = eMW_Ini;
		break;
	}

	// output
	switch (mbWDTsmS)
	{
	case eMW_Ini:
		mbWDTc = 0;
		mbWDTcM = 0;
		break;
	case eMW_En:
		mbWDTc++;
		if (mbWDTc >= C_MB_WD_COUNT2MIN)
			mbWDTcM++;
		break;
	case eMW_Timeout:
		mbWDTen = false;
		modbusWdtExp = true;
		break;
	}

	mbWDTpet = false;
}

//-----------------------------------------------------------------------------
// config state machine
//-----------------------------------------------------------------------------
CfgSM_t cfgSmS = eCFG_Load;
bool pw_flag = false;
bool dir_tx = false;
bool run_petitmodbus = false;
uint16_t pw; // entered password, not necessarily the correct password
cfg_t cfg;
uint8_t cfg_write_idx = 0;
uint16_t calc_cfg_crc = 0xFFFF;
uint16_t calc_prog_crc = 0xFFFF;

#define C_CFG_DATA_END (C_CFG_HEADER_LEN + C_CFG_DATA_LEN)
#define C_CFG_C_CRC_END (C_CFG_DATA_END + C_CFG_C_CRC_LEN)
#define C_CFG_P_CRC_END (C_CFG_C_CRC_END + C_CFG_P_CRC_LEN)

// returns true if okay
// returns false if not okay
bool ccfg_check(void)
{
	uint8_t i = 0;
	calc_cfg_crc = 0xFFFF;
	// check header
	for (i = 0; i < C_CFG_HEADER_LEN; i++)
	{
		if (*(pFLASH_CONF + i) != ex_cfg_header[i])
		{
			sv_dev_sta.v.verifSt = eVS_Setup;
			return false;
		}
		KirisakiCRC16Calc(ex_cfg_header[i], &calc_cfg_crc);
	}

	// check contents
	// modbus SID
	i = *(pFLASH_CONF + C_CFG_HEADER_LEN);
	if(i < 1 || i > 247)
	{
		sv_dev_sta.v.verifSt = eVS_Cfg;
		return false;
	}
	else
	{
		cfg.sid = i;
		KirisakiCRC16Calc(i, &calc_cfg_crc);
	}
	// modbus baud config
	i = *(pFLASH_CONF + C_CFG_HEADER_LEN + 1);
	if (i == 0 || i >= eMMW_B_NUM)
	{
		sv_dev_sta.v.verifSt = eVS_Cfg;
		return false;
	}
	else
	{
		cfg.baud = i;
		KirisakiCRC16Calc(i, &calc_cfg_crc);
	}
	// modbus WD timeout
	i = *(pFLASH_CONF + C_CFG_HEADER_LEN + 2);
	if (i == 0)
	{
		sv_dev_sta.v.verifSt = eVS_Cfg;
		return false;
	}
	else
	{
		cfg.wdto = i;
		KirisakiCRC16Calc(i, &calc_cfg_crc);
	}
	// password
	i = *(pFLASH_CONF + C_CFG_HEADER_LEN + 3);
	cfg.pw = i;
	KirisakiCRC16Calc(i, &calc_cfg_crc);
	i = *(pFLASH_CONF + C_CFG_HEADER_LEN + 4);
	cfg.pw |= (uint16_t)i << 8;
	KirisakiCRC16Calc(i, &calc_cfg_crc);
	if (cfg.pw == C_CMD_COMMIT || cfg.pw == C_CMD_CANCEL)
	{
		sv_dev_sta.v.verifSt = eVS_Cfg;
		return false;
	}
	// check CFG CRC
	i = *(pFLASH_CONF + C_CFG_DATA_END);
	if ((calc_cfg_crc & 0xFF) != i)
	{
		sv_dev_sta.v.verifSt = eVS_Cfg;
		return false;
	}
	else
	{
		i = *(pFLASH_CONF + C_CFG_DATA_END + 1);
		if (calc_cfg_crc >> 8 != i)
		{
			sv_dev_sta.v.verifSt = eVS_Cfg;
			return false;
		}
	}
	return true;
}

// returns true if ok / normal
// returns false if otherwise
bool dcfg_check()
{
	// check SID
	if (cfg.sid < 1 || cfg.sid > 247)
	{
		return false;
	}
	// check baud enum (no parity)
	if (cfg.baud < 1 || cfg.baud >= eMMW_B_NUM)
	{
		return false;
	}
	// check wdt timeout
	if (cfg.wdto < 1)
	{
		return false;
	}
	// check password
	if (cfg.pw == C_CMD_COMMIT || cfg.pw == C_CMD_CANCEL)
	{
		return false;
	}
	return true;
}

// helpers
void cfg_load()
{
	uint8_t code* addr;
	uint8_t i = 0;
	bool cfg_ok = true;

	cfg_ok = ccfg_check();

	// calculate PROG CRC
	calc_prog_crc = 0xFFFF;
	for (addr =	0; addr < C_FOUND_PROG_END; addr++)
	{
		KirisakiCRC16Calc(*addr, &calc_prog_crc);
	}

	// check PROG CRC
	if (sv_dev_sta.v.verifSt != eVS_Setup)
	{
		i = *(pFLASH_CONF + C_CFG_C_CRC_END);
		if ((calc_prog_crc & 0xFF) != i)
		{
			sv_dev_sta.v.verifSt = eVS_Prog;
		}
		else
		{
			i = *(pFLASH_CONF + C_CFG_C_CRC_END + 1);
			if (calc_prog_crc >> 8 != i)
			{
				sv_dev_sta.v.verifSt = eVS_Prog;
			}
		}
	}

	if (cfg_ok == false)
	{
		// load the default configuration structure
		cfg.sid = default_cfg.sid;
		cfg.baud = default_cfg.baud;
		cfg.wdto = default_cfg.wdto;
		cfg.pw = default_cfg.pw;
	}

	// apply configuration structure
	mmw_init(cfg.sid, cfg.baud);
	MB_WD_TIMEOUT = cfg.wdto;

	cfgSmS = eCFG_Idle;
}

void cfg_write()
{
	calc_cfg_crc = 0xFFFF;
	for (cfg_write_idx = 0; cfg_write_idx < C_CFG_HEADER_LEN; cfg_write_idx++)
	{
		FLASH_ByteWrite(C_FLASH_CONF + cfg_write_idx,
				ex_cfg_header[cfg_write_idx]);
		KirisakiCRC16Calc(ex_cfg_header[cfg_write_idx], &calc_cfg_crc);
	}
	// sid
	FLASH_ByteWrite(C_FLASH_CONF + cfg_write_idx++, cfg.sid);
	KirisakiCRC16Calc(cfg.sid, &calc_cfg_crc);
	// baud
	FLASH_ByteWrite(C_FLASH_CONF + cfg_write_idx++, cfg.baud);
	KirisakiCRC16Calc(cfg.baud, &calc_cfg_crc);
	// wdto
	FLASH_ByteWrite(C_FLASH_CONF + cfg_write_idx++, cfg.wdto);
	KirisakiCRC16Calc(cfg.wdto, &calc_cfg_crc);
	// pw
	FLASH_ByteWrite(C_FLASH_CONF + cfg_write_idx++, cfg.pw & 0xFF);
	KirisakiCRC16Calc(cfg.pw & 0xFF, &calc_cfg_crc);
	FLASH_ByteWrite(C_FLASH_CONF + cfg_write_idx++, cfg.pw >> 8);
	KirisakiCRC16Calc(cfg.pw >> 8, &calc_cfg_crc);
	// cfg crc
	FLASH_ByteWrite(C_FLASH_CONF + cfg_write_idx++,	calc_cfg_crc & 0xFF);
	FLASH_ByteWrite(C_FLASH_CONF + cfg_write_idx++,	calc_cfg_crc >> 8);
	// prog crc
	FLASH_ByteWrite(C_FLASH_CONF + cfg_write_idx++, calc_prog_crc & 0xFF);
	FLASH_ByteWrite(C_FLASH_CONF + cfg_write_idx++, calc_prog_crc >> 8);

	// reset to complete
	RSTSRC |= RSTSRC_SWRSF__SET;
	// uh, set the state machine to idle in case we have not reset
	cfgSmS = eCFG_Idle;
}

void CFGsm()
{
	// transitions
	switch (cfgSmS)
	{
	case eCFG_Load:
		// transitions handled in-function
		break;
	case eCFG_Idle:
		if (pw_flag)
		{
			if (pw == cfg.pw)
			{
				cfgSmS = eCFG_Cache;
			}
			pw_flag = false;
		}
		break;
	case eCFG_Cache:
		if (pw_flag && Petit_RxTx_State == PETIT_RXTX_RX && dir_tx == false)
		{
			if (pw == C_CMD_COMMIT)
			{
				if (dcfg_check())
				{
					cfgSmS = eCFG_Commit;
					mmw_init(cfg.sid, cfg.baud);
					MB_WD_TIMEOUT = cfg.wdto;
				}
			}
			if (pw == C_CMD_CANCEL)
			{
				cfgSmS = eCFG_Idle;
				cfg = default_cfg;
			}
			pw_flag = false;
		}
		break;
	case eCFG_Commit:
		if (pw_flag && Petit_RxTx_State == PETIT_RXTX_RX && dir_tx == false)
		{
			if (pw == cfg.pw)
			{
				cfgSmS = eCFG_Erase;
			}
			if (pw == C_CMD_CANCEL)
			{
				cfgSmS = eCFG_Idle;
			}
			pw_flag = false;
		}
		break;
	case eCFG_Erase:
			cfgSmS = eCFG_Write;
		break;
	case eCFG_Write:
		// transitions handled in function
		break;
	default:
		cfgSmS = eCFG_Idle;
		break;
	}

	// outputs
	switch (cfgSmS)
	{
	case eCFG_Load:
		cfg_load();
		run_petitmodbus = false;
		break;
	case eCFG_Idle:
		run_petitmodbus = true;
		break;
	case eCFG_Cache:
		run_petitmodbus = true;
		mbWDTen = false;
		// the magic in processing actually happens in the modbus rx function
		break;
	case eCFG_Commit:
		run_petitmodbus = true;
		// functionality implemented in the transitions, believe it or not
		break;
	case eCFG_Erase:
		run_petitmodbus = false;
		FLASH_PageErase(C_FLASH_CONF);
		break;
	case eCFG_Write:
		run_petitmodbus = false;
		cfg_write();
		break;
	}
}

//-----------------------------------------------------------------------------
// main() Routine
// ----------------------------------------------------------------------------
int main(void)
{
	// reset handling code
	//   skip startup reset if a software reset was called
	if ((RSTSRC & RSTSRC_SWRSF__BMASK) == RSTSRC_SWRSF__SET)
	{
		sv_dev_sta.v.vinSmS = eVIN_OK;
	}
	// Call hardware initialization routine
	enter_DefaultMode_from_RESET();
#if defined(DEBUG) && DEBUG
	// enter debugmode to get the pins working
	enter_DebugMode_from_DefaultMode();
#endif

	while (1)
	{
		// apparently, casting this to unsigned is necessary
		if (t1Count - t1CountLast >= (uint8_t)1)
		{
			PETIT_PROCESS_ON();
			CFGsm();
			if (run_petitmodbus)
			{
				t1CountLast++;
				ProcessPetitModbus();
			}
			else
			{
				t1CountLast = t1Count;
			}
			PETIT_PROCESS_OFF();
		}

		//     idle mode taken out because of some race condition that keeps
		// being reached.

		// $[Generated Run-time code]
		// [Generated Run-time code]$
	}
}
