/*
 * debugpins.h
 *
 *  Created on: Jul 18, 2023
 *      Author: brandon
 */

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
