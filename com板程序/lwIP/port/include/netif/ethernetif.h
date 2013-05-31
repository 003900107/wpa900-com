#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

//extern sys_sem_t tx_sem;
extern sys_sem_t rx_sem;

err_t ethernetif_1_init(struct netif *netif);
err_t ethernetif_2_init(struct netif *netif);
//err_t ethernetif_input(struct netif *netif);
struct netif *ethernetif_register(void);
int ethernetif_poll(void);
void Set_MAC_Address(unsigned char* macadd_1,unsigned char* macadd_2);
void Rx_Thread(void *arg);
//err_t  TaskTest (struct netif *netif);
err_t  ethernetif_1_input (struct netif *netif);
err_t  ethernetif_2_input (struct netif *netif);

#ifdef SERVER

#define MAC_ADDR0 0x00
#define MAC_ADDR1 0x00
#define MAC_ADDR2 0x00
#define MAC_ADDR3 0x00
#define MAC_ADDR4 0x00
#define MAC_ADDR5 0x01

#else

#define MAC_ADDR0 0x00
#define MAC_ADDR1 0x00
#define MAC_ADDR2 0x00
#define MAC_ADDR3 0x00
#define MAC_ADDR4 0x00
//#define MAC_ADDR5 0x02
#define MAC_ADDR5 0x03
//#define MAC_ADDR5 0x04

#endif

#endif 
