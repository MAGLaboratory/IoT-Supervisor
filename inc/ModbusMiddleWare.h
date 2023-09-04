/*
 * ModbusMiddleWare.h
 *
 * I want to call it MMW
 *
 *  Created on: Aug 19, 2023
 *      Author: brandon
 */

#ifndef INC_MODBUSMIDDLEWARE_H_
#define INC_MODBUSMIDDLEWARE_H_
#include <stdint.h>

// the first speed is null because of the mechanism used to choose modbus
// speeds
typedef enum
{
	eMWW_B_NULL = 0,
	eMMW_B_38400,
	eMMW_B_19200,
	eMMW_B_9600,
	eMMW_B_4800,
	eMMW_B_2400,
	eMMW_B_1200,
	eMMW_B_NUM // total number of speeds
} mmw_speeds_t;

typedef enum
{
	eMMW_CT_T1_PRE = 0,
	eMMW_CT_TH1,
	eMMW_CT_PETIT_TOP,
	eMMW_CT_TL0_TOP,
	eMMW_CT_NUM // total number of configuration table entries
} mmw_ct_i_t;

// holding registers
typedef enum
{
	eMMW_HR_STA = 0,
	eMMW_HR_CFG,
	eMMW_HR_MB,
	eMMW_HR_WDT,
	eMMW_HR_PW
} mmw_hr_t;

extern uint8_t PETITMODBUS_DLY_TOP;
extern uint8_t PETITMODBUS_SLAVE_ADDRESS;

void mmw_init(uint8_t sid, uint8_t baud);

#endif /* INC_MODBUSMIDDLEWARE_H_ */
