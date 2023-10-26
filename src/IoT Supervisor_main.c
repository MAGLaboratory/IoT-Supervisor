/******************************************************************************
 * @file "IoT Supervisor_main.c"
 *  generated by Hardware Configurator
 *
 * This file will be updated when saving a document.
 * leave the sections inside the "$[...]" comment tags alone
 * or they will be overwritten!!
 *
 * This is the internet of trash supervisor implementation
 * it has two components: voltage monitoring (because the opi doesn't have it)
 * and a watchdog (because stuff crashes all the time and nobody notices)
 *****************************************************************************/

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <hardware.h>
#include <SI_EFM8BB1_Register_Enums.h>                  // SFR declarations
#include "InitDevice.h"
#include "IoT_Supervisor.h"
#include "ModbusMiddleWare.h"
#include "PetitModbus.h"
#include "EFM8BB1_FlashPrimitives.h"
// $[Generated Includes]
// [Generated Includes]$
/**
 * @defgroup Main_Application Main Application
 * @{
 */
/******************************************************************************
 * @brief SiLabs_Startup() Routine
 ******************************************************************************
 * This function is called immediately after reset, before the initialization
 * code is run in SILABS_STARTUP.A51 (which runs before main() ). This is a
 * useful place to disable the watchdog timer, which is enabled by default
 * and may trigger before main() in some instances.
 *****************************************************************************/
void SiLabs_Startup(void)
{
	// $[SiLabs Startup]
	// [SiLabs Startup]$
}

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
volatile sv_dev_sta_t sv_dev_sta = {0};

// controls the main loop
volatile t1Count_t t1Count = 0;

// used in the timer 0 loop
volatile uint8_t t0Count = 0;
/** Software counter to implement modbus inter-byte timeout */
uint8_t T0C_TOP = 0;

/******************************************************************************
 * @defgroup Blinker Heart Beat Blinker
 ******************************************************************************
 * @{
 *
 * This section contains the heart beat blinker.
 *
 * The main driver of the heart beat LED display is a counter and some
 * combinational logic performed on select bits on the counter.
 * The counter selected is the previously implemented ::t1Count which
 * increments every 1ms or so.
 * A slow bit of the counter a bit two times faster are selected.
 * These two bits are then "and"ed together to form the main heartbeat.
 *
 * This program implements two complications on the original heart beat
 * functionality.
 * One is a heart beat skip implemented as the counter ::hbsCount.
 * This counter is triggered on ::hbs_re or "heart beat skip, rising edge"
 * which is triggered on some combinational logic on the same ::t1Count as
 * the main heartbeat.
 * Specifically, it is triggered when the slow counter bit is high and the
 * subsequent lesser-significant bits are zero.
 * In bit-vector terms, this would look something like 0bzzzz zz10 0000 0000
 * where 'z' is a "don't care" bit.
 * The heart beat skip counter is set to 0 the cycle after it reaches its
 * compare match value ::HB_SKIP.
 * The original heart beat ::hb_intrenal can reach additional code when the
 * hbsCount is zero.
 *
 * The other complication upon the original heart beat is a duty cycle
 * limiter.
 * Instead of implementing a compare value, this duty cycle counter is clocked
 * on the execution of this block of code.
 * It contains a state machine with two states: ack and nack.
 * Upon the heart beat reaching this block of code, it is treated as a strobe.
 * In the nack state, a high value on the strobe causes the state machine to
 * transition into the ack state, set the counter to one, and light the LED.
 * In the ack state, the counter up-counts on each execution until it reaches
 * its top value.
 * On the cycle after reaching the top value, the LED is unlit.
 * When the heart beat signal (strobe) is no longer given, the state machine
 * returns to its initial value, nack.
 *****************************************************************************/
#define HB_DIV (9u) // approximately once a second, actually less
                    // must be at least two, but it probably won't look good
                    // at two...
#define HB_DUTYCYCLE (32u) // select zero here to bypass the dutycycle gen
#define HB_SKIP (3u) // number of heartbeat cycles to skip between blinks
#define FAST_BLINK (9u) // one second
#define SLOW_BLINK (FAST_BLINK + 2u)

bool sys_ok = true;
bool configuring = false;
uint8_t hbdCount = 0; // heartbeat dutycycle counter
uint8_t hbsCount = 0; // heartbeat skip counter

void blinker(void)
{
	// sys ok is heart beat
	if (sys_ok == true && configuring == false)
	{
		// one: determine heart beat
		// t1Count & (1 << HB_DIV)
		bool hb_slow = (t1Count & (1 << HB_DIV)) != 0;
		bool hb_fast = (t1Count & (1 << HB_DIV - 2)) != 0;
		// heart beat slow _ rising edge
		bool hbs_re = (t1Count & ((1 << HB_DIV + 1) - 1)) == (1 << HB_DIV);
		// heart beat fast _ rising edge
		bool hbf_re = (t1Count & ((1 << HB_DIV - 1) - 1)) == (1 << HB_DIV - 2);
		bool hb_internal = hb_fast && hb_slow;
		if (hbs_re == true && hbsCount < HB_SKIP)
		{
			hbsCount++;
		}
		else if (hbs_re == true)
		{
			hbsCount = 0;
		}
		// compute heartbeat skip
		hb_internal = hb_internal && (hbsCount == 0);

		// two: LED duty cycle limiter
		if (hb_internal == true && hbf_re == true)
		{
			hbdCount = 1;
			nPWR_LED = 0;
		}

		// if the selected dutycycle is 0, base this part
		// off of the heatbeat generated before
		if (hbdCount < HB_DUTYCYCLE)
		{
			hbdCount++;
		}
		else if (HB_DUTYCYCLE > 0u || hb_internal == false)
		{
			nPWR_LED = 1;
		}
	}
	// sys not ok is short blink
	else if (configuring == false) // sys_ok == false
	{
		nPWR_LED = !(t1Count & (1 << FAST_BLINK));
	}
	// configuring is long blink
	else // configuring == true
	{
		nPWR_LED = !((t1Count & (1 << SLOW_BLINK)));
	}
}
/** @} */

/******************************************************************************
 * @defgroup Voltage_State_Machine Voltage In and Reset State Machine
 ******************************************************************************
 * @{
 * @brief vin and reset the state machine
 *
 * @mermaid{vin}
 * @mermaid{vin_timing}
 * The reset and voltage in state machine responds to reset sources and pulls
 * the reset line down for two seconds upon the reset source triggering.
 * The detailed implementation is different for each reset source.
 * The voltage in reset source pulls down for as long as it is running.
 * It only reaches the two second countdown once the vin state is cleared.
 * The WDT reset source fires once and automatically reaches the two second
 * countdown.
 *****************************************************************************/
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
/**
 * Voltage in state machine.  Perhaps this could be called the main state
 * machine in this application.
 *
 * The state machine starts in the reset mode to enable a boot-up delay on the
 * SBC so that screens and stuff can initialize before the board does.
 */
void VinSm(void)
{
	// static variables
	static uint8_t sCounter = 0;

	// determine input state
	bool VinCmp = VIN_CMP_CPOUT();

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
		sys_ok = false;
		break;
	case eVIN_OK:
		nReset = true;
		modbusWdtSmEn = true;
		sCounter = 0;
		break;
	}

	// output signals
	RESET_P = nReset;
	RESET_LED_SET(nReset);

	// reset flags
	cprif = false;
}
/** @} */ // section Voltage State Machine
/******************************************************************************
 * @defgroup Modbus_WDT_State_Machine Modbus WDT State Machine
 ******************************************************************************
 * @{
 * @brief The modbus watchdog timer state machine.
 * 
 * @mermaid{wdt}
 * @mermaid{wdt_timing}
 * The watchdog state machine is specifically implemented to "bite" after it
 * has not received a watchdog "pet" for a certain number of minutes
 * (configurable).
 *
 * It starts as disabled and can be enabled through modbus.
 * After biting, it returns to being disabled.
 *****************************************************************************/
/** Default number of minutes before modbus timeout. */
#define C_DEFAULT_MB_WD_TIMEOUT (15)
/** number of watchdog counts to minutes */
#define C_MB_WD_COUNT2MIN (7500)

uint8_t MB_WD_TIMEOUT = C_DEFAULT_MB_WD_TIMEOUT;
bool mbWDTen = false;
bool mbWDTpet = false;

/**
 * This is the implementation of the modbus watchdog state machine.
 *
 * It follows the same structure as the rest of the state machines in this
 * application where the transitions are first and the states follow.
 */
void mbWDTsm(void)
{
	// statics
	static uint16_t mbWDTc = 0;		// modbus watchdog timer counter
	static uint16_t mbWDTcM = 0;	// modbus watchdog timer counter (minute)

	// transitions
	switch (sv_dev_sta.v.wdtSmS)
	{
	case eMW_Ini:
		if (mbWDTen && modbusWdtSmEn)
			sv_dev_sta.v.wdtSmS = eMW_En;
		break;
	case eMW_En:
		if (!mbWDTen || mbWDTpet)
			sv_dev_sta.v.wdtSmS = eMW_Ini;
		if (mbWDTcM >= MB_WD_TIMEOUT)
			sv_dev_sta.v.wdtSmS = eMW_Timeout;
		break;
	case eMW_Timeout:
		sv_dev_sta.v.wdtSmS = eMW_Ini;
		break;
	default:
		sv_dev_sta.v.wdtSmS = eMW_Ini;
		break;
	}

	// output
	switch (sv_dev_sta.v.wdtSmS)
	{
	case eMW_Ini:
		mbWDTc = 0;
		mbWDTcM = 0;
		break;
	case eMW_En:
		mbWDTc++;
		if (mbWDTc >= C_MB_WD_COUNT2MIN)
		{
			mbWDTcM++;
			mbWDTc = 0;
		}
		break;
	case eMW_Timeout:
		mbWDTen = false;
		modbusWdtExp = true;
		break;
	}

	// inverse logic
	WDT_LED_SET(sv_dev_sta.v.wdtSmS == eMW_En);

	mbWDTpet = false;
}
/** @} */ // section Modbus WDT State Machine
/******************************************************************************
 * @defgroup Configuration_State_Machine Configuration State Machine
 ******************************************************************************
 * @{
 * @brief The configuration state machine starts and maintains the system 
 * configuration.
 *
 * @mermaid{cfgsm}
 * The configuration state machine implements the startup and configuration
 * tasks of this application.
 * On startup, the configuration is read from memory and the program memory
 * is checked against a stored CRC.
 * On configuration commit, a cached configuration from modbus is applied to
 * the application.
 * On configuration write, the committed configuration is written into flash
 * along with the newly calculated CRC.
 *****************************************************************************/
/**
 * Configuration page start address in program memory (code memory).
 */
#define C_FLASH_CONF (0x1E00)
/** Pointer to the flash config */
#define pFLASH_CONF ((uint8_t code*)C_FLASH_CONF)
/**
 * The default password used to enter configuration cache or configuration page
 * erase and write.
 */
#define C_PW_DEFAULT (0xDEFA)
// manually determined program memory end.
#if defined(DEBUG) && DEBUG
/**
 * The program memory end should be determined using manual methods.  These
 * should be documented in the readme included with this application.
 */
#define C_FOUND_PROG_END (0x101F) // end of program memory to check (exclusive)
#else
#define C_FOUND_PROG_END (0x10D0) // determine me
#endif

// configuration variables
/**
 * Stores the header of the configuration page in FLASH
 *
 * This is the <b>ex</b>pected <b>c</b>on<b>f</b>i<b>g</b>uration
 * <b>header</b>.
 */
code const uint8_t ex_cfg_header[] =
{
	'M','A','G', 	// Makers, Artists, and Gadgeteers Laboratory
	'S','V', 		// IoT Supervisor
	'0', 			// sw major
	'0', 			// sw minor
	'0' 			// header version
};
/** Configuration header length */
#define C_CFG_HEADER_LEN (sizeof(ex_cfg_header))
/**
 * Stores the default configuration data.
 *
 * @note The default modbus sid is 255 because this sid is specifically
 * reserved for devices that have not been configured yet.
 */
code const cfg_t default_cfg =
{
	255, // sid, the default is 255
	eMMW_B_38400, // baud
	C_DEFAULT_MB_WD_TIMEOUT, // fifteen minutes
	C_PW_DEFAULT
};
/**
 * Configuration data length
 *
 * The password is stored in two bytes.
 */
#define C_CFG_DATA_LEN (sizeof(cfg_t))
/** configuration CRC length (16 bits) */
#define C_CFG_C_CRC_LEN (2)
/** program CRC length (16 bits) */
#define C_CFG_P_CRC_LEN (2)
/**
 * End of configuration data offset in configuration page.
 */
#define C_CFG_DATA_END (C_CFG_HEADER_LEN + C_CFG_DATA_LEN)
/**
 * End of configuration CRC offset in configuration page.
 */
#define C_CFG_C_CRC_END (C_CFG_DATA_END + C_CFG_C_CRC_LEN)
/**
 * End of program CRC offset in configuration page.
 * Total configuration length.
 */
#define C_CFG_P_CRC_END (C_CFG_C_CRC_END + C_CFG_P_CRC_LEN)

CfgSM_t cfgSmS = eCFG_Load;
bool pw_flag = false;
bool dir_tx = false;
bool run_petitmodbus = false;
uint16_t pw; /** entered password, not necessarily the correct password */
cfg_t cfg;
uint8_t cfg_write_idx = 0;
uint16_t calc_cfg_crc = 0xFFFF;
uint16_t calc_prog_crc = 0xFFFF;

/**
 * This function performs the code memory configuration checks.  It first
 * checks if the values from memory are in-range and then checks if the
 * CRC against the value stored in code memory.
 *
 * @return the validity of the code memory configuration.  true if okay.
 */
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
	if(i < C_SID_MIN || i > C_SID_MAX)
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

/**
 * This function checks the configuration stored in data memory.  It does not
 * perform a CRC.
 *
 * @return the validity of the configuration. true if okay.
 */
bool dcfg_check()
{
	// check SID
	if (cfg.sid < C_SID_MIN || cfg.sid > C_SID_MAX)
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

/******************************************************************************
 * This function performs the tasks in the configuration load state in the
 * configuration state machine.
 * This includes loading the configuration from code memory and checking the
 * CRC of program memory.
 *
 * \mermaid{cfgmem}
 *
 * The verification status is set based on greater than logic.
 * If code memory lacks the correct configuration header, the verification
 * status is "Setup."
 * If the program memory CRC does not match what is stored in the
 * configuration, the verification status is "Program [invalid]."
 * If code memory is invalid or deemed corrupted by an incorrect CRC, the
 * verification status is "Configuration [invalid]."
 *****************************************************************************/
void cfg_load()
{
	uint8_t code* addr;
	uint8_t i = 0;
	bool cfg_ok = true;

	cfg_ok = ccfg_check();

	// calculate PROG CRC
	// this is set to run even if the CRC is not checked in order to ensure
	// a valid startup time for debugging is achieved
	calc_prog_crc = 0xFFFF;
	for (addr =	0; addr < C_FOUND_PROG_END; addr++)
	{
		KirisakiCRC16Calc(*addr, &calc_prog_crc);
	}

	// check PROG CRC
	if (sv_dev_sta.v.verifSt != eVS_Setup)
	{
		i = *(pFLASH_CONF + C_CFG_C_CRC_END);
		// check low byte
		if ((calc_prog_crc & 0xFF) != i)
		{
			sv_dev_sta.v.verifSt = eVS_Prog;
			sys_ok = false;
		}
		else
		{
			// check high byte
			i = *(pFLASH_CONF + C_CFG_C_CRC_END + 1);
			if (calc_prog_crc >> 8 != i)
			{
				sv_dev_sta.v.verifSt = eVS_Prog;
				sys_ok = false;
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
		sys_ok = false;
	}

	// apply configuration structure
	mmw_init(cfg.sid, cfg.baud);
	MB_WD_TIMEOUT = cfg.wdto;

	cfgSmS = eCFG_Idle;
}

/**
 * This function writes the data memory configuration into code memory.
 * It also calculates the CRC so that later reads can determine the integrity
 * of the configuration data.
 */
void cfg_write(void)
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
	// wdt top
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

/**
 * This function implements the configuration state machine which controls
 * various parts of the supervisor's behavior.  It controls the modbus sid,
 * modbus baud rate, watchdog timer, and configuration memory password.
 *
 * The configuration state machine locks the modbus master out of reading or
 * writing the configuration memory if the state machine is not put into either
 * cache or commit state.
 *
 * Caches or commits can be escaped from if the modbus user does not want to
 * write data to code memory.
 */
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
				configuring = true;
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
				configuring = false;
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
				configuring = false;
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
/** @} */ // section Configuration State Machine

/******************************************************************************
 * This is the main function where the configuration and modbus are run.
 *
 * It is assumed that the main function is run after the cinit and startup
 * implementations, so all global and static variables are initialized to their
 * correct default values.
 *
 * The main function takes into consideration the reset condition to determine
 * whether the vin state machine should start into the "ok" state or the
 * "Init" state.
 *
 * The main function implements a counter-based system using the ::t1Count
 * global which is incremented once a millisecond.
 * An internal counter is incremented each time the modbus loop is executed.
 * This makes it acceptable for the modbus process to overrun its timeslot
 * through multiple cycles without affecting the number of times it is
 * executed for timing purposes.
 * Note that there is an
 * anti-windup feature activated when ::run_petitmodbus is not set where
 * the internal counter is immediately set to the value of ::t1Count rather
 * than incremented upon each execution.
 * @return main functions usually have a return that should never get reached.
 *****************************************************************************/
int main(void)
{
	static t1Count_t t1CountLast = 0;
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
		if (t1Count - t1CountLast >= 1u)
		{
			PETIT_PROCESS_ON();
			CFGsm();
			blinker();
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
	// this return should never be reached
}
/** @} */ // group Main_Application
