//=========================================================
// PetitModbusPort.h
// This is a file to support porting PetitModbus
//=========================================================
#ifndef __PETIT_MODBUS_PORT_H__
#define __PETIT_MODBUS_PORT_H__


#define FALSE (0)
#define TRUE (1)

extern unsigned int PetitRxCounter;
extern unsigned int PetitRxRemaining;
extern unsigned char* Petit_Rx_Ptr;

extern unsigned int Petit_Tx_Buf_Size;
extern unsigned char* Petit_Tx_Ptr;
extern PETIT_RXTX_STATE Petit_Tx_State;

#endif
