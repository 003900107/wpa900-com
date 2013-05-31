


#ifndef  __APP_CLIENT_H__
#define  __APP_CLIENT_H__

#define MODBUS_TCP_INTERVAL_S  1
#define MODBUS_TCP_INTERVAL_MS 100

void  AppTaskClient (void *p_arg);
void  AppTaskClientReconnect (void *p_arg);
void  command_execute(uint16_t id,uint16_t startaddr,uint16_t state);

#endif