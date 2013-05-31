/**
* @file lwIP.c
* @brief A brief file description.
* @details
*     A more elaborated file description.
* @author Wang Mengyin
* @date 2010Jul19 15:53:34
* @note
*               Copyright 2010 Wang Mengyin.ALL RIGHTS RESERVED.
*                            http://tigerwang202.blogbus.com
*    This software is provided under license and contains proprietary and
* confidential material which is the property of Company Name tech.
*/


#define __LW_IP_C


/* Includes ------------------------------------------------------------------*/
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

//#include "helloworld.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAX_DHCP_TRIES        4
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct netif netif_1;
struct netif netif_2;
static uint32_t IPaddress = 0;

OS_EVENT *Eth1_pkt_Sem;
OS_EVENT *Eth2_pkt_Sem;
static   OS_STK      App_Eth_1_Stk[512];
static   OS_STK      App_Eth_2_Stk[512];

/* Private function prototypes -----------------------------------------------*/
static void TcpipInitDone(void *arg);
static void list_if(void);

//static  void  TaskTest (void *p_arg);

/* Private functions ---------------------------------------------------------*/

/** 
 * @brief TcpipInitDone wait for tcpip init being done
 * 
 * @param arg the semaphore to be signaled
 */
static void TcpipInitDone(void *arg)
{
    sys_sem_t *sem;
    sem = arg;
    sys_sem_signal(*sem);
}

/**
* @brief Init_lwIP initialize the LwIP
*/
void Init_lwIP(void)
{
  //网络接口1地址
    struct ip_addr ipaddr_1;
    struct ip_addr netmask_1;
    struct ip_addr gw_1;
    
    //网络接口2地址
    struct ip_addr ipaddr_2;
    struct ip_addr netmask_2;
    struct ip_addr gw_2;
    
    //uint8_t macaddress[6]={0,0,0,0,0,1};
    uint8_t macaddress_1[6]={0xcc,0xbb,0xaa,0x99,0x88,0x1};
    extern uint8_t macaddress_2[6];
    
    sys_sem_t sem;
    
    sys_init();
    
    /* Initializes the dynamic memory heap defined by MEM_SIZE.*/
    mem_init();
    
    /* Initializes the memory pools defined by MEMP_NUM_x.*/
    memp_init();
    
    pbuf_init();	
    netif_init();
    
    //printf("TCP/IP initializing...\n");
    sem = sys_sem_new(0);
    tcpip_init(TcpipInitDone, &sem);
    sys_sem_wait(sem);
    sys_sem_free(sem);
    //printf("TCP/IP initialized.\n");
    
//    rx_sem = sys_sem_new(0);                // create receive semaphore
    //tx_sem = sys_sem_new(0);                // create transmit semaphore
    
//    sys_thread_new("Rx Thread", Rx_Thread, &netif, LWIP_STK_SIZE, DrvRx_Thread_PRIO); //lwIP Task 1
//    sys_thread_new(Tx_Thread, NULL, DrvTx_Thread_PRIO); //lwIP Task 2
    //and lwIP Task 3 was used for TCPIP thread
      
#if LWIP_DHCP
    /* 启用DHCP服务器 */
    ipaddr_1.addr = 0;
    netmask_1.addr = 0;
    gw_1.addr = 0;
#else
    /* 启用静态IP */
    //网络接口1
    IP4_ADDR(&ipaddr_1, 192, 168, 1, 14);
    IP4_ADDR(&netmask_1, 255, 255, 255, 0);
    IP4_ADDR(&gw_1, 192, 168, 1, 1);
    
    //网络接口2
    //IP4_ADDR(&ipaddr_2, 192, 168, 1, 15);
    //IP4_ADDR(&netmask_2, 255, 255, 255, 0);
    //IP4_ADDR(&gw_2, 192, 168, 1, 1);
    IP4_ADDR(&ipaddr_2, 172, 18, 154, 100);
    IP4_ADDR(&netmask_2, 255, 255, 0, 0);
    IP4_ADDR(&gw_2, 172, 18, 154, 1);
#endif
    
    //设置网络接口1、2的MAC地址
    Set_MAC_Address(macaddress_1,macaddress_2);
    
    //添加网络接口1
    netif_add(&netif_1, &ipaddr_1, &netmask_1, &gw_1, NULL, &ethernetif_1_init, &tcpip_input);
    
    //添加网络接口2
    netif_add(&netif_2, &ipaddr_2, &netmask_2, &gw_2, NULL, &ethernetif_2_init, &tcpip_input);
    
    //设置缺省网络接口
    netif_set_default(&netif_1);
    
#if LWIP_DHCP
    dhcp_start(&netif_1);
#endif
    
    netif_set_up(&netif_1);
    netif_set_up(&netif_2);
    
    //网口1、2数据包接收信号量
    Eth1_pkt_Sem = OSSemCreate(0);
    Eth2_pkt_Sem = OSSemCreate(0);
    
    //建立数据包接收线程
    OSTaskCreate( (void (*)(void *)) ethernetif_1_input,				
	          (void          * ) 0,							
	          (OS_STK        * )&App_Eth_1_Stk[512 - 1],		
	          (INT8U           ) 5  );
    
    OSTaskCreate( (void (*)(void *)) ethernetif_2_input,				
	          (void          * ) 0,							
	          (OS_STK        * )&App_Eth_2_Stk[512 - 1],		
	          (INT8U           ) 6  );
}

//static  void  TaskTest (void *p_arg)
//{
//  (void)p_arg;
//  INT8U     err;
  
//  while(1)
//  {
    //ethernetif_1_input(&netif_1);
    //OSSemPend(Eth1_pkt_Sem,100,&err);
    //printf("\n\n\r网口1数据包接收信号量");
//    if(ETH_GetRxPktSize() != 0)
//    {
//      ethernetif_1_input(&netif_1);
//    }
//    else
//    {
//      OSTimeDlyHMSM(0, 0, 0, 5);
//    }
//  }
//}

/** 
 * @brief Display_IPAddress Display IP Address
 * 
 */
void Display_IPAddress(void)
{
    if(IPaddress != netif_1.ip_addr.addr)
    {   /* IP 地址发生改变*/
        __IO uint8_t iptab[4];
        uint8_t iptxt[20];
        
        /* read the new IP address */
        IPaddress = netif_1.ip_addr.addr;
        
        iptab[0] = (uint8_t)(IPaddress >> 24);
        iptab[1] = (uint8_t)(IPaddress >> 16);
        iptab[2] = (uint8_t)(IPaddress >> 8);
        iptab[3] = (uint8_t)(IPaddress);
        
        sprintf((char*)iptxt, "   %d.%d.%d.%d    ", iptab[3], iptab[2], iptab[1], iptab[0]);
        
        list_if();
        
        /* Display the new IP address */
#if LWIP_DHCP
        if(netif_1.flags & NETIF_FLAG_DHCP)
        {   // IP地址由DHCP指定
            // Display the IP address
            LCD_DisplayStringLine(Line1, "  IP assigned   ");
            LCD_DisplayStringLine(Line2, "by DHCP server  ");
            LCD_DisplayStringLine(Line3, iptxt);
        }
        else
#endif  // 静态IP地址
        {   /* Display the IP address */
            //LCD_DisplayStringLine(Line1, " Static IP Addr ");
            //LCD_DisplayStringLine(Line2, iptxt);
            //LCD_DisplayStringLine(Line3, "                ");
            //LCD_DisplayStringLine(Line4, "                ");
            
//            LCD_Puts("0123456789abcdef" "123456789abcdef0" "23456789abcdef01" "3456789abcdef012");
        }
        
    }
#if LWIP_DHCP
    else if(IPaddress == 0)
    {   // 等待DHCP分配IP
        LCD_DisplayStringLine(Line1, " Looking for    ");
        LCD_DisplayStringLine(Line2, " DHCP server    ");
        LCD_DisplayStringLine(Line3, " please wait... ");
        LCD_DisplayStringLine(Line4, "                ");
        
        //printf("DHCP tries %d\n",netif_1.dhcp->tries);
        
        /* If no response from a DHCP server for MAX_DHCP_TRIES times */
        /* stop the dhcp client and set a static IP address */
        if(netif_1.dhcp->tries > MAX_DHCP_TRIES) 
        {   /* 超出DHCP重试次数，改用静态IP设置 */
            struct ip_addr ipaddr_1;
            struct ip_addr netmask_1;
            struct ip_addr gw_1;
            
            LCD_DisplayStringLine(Line4, " DHCP timeout   ");
            dhcp_stop(&netif_1);
            
            IP4_ADDR(&ipaddr_1, 192, 168, 1, 6);
            IP4_ADDR(&netmask_1, 255, 255, 255, 0);
            IP4_ADDR(&gw_1, 192, 168, 1, 1);
            
            netif_set_addr(&netif_1, &ipaddr_1 , &netmask_1, &gw_1);
            
            list_if();
        }
    }
#endif
}

/**
* @brief display ip address in serial port debug windows
*/
static void list_if()
{
    //printf("Default network interface: %c%c\n", netif.name[0], netif.name[1]);
    //printf("ip address: %s\n", inet_ntoa(*((struct in_addr*)&(netif.ip_addr))));
    //printf("gw address: %s\n", inet_ntoa(*((struct in_addr*)&(netif.gw))));
    //printf("net mask  : %s\n", inet_ntoa(*((struct in_addr*)&(netif.netmask))));
    
}

/*---------Ethernte ISR Added by Wang 2010-07-20------------------------------*/
/**
  * @brief  Ethernet ISR
  * @param  None
  * @retval None
  */
void LwIP_Pkt_Handle(void)
{
    //printf("\npkt interrupt\n\r");
    //while(ETH_GetRxPktSize() != 0)
    //{
      //printf("数据包长度：%d\n\r",ETH_GetRxPktSize());
        //ethernetif_1_input(&netif_1);
        
        //测试
        //OSSemPost(Eth1_pkt_Sem);
        //printf("\n\n\r发送信号量");
    //}
    
    /* Clear Rx Pending Bit */
    ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
    /* Clear the Eth DMA Rx IT pending bits */
    ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
    
}

void  BSP_IntHandlerEXTI1         (void)
{
  //printf("\n\n\r网口2中断");
  //OSSemPost(Eth2_pkt_Sem);
}
/*-- File end --*/

