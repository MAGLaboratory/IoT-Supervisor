/******************************************************************************
 * @file IoT_Supervisor.h
 *
 * This file contains globals that are used throughout this application.
 * (Or at least from the main file and the interrupts...)
 *
 * @date Aug 29, 2023
 * @author brandon
 *****************************************************************************/

#ifndef INC_IOT_SUPERVISOR_H_
#define INC_IOT_SUPERVISOR_H_
#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
/** @addtogroup Modbus_WDT_State_Machine
 *  @{
 */
#define C_WDT_PET 0x5A
#define C_WDT_DIS 0xA5
/** @} */ // group Modbus WDT State Machine
/** @addtogroup Configuration_State_Machine
 *  @{
 */
#define C_CMD_COMMIT 0x5FAF
#define C_CMD_CANCEL 0x0000
#define C_SID_MIN 1
#define C_SID_MAX 247
/** @} */ // group Configuration_State_Machine
//-----------------------------------------------------------------------------
// Type Definitions
//-----------------------------------------------------------------------------
#define t1Count_t uint16_t

/**
 * @addtogroup Voltage_State_Machine
 * @{
 */
/**
 * Voltage in (and Reset) State Machine _ Type
 *
 * <b>V</b>oltage <b>in</b> (and Reset) <b>S</b>tate <b>M</b>achine
 * <b>_ T</b>ype
 */
typedef enum
{
	/**
	 * Voltage in and Reset State Machine Initial State
	 *
	 * The state machine sets reset to true in this state, and the state
	 * machine transitions into this state when the Modbus WDT bites.
	 */
	eVIN_Init = 0,/**< eVIN_Init */
	eVIN_VLow, ///< Voltage Low State
	eVIN_OK ///< Voltage in and Reset State Machine OK
} VinSm_t;
/** @} */ // group Voltage_State_Machine
/**
 * @addtogroup Modbus_WDT_State_Machine
 * @{
 */
/**
 * Modbus Watchdog Timer State Machine Status _ Type
 *
 * <b>M</b>od<b>b</b>us <b>W</b>atch<b>d</b>og <b>T</b>imer <b>S</b>tate
 * <b>M</b>achine <b>S</b>tatus <b>_ T</b>ype
 */
typedef enum
{
	eMW_Ini = 0, ///< Initial State.
	eMW_En, ///< Enable State
	eMW_Timeout ///< Timeout State.  This watchdog bites!
} mbWDTsmS_t;
/** @} */ // Modbus_WDT_State_Machine
/**
 * @addtogroup Main_Application
 * @{
 */
/**
 * Last Reset Source _ Type
 *
 * <b>Last</b> <b>R</b>e<b>s</b>e<b>t</b> Source <b>_ T</b>ype
 */
typedef enum
{
	eLR_Init = 0, ///< Initial State.  No recorded reset.
	eLR_VSM, ///< A low voltage condition caused the last reset.
	eLR_WDT ///< The modbus watchdog timer caused the last reset.
} LastRst_t;
/** @} */ // Main_Application
/**
 * @addtogroup Configuration_State_Machine
 * @{
 */
/**
 * Verification Status _ Type
 *
 * <b>Verif</b>ication <b>St</b>atus <b>_ T</b>ype
 */
typedef enum
{
	/**
	 * Normal Verification Status
	 *
	 * No configuration errors or program memory CRC mismatch detected.
	 */
	eVS_Norm = 0,/**< eVS_Norm */
	eVS_Cfg, ///< Configuration Error or CRC Mismatch Detected
	eVS_Prog, ///< Program Memory Mismatch Detected
	eVS_Setup ///< Initial Setup.
} VerifSt_t;

/**
 * Configuration Memory State Machine _ Type
 *
 * <b>C</b>on<b>f</b>i<b>g</b>uration Memory <b>S</b>tate <b>M</b>achine
 * <b>_ T</b>ype
 */
typedef enum
{
	eCFG_Idle = 0, ///< Idle State.  No configuration state machine activity.
	eCFG_Load, ///< Configuration Loading from memory
	eCFG_Cache, ///< New Configuration is Caching from Modbus
	eCFG_Commit, ///< New Configuration is Committed (Active)
	eCFG_Erase, ///< Erase the old Configuration (from Flash)
	eCFG_Write ///< Write the new Configuration (into Flash)
} CfgSM_t;

/// This struct is a storage type for flash memory.
typedef struct
{
	uint8_t sid;
	uint8_t baud;
	uint8_t wdto;
	uint16_t pw;
} cfg_t;
/** @} */ // group Configuration_State_Machine

/**
 * @addtogroup Main_Application
 * @{
 */
/**
 * Supervisor (Device) Status _ Type
 *
 * <b>S</b>uper<b>v</b>isor (<b>Dev</b>ice) <b>Sta</b>tus <b>_ T</b>ype
 */
typedef union
{
	/* bitfield access
	 *
	 * @note these fields are represented in LSB first which is normal in C
	 */
	struct
	{
		VinSm_t vinSmS :2;
		mbWDTsmS_t wdtSmS :2;
		LastRst_t lastRstS :2;
		VerifSt_t verifSt :2;
	} v;
	/// byte access
	uint8_t b;
} sv_dev_sta_t;
/** @} */ // Main_Application

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
extern bool sys_ok;

extern volatile t1Count_t t1Count;
extern volatile bool cprif;

extern volatile uint8_t t0Count;
extern uint8_t T0C_TOP;

extern sv_dev_sta_t sv_dev_sta;

extern bool mbWDTen;
extern bool mbWDTpet;
extern uint8_t MB_WD_TIMEOUT;

extern bool pw_flag;
extern bool dir_tx;
extern uint16_t pw;
extern cfg_t cfg;

extern CfgSM_t cfgSmS;
//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
void VinSm(void);
void mbWDTsm(void);

#endif /* INC_IOT_SUPERVISOR_H_ */
