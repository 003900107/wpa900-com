#ifndef __MODBUS_TCP_MASTER_H
#define __MODBUS_TCP_MASTER_H

//功能码请求帧组织
INT16U tcp_getCoilStatus(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth);
INT16U tcp_getInputStatus(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth);
INT16U  tcp_getHoldingRegisters(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth);
INT16U tcp_getAnalogInput(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth);
INT16U  tcp_setCoil(INT8U addr,INT8U *buf,INT16U start_address,INT16U status);
INT16U tcp_setHoldingRegister(INT8U addr,INT8U *buf, INT16U start_address,INT16U value );

#endif