/*
 * IoT_Supervisor.h
 *
 *  Created on: Aug 29, 2023
 *      Author: brandon
 */

#ifndef INC_IOT_SUPERVISOR_H_
#define INC_IOT_SUPERVISOR_H_

#include <stdint.h>
#include <stdbool.h>
//-----------------------------------------------------------------------------
// Type Definitions
//-----------------------------------------------------------------------------
#define C_WDT_PET 0x5A
#define C_WDT_DIS 0xA5
#define C_CMD_COMMIT 0x5FAF
#define C_CMD_CANCEL 0x0000

// voltage in state machine _ type
typedef enum
{
	eVIN_Init = 0,
	eVIN_VLow,
	eVIN_OK
} VinSm_t;

// modbus watchdog timer state machine status _ type
typedef enum
{
	eMW_Ini = 0,
	eMW_En,
	eMW_Timeout
} mbWDTsmS_t;

// last reset source _ type
typedef enum
{
	eLR_Init = 0,
	eLR_VSM,
	eLR_WDT
} LastRst_t;

// verification status _ type
typedef enum
{
	eVS_Norm = 0,
	eVS_Cfg,
	eVS_Prog,
	eVS_Setup
} VerifSt_t;

// supervisor (device) status
typedef union
{
	// bitfield access
	struct
	{
		VinSm_t vinSmS :2;
		mbWDTsmS_t wdtSmS :2;
		LastRst_t lastRstS :2;
		VerifSt_t verifSt :2;
	} v;
	// byte access
	uint8_t b;
} sv_dev_sta_t;

// configuration memory state machine _ type
typedef enum
{
	eCFG_Idle = 0,
	eCFG_Load,
	eCFG_Cache,
	eCFG_Commit,
	eCFG_Erase,
	eCFG_Write
} CfgSM_t;

typedef struct
{
	uint8_t sid;
	uint8_t baud;
	uint8_t wdto;
	uint16_t pw;
} cfg_t;

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
extern volatile bool vinSmFlag;
extern volatile bool WDTsmFlag;

extern uint8_t t1Count;
extern bool cprif;

extern uint8_t t0Count;
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

#endif /* INC_IOT_SUPERVISOR_H_ */
