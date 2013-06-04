/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp_oe.h"
#include "err.h"
#include "ethernetif.h"

#include "includes.h"
#include "stm32_eth.h"
#include <string.h>

#include "pbuf.h"
#include "enc28j60.h"
#include "ucos_ii.h"

/* TCP and ARP timeouts */
volatile int tcp_end_time, arp_end_time;

/* Define those to better describe your network interface. */
#define IFNAME0 's'
#define IFNAME1 't'

#define  ETH_DMARxDesc_FrameLengthShift           16
#define  ETH_ERROR              ((u32)0)
#define  ETH_SUCCESS            ((u32)1)

//static  OS_STK   App_Task_Ethernetif_Input_Stk[512];

extern OS_EVENT *Eth1_pkt_Sem;
extern OS_EVENT *Eth2_pkt_Sem;

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif
{
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
  int unused;
};

/* Forward declarations. */
err_t  ethernetif_2_input(struct netif *netif);
//err_t  TaskTest (struct netif *netif);  
err_t  ethernetif_1_input (struct netif *netif);

#define ETH_RXBUFNB        4    // DMA接收缓冲区数目
#define ETH_TXBUFNB        2    // DMA发送缓冲区数目

uint8_t MACaddr_1[6];
uint8_t MACaddr_2[6];
ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB], DMATxDscrTab[ETH_TXBUFNB];/* Ethernet Rx & Tx DMA Descriptors */
uint8_t Rx_Buff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE], Tx_Buff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE];/* Ethernet buffers */

ETH_DMADESCTypeDef  *DMATxDesc = DMATxDscrTab;
extern ETH_DMADESCTypeDef  *DMATxDescToSet;
extern ETH_DMADESCTypeDef  *DMARxDescToGet;

//extern struct netif netif;
//sys_sem_t tx_sem;
//sys_sem_t rx_sem;

typedef struct{
u32 length;
u32 buffer;
ETH_DMADESCTypeDef *descriptor;
}FrameTypeDef;

FrameTypeDef ETH_RxPkt_ChainMode(void);
u32 ETH_GetCurrentTxBuffer(void);
u32 ETH_TxPkt_ChainMode(u16 FrameLength);
void LwIP_Pkt_Handle(void); /* Ethernet ISR */

/**
 * Setting the MAC address.
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
void Set_MAC_Address(uint8_t* macadd_1,uint8_t* macadd_2)
{
  MACaddr_1[0] = macadd_1[0];
  MACaddr_1[1] = macadd_1[1];
  MACaddr_1[2] = macadd_1[2];
  MACaddr_1[3] = macadd_1[3];
  MACaddr_1[4] = macadd_1[4];
  MACaddr_1[5] = macadd_1[5];
  
  ETH_MACAddressConfig(ETH_MAC_Address0, macadd_1);  
  
  MACaddr_2[0] = macadd_2[0];
  MACaddr_2[1] = macadd_2[1];
  MACaddr_2[2] = macadd_2[2];
  MACaddr_2[3] = macadd_2[3];
  MACaddr_2[4] = macadd_2[4];
  MACaddr_2[5] = macadd_2[5];
}


/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_1_init(struct netif *netif)
{
  SYS_ARCH_DECL_PROTECT(sr);
    
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] =  MACaddr_1[0];
  netif->hwaddr[1] =  MACaddr_1[1];
  netif->hwaddr[2] =  MACaddr_1[2];
  netif->hwaddr[3] =  MACaddr_1[3];
  netif->hwaddr[4] =  MACaddr_1[4];
  netif->hwaddr[5] =  MACaddr_1[5];

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

  SYS_ARCH_PROTECT(sr);
  
  /* Initialize Tx Descriptors list: Chain Mode */
  ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
  /* Initialize Rx Descriptors list: Chain Mode  */
  ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

/*----------added by Wang Mengyin 2010-07-20----------------------------------*/  
  // install Ethernet Interrupt Handlers
  // enable Ethernet Interrupt in NVIC and set up ISR
  BSP_IntVectSet(BSP_INT_ID_ETH, LwIP_Pkt_Handle);
  BSP_IntEn(BSP_INT_ID_ETH);
  
  /* Enable Ethernet Rx interrrupt */
  { int i;
    for(i=0; i<ETH_RXBUFNB; i++)
    {
      ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
    }
  }

#ifdef CHECKSUM_BY_HARDWARE
  /* Enable the checksum insertion for the Tx frames */
  { int i;
    for(i=0; i<ETH_TXBUFNB; i++)
    {
      ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
    }
  }
#endif

  SYS_ARCH_UNPROTECT(sr);
  
  /* Enable MAC and DMA transmission and reception */
  ETH_Start();

}

static void
low_level_2_init(struct netif *netif)
{
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] =  MACaddr_2[0];
  netif->hwaddr[1] =  MACaddr_2[1];
  netif->hwaddr[2] =  MACaddr_2[2];
  netif->hwaddr[3] =  MACaddr_2[3];
  netif->hwaddr[4] =  MACaddr_2[4];
  netif->hwaddr[5] =  MACaddr_2[5];

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_1_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  int l = 0;
  u8 *buffer = NULL;

  SYS_ARCH_DECL_PROTECT(sr);
  /* Interrupts are disabled through this whole thing to support multi-threading
	   transmit calls. Also this function might be called from an ISR. */
  SYS_ARCH_PROTECT(sr);

  buffer = (u8 *)ETH_GetCurrentTxBuffer();
  
  for(q = p; q != NULL; q = q->next) 
  {
    memcpy((u8_t*)&buffer[l], q->payload, q->len);
	l = l + q->len;
  }

  ETH_TxPkt_ChainMode(l);

  SYS_ARCH_UNPROTECT(sr);

  return ERR_OK;
}

static err_t
low_level_2_output(struct netif *netif, struct pbuf *p)
{
  SYS_ARCH_DECL_PROTECT(sr);
  /* Interrupts are disabled through this whole thing to support multi-threading
	   transmit calls. Also this function might be called from an ISR. */
  SYS_ARCH_PROTECT(sr);

  enc28j60PacketSend(p);

  SYS_ARCH_UNPROTECT(sr);

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_1_input(struct netif *netif)
{
  struct pbuf *p, *q;
  u16_t len;
  int l =0;
  FrameTypeDef frame;
  u8 *buffer;
  
  p = NULL;
  frame = ETH_RxPkt_ChainMode();
  /* Obtain the size of the packet and put it into the "len"
     variable. */
  len = frame.length;
  
  buffer = (u8 *)frame.buffer;

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

  if (p != NULL)
  {
    for (q = p; q != NULL; q = q->next)
    {
	  memcpy((u8_t*)q->payload, (u8_t*)&buffer[l], q->len);
      l = l + q->len;
    }    
  }


  /* Set Own bit of the Rx descriptor Status: gives the buffer back to ETHERNET DMA */
  frame.descriptor->Status = ETH_DMARxDesc_OWN; 
 
  /* When Rx Buffer unavailable flag is set: clear it and resume reception */
  if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)  
  {
    /* Clear RBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_RBUS;
    /* Resume DMA reception */
    ETH->DMARPDR = 0;
  }


  return p;
}

static struct pbuf *
low_level_2_input(struct netif *netif)
{
  struct pbuf *p;
  
  SYS_ARCH_DECL_PROTECT(sr);
  SYS_ARCH_PROTECT(sr);
  
  p = enc28j60PacketReceive();
  
  SYS_ARCH_UNPROTECT(sr);
  
  return p;
}



/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
err_t  ethernetif_1_input (struct netif *netif)
{
  //(void)p_arg;
  //INT8U     err;
  struct pbuf *p;
  extern struct netif netif_1;
  
  netif = &netif_1;
  
  while(1)
  {
//    OSSemPend(Eth1_pkt_Sem,0,&err);
//    printf("\n\n\r网口1数据包接收信号量");
    //ethernetif_1_input(netif);
    if(ETH_GetRxPktSize() != 0)
    {
      //ethernetif_1_input(netif);
      SYS_ARCH_DECL_PROTECT(sr);
  
      SYS_ARCH_PROTECT(sr);
      /* move received packet into a new pbuf */
      p = low_level_1_input(netif);
      SYS_ARCH_UNPROTECT(sr);
      
      if (p != NULL)
      {
        err_t err;
        err = netif->input(p, netif); // 将pbuf传递给上层协议栈
        if (err != ERR_OK)
        {
          LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
          pbuf_free(p);
          p = NULL;
        }
      }
    }
    else
    {
      OSTimeDlyHMSM(0, 0, 0, 5);
    }
  }
}

err_t
ethernetif_2_input(struct netif *netif)
{
  //INT8U Eth2_sem_err;
  struct pbuf *p;
  
  extern struct netif netif_2;
  
  netif = &netif_2;
  
  while(1)
  {
    //OSSemPend(Eth2_pkt_Sem,0,&Eth2_sem_err);
    //printf("\n\n\r网口2接收数据包");
    /* move received packet into a new pbuf */
    p = low_level_2_input(netif);
    
    if (p != NULL)
    {
      //printf("\n\n\r ethernetif_2_input !");
      
      err_t err;
      err = netif->input(p, netif); // 将pbuf传递给上层协议栈
      if (err != ERR_OK)
      {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
        pbuf_free(p);
        p = NULL;
      }
    }
    else
    {
      OSTimeDlyHMSM(0, 0, 0, 5);
    }
  }
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_1_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));

  ethernetif = mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_1_output;

  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  /* initialize the hardware */
  low_level_1_init(netif);

  return ERR_OK;
}

err_t
ethernetif_2_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));

  ethernetif = mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

  netif->state = ethernetif;
  netif->name[0] = 'a';  //IFNAME0;
  netif->name[1] = 'b';  //IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_2_output;

  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  /* initialize the hardware */
  low_level_2_init(netif);

  return ERR_OK;
}

/*******************************************************************************
* Function Name  : ETH_RxPkt_ChainMode
* Description    : Receives a packet.
* Input          : None
* Output         : None
* Return         : frame: farme size and location
*******************************************************************************/
FrameTypeDef ETH_RxPkt_ChainMode(void)
{ 
  u32 framelength = 0;
  FrameTypeDef frame = {0,0}; 

  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (u32)RESET)
  {	
	frame.length = ETH_ERROR;

    if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)  
    {
      /* Clear RBUS ETHERNET DMA flag */
      ETH->DMASR = ETH_DMASR_RBUS;
      /* Resume DMA reception */
      ETH->DMARPDR = 0;
    }

	/* Return error: OWN bit set */
    return frame; 
  }
  
  if(((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (u32)RESET) && 
     ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (u32)RESET) &&  
     ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (u32)RESET))  
  {      
    /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
    framelength = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4;
	
	/* Get the addrees of the actual buffer */
	frame.buffer = DMARxDescToGet->Buffer1Addr;	
  }
  else
  {
    /* Return ERROR */
    framelength = ETH_ERROR;
  }

  frame.length = framelength;


  frame.descriptor = DMARxDescToGet;
  
  /* Update the ETHERNET DMA global Rx descriptor with next Rx decriptor */      
  /* Chained Mode */    
  /* Selects the next DMA Rx descriptor list for next buffer to read */ 
  DMARxDescToGet = (ETH_DMADESCTypeDef*) (DMARxDescToGet->Buffer2NextDescAddr);    
  
  /* Return Frame */
  return (frame);  
}

/*******************************************************************************
* Function Name  : ETH_TxPkt_ChainMode
* Description    : Transmits a packet, from application buffer, pointed by ppkt.
* Input          : - FrameLength: Tx Packet size.
* Output         : None
* Return         : ETH_ERROR: in case of Tx desc owned by DMA
*                  ETH_SUCCESS: for correct transmission
*******************************************************************************/
u32 ETH_TxPkt_ChainMode(u16 FrameLength)
{   
  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (u32)RESET)
  {  
	/* Return ERROR: OWN bit set */
    return ETH_ERROR;
  }
        
  /* Setting the Frame Length: bits[12:0] */
  DMATxDescToSet->ControlBufferSize = (FrameLength & ETH_DMATxDesc_TBS1);

  /* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */    
  DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

  /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
  DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

  /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
  if ((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET)
  {
    /* Clear TBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_TBUS;
    /* Resume DMA transmission*/
    ETH->DMATPDR = 0;
  }
  
  /* Update the ETHERNET DMA global Tx descriptor with next Tx decriptor */  
  /* Chained Mode */
  /* Selects the next DMA Tx descriptor list for next buffer to send */ 
  DMATxDescToSet = (ETH_DMADESCTypeDef*) (DMATxDescToSet->Buffer2NextDescAddr);    


  /* Return SUCCESS */
  return ETH_SUCCESS;   
}


/*******************************************************************************
* Function Name  : ETH_GetCurrentTxBuffer
* Description    : Return the address of the buffer pointed by the current descritor.
* Input          : None
* Output         : None
* Return         : Buffer address
*******************************************************************************/
u32 ETH_GetCurrentTxBuffer(void)
{ 
  /* Return Buffer address */
  return (DMATxDescToSet->Buffer1Addr);   
}
