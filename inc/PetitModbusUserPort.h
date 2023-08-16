/*******************************************************************************
 * PetitModbusUserPort.h
 *
 *  Edit this file to change some of the internal PetitModbus functionality.
 *******************************************************************************
 */

#ifndef INC_PETITMODBUSUSERPORT_H_
#define INC_PETITMODBUSUSERPORT_H_

// Petit Modbus RTU Slave Output Register Number
// Have to put a number of registers here
// It has to be bigger than 0 (zero)!!
#define NUMBER_OF_OUTPUT_PETITREGISTERS                 ( 3 )
#define NUMBER_OF_REGISTERS_IN_BUFFER                   ( 3 )
// Cycles to delay TX once the CRC finishes calculation
#define PETITMODBUS_DLY_TOP                             ( 0 )

#define PETITMODBUS_READ_HOLDING_REGISTERS_ENABLED      ( 1 )
#define PETITMODBUSWRITE_SINGLE_REGISTER_ENABLED        ( 1 )
#define PETITMODBUS_WRITE_MULTIPLE_REGISTERS_ENABLED    ( 1 )

// Where to process our modbus message
// 0 for processing in its own cycle
// 1 for processing in the same cycle as TX CRC calculation
#define PETITMODBUS_PROCESS_POSITION                    ( 1 )

// how to process the CRC
// PETIT_CRC_TABULAR takes up code space but is the fastest.
//     this should probably be your default choice
// PETIT_CRC_BITWISE takes up cycles but is space efficient
// PETIT_CRC_EXTERNAL requires you to define PetitPortCRC16Calc.
//     it is possible to use hardware CRC calculation with this.
#define PETIT_CRC PETIT_CRC_TABULAR

#define PETIT_REG PETIT_REG_INTERNAL

// define this to let the CRC table reside in code memory rather than RAM
#define PETIT_CODE code

#endif /* INC_PETITMODBUSUSERPORT_H_ */
