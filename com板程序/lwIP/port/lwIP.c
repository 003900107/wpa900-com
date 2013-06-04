/*
*********************************************************************************************************
*
*                                            COM BOARD
*
*                                            STM32F107
*                                            UCOS V2.92
*                                            LWIP V1.3.1
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : yyj
*********************************************************************************************************
*/


#define __LW_IP_C


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "lwip/memp.h"
#include "lwIP.h"
#include "lwIP/tcp.h"
#include "lwIP/udp.h"
#include "lwIP/tcpip.h"
#include "netif/etharp.h"
#include "lwIP/dhcp.h"
#include "ethernetif.h"
#include "stm32f10x.h"
#include "arch/sys_arch.h"
#include <stdio.h>
#include "stm3210c_eval_lcd.h"
#include "stm32_eth.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define MAX_DHCP_TRIES        4


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

struct netif netif_1;
struct netif netif_2;

static   OS_STK      App_Eth_1_Stk[512];
static   OS_STK      App_Eth_2_Stk[512];


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void TcpipInitDone(void *arg);


/*
*********************************************************************************************************
*                                               TcpipInitDone()
*
* Description : Called by tcpip_init().
*
* Argument(s) : arg    semphore passed to this function.
*
* Return(s)   : none.
*
*********************************************************************************************************
*/

static void TcpipInitDone(void *arg)
{
    sys_sem_t *sem;
    sem = arg;
    sys_sem_signal(*sem);
}


/*
*********************************************************************************************************
*                                               TcpipInitDone()
*
* Description : This function initialize LWIP stack.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
*********************************************************************************************************
*/
void lwIP_Init(void)
{
    /* ETH_1 address */
    struct ip_addr ipaddr_1;
    struct ip_addr netmask_1;
    struct ip_addr gw_1;
    
    /* ETH_2 address */
    struct ip_addr ipaddr_2;
    struct ip_addr netmask_2;
    struct ip_addr gw_2;
    
    /* MAC address of ETH_1 and ETH_2 */
    uint8_t macaddress_1[6]={0xcc,0xbb,0xaa,0x99,0x88,0x1};
    extern uint8_t macaddress_2[6];
    
    /* Configure memmory and timeout structure for lwip */
    sys_init();
    
    /* Initializes the dynamic memory heap defined by MEM_SIZE.*/
    mem_init();
    
    /* Initializes the memory pools defined by MEMP_NUM_x.*/
    memp_init();
    
    /* Initializes pbuf */
    pbuf_init();	
    
    /* Initializes netif */
    netif_init();
    
    /* Create tcpip_thread */
    sys_sem_t sem;
    sem = sys_sem_new(0);
    tcpip_init(TcpipInitDone, &sem);
    sys_sem_wait(sem);
    sys_sem_free(sem);
    
#if LWIP_DHCP
    /* Clear address */
    ipaddr_1.addr = 0;
    netmask_1.addr = 0;
    gw_1.addr = 0;
#else
    /* Set static address  for ETH_1 , ETH_2*/
    IP4_ADDR(&ipaddr_1, 192, 168, 1, 14);
    IP4_ADDR(&netmask_1, 255, 255, 255, 0);
    IP4_ADDR(&gw_1, 192, 168, 1, 1);
    
    //IP4_ADDR(&ipaddr_2, 192, 168, 1, 15);
    //IP4_ADDR(&netmask_2, 255, 255, 255, 0);
    //IP4_ADDR(&gw_2, 192, 168, 1, 1);
    IP4_ADDR(&ipaddr_2, 172, 18, 154, 100);
    IP4_ADDR(&netmask_2, 255, 255, 0, 0);
    IP4_ADDR(&gw_2, 172, 18, 154, 1);
#endif
    
    /* Set MAC address */
    Set_MAC_Address(macaddress_1,macaddress_2);
    
    /* Add network interfaces */
    netif_add(&netif_1, &ipaddr_1, &netmask_1, &gw_1, NULL, &ethernetif_1_init, &tcpip_input);
    netif_add(&netif_2, &ipaddr_2, &netmask_2, &gw_2, NULL, &ethernetif_2_init, &tcpip_input);
    
    /* Set default netif */
    netif_set_default(&netif_1);
    
#if LWIP_DHCP
    dhcp_start(&netif_1);
#endif
    
    /* Bring up netif */
    netif_set_up(&netif_1);
    netif_set_up(&netif_2);
    
    /* Create tasks for polling network packets */
    OSTaskCreate( (void (*)(void *)) ethernetif_1_input,				
	          (void          * ) 0,							
	          (OS_STK        * )&App_Eth_1_Stk[512 - 1],		
	          (INT8U           ) 5  );
    
    OSTaskCreate( (void (*)(void *)) ethernetif_2_input,				
	          (void          * ) 0,							
	          (OS_STK        * )&App_Eth_2_Stk[512 - 1],		
	          (INT8U           ) 6  );
}


/*
*********************************************************************************************************
*                                               LwIP_Pkt_Handle()
*
* Description : Interrupt handler function of netif_1.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
*********************************************************************************************************
*/

void LwIP_Pkt_Handle(void)
{
    //while(ETH_GetRxPktSize() != 0)
    //{
        //ethernetif_1_input(&netif_1);
    //}
    
    /* Clear Rx Pending Bit */
    ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
    /* Clear the Eth DMA Rx IT pending bits */
    ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
    
}


/*
*********************************************************************************************************
*                                               BSP_IntHandlerEXTI1()
*
* Description : Interrupt handler function of netif_2.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
*********************************************************************************************************
*/

void  BSP_IntHandlerEXTI1         (void)
{
  EXTI_ClearITPendingBit(EXTI_Line1);
}
/*-- File end --*/

