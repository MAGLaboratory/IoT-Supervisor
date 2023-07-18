/*
 * debugpins.h
 *
 *  Created on: Jul 18, 2023
 *      Author: brandon
 */

#ifndef INC_DEBUGPINS_H_
#define INC_DEBUGPINS_H_

#if DEBUG
// debug mode is on, toggle the debug pins
#define TIMER0_PIN_ON() P0_B6 = 1;
#define TIMER0_PIN_OFF() P0_B6 = 0;
#define TIMER1_PIN_ON() P1_B0 = 1;
#define TIMER1_PIN_OFF() P1_B0 = 0;
#define UART0_PIN_ON() P1_B2 = 1;
#define UART0_PIN_OFF() P1_B2 = 0;
#define ADC0_PIN_ON()
#define ADC0_PIN_OFF()
#else
// empty declarations
#define TIMER0_PIN_ON()
#define TIMER0_PIN_OFF()
#define TIMER1_PIN_ON()
#define TIMER1_PIN_OFF()
#define UART0_PIN_ON()
#define UART0_PIN_OFF()
#define ADC0_PIN_ON()
#define ADC0_PIN_OFF()
#endif

#endif /* INC_DEBUGPINS_H_ */
