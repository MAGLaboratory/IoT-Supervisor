/******************************************************************************
 * @file debugpins.h
 * This file contains function-like macros that aim to help debug the
 * application code through applicable timing data.
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
 * @date Jul 18, 2023
 * @author brandon
 *****************************************************************************/

#ifndef INC_DEBUGPINS_H_
#define INC_DEBUGPINS_H_

#if defined(DEBUG) && DEBUG
// debug mode is on, toggle the debug pins
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
#endif

#endif /* INC_DEBUGPINS_H_ */
