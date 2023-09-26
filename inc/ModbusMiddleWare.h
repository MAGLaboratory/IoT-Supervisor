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

/**
 * @addtogroup Modbus_MiddleWare
 * @{
 */

/**
 * This enumeration contains the speeds used by the Modbus MiddleWare.
 *
 * Note that these are standard UART speeds.
 */
typedef enum
{
	/**
	 * The first modbus speed is null because of the mechanism used to set
	 * individual bitfields within the modbus register.
	 */
	eMMW_B_NULL = 0,                                         /**< eMMW_B_NULL */
	eMMW_B_38400,   /**< eMMW_B_38400 */                     /**< eMMW_B_38400 */
	eMMW_B_19200,   /**< eMMW_B_19200 */                     /**< eMMW_B_19200 */
	eMMW_B_9600,    /**< eMMW_B_9600 */                      /**< eMMW_B_9600 */
	eMMW_B_4800,    /**< eMMW_B_4800 */                      /**< eMMW_B_4800 */
	eMMW_B_2400,    /**< eMMW_B_2400 */                      /**< eMMW_B_2400 */
	eMMW_B_1200,    /**< eMMW_B_1200 */                      /**< eMMW_B_1200 */
	eMMW_B_NUM      /**< The total number of modbus speeds *//**< eMMW_B_NUM */
} mmw_speeds_t;

/**
 * This enumeration contains the number of entries across the MMW table.
 *
 * CL in this context stands for Configuration List.
 */
typedef enum
{
	/**
	 * Timer 1 Prescale Control.
	 *
	 * This flag controls the hardware prescale selection.
	 */
	eMMW_CL_T1_PRE = 0,
	/**
	 * Timer High 1 Value.
	 *
	 * This is the timer reload value for determining the UART baud rate.
	 * Each BAUD is two timer overflow periods, probably.
	 */
	eMMW_CL_TH1,
	/**
	 * Petit Modbus delay cycles.
	 *
	 * The main application runs on a 1ms loop and it needs to be delayed to
	 * meet the T_3.5 character timing period for transmission line
	 * direction switch.
	 */
	eMMW_CL_PETIT_TOP,
	/**
	 * Timer 0 Low Reload.
	 *
	 * This value controls the T_1.5 character timing period for maximum
	 * intra-octet timing.
	 */
	eMMW_CL_TL0_TOP,
	eMMW_CL_NUM /**< total number of configuration table entries */
} mmw_cl_i_t;

/**
 * This is the enumeration for all <b>H</b>olding <b>R</b>egisters
 *
 * @note These are not input registers.  The input registers are read-only.
 */
typedef enum
{
	eMMW_HR_STA = 0, /**< SV Status and MB Watchdog Control */
	eMMW_HR_CFG, /**< Password and Config SM Control */
	eMMW_HR_MB, /**< Modbus Control Register */
	eMMW_HR_WDT, /**< MB WDT Period Register */
	eMMW_HR_PW, /**< Password Set Register */
	eMMW_HR_NUM /**< Number of holding registers */
} mmw_hr_t;

extern uint8_t PETITMODBUS_DLY_TOP;
extern uint8_t PETITMODBUS_SLAVE_ADDRESS;

void mmw_init(uint8_t sid, uint8_t baud);

// group Modbus_Middleware
/** @} */

#endif /* INC_MODBUSMIDDLEWARE_H_ */
