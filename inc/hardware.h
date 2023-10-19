/******************************************************************************
 * @file pins.h
 * This file contains function-like macros that aim to provide device hardware
 * pinout data.
 *
 * This project is meant to be developed in conjunction with an
 * affordable USB logic analyer.  Much of the timing data is validated using
 * a logic analyzer since timing calculations should not be trusted with
 * "new to you" hardware.  The timing information should both be
 * calculated with either an autoconfig tool or pen and paper and then
 * validated using other means such as a logic analyzer or USB UART.
 *
 * Even in debug mode, many of these macros are blank because they were not
 * used at the time.  They were strategically placed and should remain in the
 * code until they are no longer needed.  The definitions in this file can be
 * changed to suit the target of the debugging effort.  New definitions can be
 * added in this file to help future developers.
 *
 * The fully integrated PCB does not use debug pins for timing data since the
 * timing should have already been developed in the low-cost kit board.
 * @date Jul 18, 2023
 * @author brandon
 *****************************************************************************/

#ifndef INC_HARDWARE_H_
#define INC_HARDWARE_H_

#if defined(PCB)
// PORT 0
// P0_B0 NC
// P0_B1 NC
#define nLED (P0_B2) // reset LED, present since initial development as "LED"
#define XCVR_TX (P0_B3)
// P0_B4 UART TX
// P0_B5 UART RX
#define nPWR_LED (P0_B6)
// P0_B7 comparator output (hardware)

// PORT 1
#define nMB_LED (P1_B0)
#define RESET_P (P1_B1)
#define nWDT_LED (P1_B2)
// P1_B3 is taken by the comparator input

#define VIN_CMP_CPOUT() (CMP1CN0 & CMP1CN0_CPOUT__BMASK)

#else // not PCB
#define RESET_P (P1_B1)
#define nLED (P1_B4)
#define XCVR_TX (P0_B3)

#define VIN_CMP_CPOUT() (CMP0CN0 & CMP0CN0_CPOUT__BMASK)
#endif // PCB

#if defined(DEBUG) && DEBUG
// debug mode is on, toggle the debug pins
// do not use the pcb with debug mode!
#if defined(PCB)
#error "Do not use debug mode with the PCB!"
#endif // PCB
#define DBG0 (P0_B6)
#define DBG1 (P1_B0)
#define DBG2 (P1_B2)
#define DBG3 (P1_B3)
#define TIMER0_PINO_ON()
#define TIMER0_PINO_OFF()
#define TIMER0_PINI_ON() DBG0 = 1;
#define TIMER0_PINI_OFF() DBG0 = 0;
#define TIMER1_PIN_ON() DBG1 = 1;
#define TIMER1_PIN_OFF() DBG1 = 0;
#define UART0_PIN_ON() DBG2 = 1;
#define UART0_PIN_OFF() DBG2 = 0;
#define ADC0_PIN_ON()
#define ADC0_PIN_OFF()
#define MAIN_PIN_ON()
#define MAIN_PIN_OFF()
#define PETIT_PROCESS_ON() DBG3 = 1;
#define PETIT_PROCESS_OFF() DBG3 = 0;
// note that these respond to inverse logic
#define RESET_LED_SET(nSTATE)
#define WDT_LED_SET(nSTATE) nLED = (nSTATE);
#else
// empty declarations
#define TIMER0_PINO_ON()
#define TIMER0_PINO_OFF()
#define TIMER0_PINI_ON()
#define TIMER0_PINI_OFF()
#define TIMER1_PIN_ON()
#define TIMER1_PIN_OFF()
#define UART0_PIN_ON()
#define UART0_PIN_OFF()
#define ADC0_PIN_ON()
#define ADC0_PIN_OFF()
#define MAIN_PIN_ON()
#define MAIN_PIN_OFF()
#define PETIT_PROCESS_ON()
#define PETIT_PROCESS_OFF()
#define RESET_LED_SET(nSTATE) nLED = (nSTATE);
#define WDT_LED_SET(nSTATE) nWDT_LED = (nSTATE);
#endif

#endif /* INC_HARDWARE_H_ */
