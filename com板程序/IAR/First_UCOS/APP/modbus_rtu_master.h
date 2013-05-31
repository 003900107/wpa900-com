

#ifndef __COM_MODBUS_MASTER_H__
#define __COM_MODBUS_MASTER_H__



#define   BIT0    0x0001
#define   BIT1    0x0002
#define   BIT2    0x0004
#define   BIT3    0x0008
#define   BIT4    0x0010
#define   BIT5    0x0020
#define   BIT6    0x0040
#define   BIT7    0x0080
#define   BIT8    0x0100
#define   BIT9    0x0200
#define   BITA    0x0400
#define   BITB    0x0800
#define   BITC    0x1000
#define   BITD    0x2000
#define   BITE    0x4000
#define   BITF    0x8000

INT16U rtu_getCoilStatus(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth);
INT16U rtu_getInputStatus(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth);
INT16U rtu_getHoldingRegisters(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth);
INT16U rtu_getAnalogInput(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth);
INT16U rtu_setCoil(INT8U addr,INT8U *buf,INT16U start_address,INT16U status);
INT16U rtu_setHoldingRegister(INT8U addr,INT8U *buf, INT16U start_address,INT16U value );


#endif //__COM_MODBUS_MASTER_H__