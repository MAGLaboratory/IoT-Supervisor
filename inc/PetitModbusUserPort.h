/*
 * PetitModbusUserPort.h
 *
 *  Created on: Jul 21, 2023
 *      Author: brandon
 */

#ifndef INC_PETITMODBUSUSERPORT_H_
#define INC_PETITMODBUSUSERPORT_H_

#define NUMBER_OF_OUTPUT_PETITREGISTERS           ( 3 )                      // Petit Modbus RTU Slave Output Register Number
                                                                             // Have to put a number of registers here
                                                                             // It has to be bigger than 0 (zero)!!
#define PETITMODBUS_DLY_TOP                       ( 0 )                      // Timeout Constant for Petit Modbus RTU Slave [millisecond]

#define PETITMODBUS_READ_HOLDING_REGISTERS_ENABLED      ( 1 )                   // If you want to use make it 1, or 0
#define PETITMODBUSWRITE_SINGLE_REGISTER_ENABLED        ( 1 )                   // If you want to use make it 1, or 0
#define PETITMODBUS_WRITE_MULTIPLE_REGISTERS_ENABLED    ( 1 )                   // If you want to use make it 1, or 0
#define PETITMODBUS_PROCESS_POSITION                            ( 1 )

#define PETIT_CRC_TABULAR (0x4040)
#define PETIT_CRC_BITWISE (0x2020)

#define PETIT_CRC PETIT_CRC_BITWISE

// define this to let the CRC table reside in code memory rather than RAM
#define PETIT_CODE code

#endif /* INC_PETITMODBUSUSERPORT_H_ */
