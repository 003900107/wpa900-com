/*
*********************************************************************************************************
*
*                                              MODBUS TCP
*
*                                               STM32F107
*                                             ucos-II v2.92
*                                              lwip v1.3.1
*
* Filename      : TcpClient.c
* Version       : V1.00
* Programmer(s) : yyj
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "includes.h"
#include <stdio.h>
#include "tcp.h"
#include "err.h"
#include "TcpClient.h"
#include "modbus_tcp_master.h"
#include "string.h"
#include "bsp.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define MAX_BUFF_SIZE 100


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/



/* modbus_tcp request/respond struct */
static uint8_t MD_request[20];
static uint8_t tx_len;

enum net_state
{
  NET_CLOSED = 0,
  NET_CONNECTED,
};

struct RxBuffer 
{
  int length;
  char bytes[MAX_BUFF_SIZE];
};

struct TcpClient_info
{
  //IP
  uint8_t IP_addr0;
  uint8_t IP_addr1;
  uint8_t IP_addr2;
  uint8_t IP_addr3;
  //IP
  uint32_t IPaddress;
  //port
  int localport;
  int remoteport;
  //pcb
  struct tcp_pcb *TCP_pcb;
  //state
  enum net_state state;
};

struct TcpClient_info TcpClient_1={172,18,154,50,0,501,502};
struct TcpClient_info TcpClient_2={172,18,154,52,0,502,502};
struct TcpClient_info TcpClient_3;
struct TcpClient_info TcpClient_4;
struct TcpClient_info TcpClient_5;
struct TcpClient_info TcpClient_6;
struct TcpClient_info TcpClient_7;
struct TcpClient_info TcpClient_8;
struct TcpClient_info TcpClient_9;
struct TcpClient_info TcpClient_10;
struct TcpClient_info TcpClient_11;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void tcpclient_connect(struct TcpClient_info *ClientInfo);
static err_t tcp_connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err);
static err_t tcp_recv_callback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
static void tcp_conn_err(void *arg, err_t err);


/*
*********************************************************************************************************
*                                             tcpclient_connect()
*
* Description : This function connects remote TCP server with given TcpClient_info and register callback
                function of tcp_connect, tcp_recv, tcp_err.
*
* Argument(s) : struct TcpClient_info *ClientInfo    Tcp client struct.
*
* Return(s)   : none.
*********************************************************************************************************
*/

static void tcpclient_connect(struct TcpClient_info *ClientInfo)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb;

  pcb = tcp_new();
  
  IP4_ADDR( &DestIPaddr, ClientInfo->IP_addr0, ClientInfo->IP_addr1, ClientInfo->IP_addr2, ClientInfo->IP_addr3 );
  
  ClientInfo->IPaddress = (uint32_t)ClientInfo->IP_addr0 + (((uint32_t)ClientInfo->IP_addr1)<<8)
    + (((uint32_t)ClientInfo->IP_addr2)<<16) + (((uint32_t)ClientInfo->IP_addr3)<<24);
  
  tcp_bind(pcb, IP_ADDR_ANY, ClientInfo->localport);
  
  tcp_arg(pcb, mem_calloc(sizeof(struct RxBuffer), 1));
  
  tcp_connect(pcb,&DestIPaddr,ClientInfo->remoteport,tcp_connect_callback);
  
  tcp_err(pcb, tcp_conn_err);
  
  tcp_recv(pcb,tcp_recv_callback);
}


/*
*********************************************************************************************************
*                                             tcp_connect_callback()
*
* Description : callback function of tcp_connect.
*
* Argument(s) : see tcp_connect.
*
* Return(s)   : ERR_OK.
*********************************************************************************************************
*/

static err_t tcp_connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  uint32_t IPaddress;
  
  IPaddress = tpcb->remote_ip.addr;
  
  if(TcpClient_1.IPaddress == IPaddress)
  {
    printf("测控1连接成功\n");
    TcpClient_1.TCP_pcb = tpcb;
    TcpClient_1.state = NET_CONNECTED;
  }
  else if(TcpClient_2.IPaddress == IPaddress)
  {
    printf("测控2连接成功\n");
    TcpClient_2.TCP_pcb = tpcb;
    TcpClient_2.state = NET_CONNECTED;
  }
  else if(TcpClient_3.IPaddress == IPaddress)
  {
    printf("测控3连接成功\n");
    TcpClient_3.TCP_pcb = tpcb;
    TcpClient_3.state = NET_CONNECTED;
  }
  else if(TcpClient_4.IPaddress == IPaddress)
  {
    printf("测控4连接成功\n");
    TcpClient_4.TCP_pcb = tpcb;
    TcpClient_4.state = NET_CONNECTED;
  }
  else if(TcpClient_5.IPaddress == IPaddress)
  {
    printf("测控5连接成功\n");
    TcpClient_5.TCP_pcb = tpcb;
    TcpClient_5.state = NET_CONNECTED;
  }
  else if(TcpClient_6.IPaddress == IPaddress)
  {
    printf("测控6连接成功\n");
    TcpClient_6.TCP_pcb = tpcb;
    TcpClient_6.state = NET_CONNECTED;
  }
  else if(TcpClient_7.IPaddress == IPaddress)
  {
    printf("测控7连接成功\n");
    TcpClient_7.TCP_pcb = tpcb;
    TcpClient_7.state = NET_CONNECTED;
  }
  else if(TcpClient_8.IPaddress == IPaddress)
  {
    printf("测控8连接成功\n");
    TcpClient_8.TCP_pcb = tpcb;
    TcpClient_8.state = NET_CONNECTED;
  }
  else if(TcpClient_9.IPaddress == IPaddress)
  {
    printf("测控9连接成功\n");
    TcpClient_9.TCP_pcb = tpcb;
    TcpClient_9.state = NET_CONNECTED;
  }
  else if(TcpClient_10.IPaddress == IPaddress)
  {
    printf("测控10连接成功\n");
    TcpClient_10.TCP_pcb = tpcb;
    TcpClient_10.state = NET_CONNECTED;
  }
  else if(TcpClient_11.IPaddress == IPaddress)
  {
    printf("测控11连接成功\n");
    TcpClient_11.TCP_pcb = tpcb;
    TcpClient_11.state = NET_CONNECTED;
  }
  
  return ERR_OK;
}


/*
*********************************************************************************************************
*                                             tcp_recv_callback()
*
* Description : callback function of tcp_recv.
*
* Argument(s) : see tcp_recv.
*
* Return(s)   : ERR_ARG         
                ERR_OK           
                tcp_close(pcb)
*********************************************************************************************************
*/

static err_t tcp_recv_callback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  struct RxBuffer *RxBuffer = (struct RxBuffer *)arg;
  uint8_t i,done;
  char *c;
  
  //判断连接
  uint32_t IPaddress;
  IPaddress = pcb->remote_ip.addr;
  
  /* We perform here any necessary processing on the pbuf */
  if (p != NULL) 
  {        
    /* We call this function to tell the LwIp that we have processed the data */
    /* This lets the stack advertise a larger window, so more data can be received*/
    tcp_recved(pcb, p->tot_len);

    /* Check the name if NULL, no data passed, return withh illegal argument error */
    
    if(!RxBuffer) 
    {
      pbuf_free(p);
      return ERR_ARG;
    }
    
    done = 0;
    
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len && !done ; i++) 
      {
        done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(4+c[5])));
        if(RxBuffer->length < MAX_BUFF_SIZE) 
	{
          RxBuffer->bytes[RxBuffer->length++] = c[i];
        }
      }
    }
    if(done) 
    {
      if(TcpClient_1.IPaddress == IPaddress)
      {
        printf("\n测控1");
      }
      else if(TcpClient_2.IPaddress == IPaddress)
      {
        printf("\n测控2");
      }
      else if(TcpClient_3.IPaddress == IPaddress)
      {
        printf("\n测控3");
      }
      else if(TcpClient_4.IPaddress == IPaddress)
      {
        printf("\n测控4");
      }
      else if(TcpClient_5.IPaddress == IPaddress)
      {
        printf("\n测控5");
      }
      else if(TcpClient_6.IPaddress == IPaddress)
      {
        printf("\n测控6");
      }
      else if(TcpClient_7.IPaddress == IPaddress)
      {
        printf("\n测控7");
      }
      else if(TcpClient_8.IPaddress == IPaddress)
      {
        printf("\n测控8");
      }
      else if(TcpClient_9.IPaddress == IPaddress)
      {
        printf("\n测控9");
      }
      else if(TcpClient_10.IPaddress == IPaddress)
      {
        printf("\n测控10");
      }
      else if(TcpClient_11.IPaddress == IPaddress)
      {
        printf("\n测控11");
      }
    }
    pbuf_free(p);
  }
  else if (err == ERR_OK) 
  {
    
    if(TcpClient_1.IPaddress == IPaddress)
    {
      TcpClient_1.state = NET_CLOSED;
    }
    else if(TcpClient_2.IPaddress == IPaddress)
    {
      TcpClient_2.state = NET_CLOSED;
    }
    else if(TcpClient_3.IPaddress == IPaddress)
    {
      TcpClient_3.state = NET_CLOSED;
    }
    else if(TcpClient_4.IPaddress == IPaddress)
    {
      TcpClient_4.state = NET_CLOSED;
    }
    else if(TcpClient_5.IPaddress == IPaddress)
    {
      TcpClient_5.state = NET_CLOSED;
    }
    else if(TcpClient_6.IPaddress == IPaddress)
    {
      TcpClient_6.state = NET_CLOSED;
    }
    else if(TcpClient_7.IPaddress == IPaddress)
    {
      TcpClient_7.state = NET_CLOSED;
    }
    else if(TcpClient_8.IPaddress == IPaddress)
    {
      TcpClient_8.state = NET_CLOSED;
    }
    else if(TcpClient_9.IPaddress == IPaddress)
    {
      TcpClient_9.state = NET_CLOSED;
    }
    else if(TcpClient_10.IPaddress == IPaddress)
    {
      TcpClient_10.state = NET_CLOSED;
    }
    else if(TcpClient_11.IPaddress == IPaddress)
    {
      TcpClient_11.state = NET_CLOSED;
    }
    
    mem_free(RxBuffer);
    
    return tcp_close(pcb);
  }
  return ERR_OK;
}


/*
*********************************************************************************************************
*                                             tcp_conn_err()
*
* Description : callback function of tcp_err.
*
* Argument(s) : see tcp_err.
*
* Return(s)   : ERR_OK.
*********************************************************************************************************
*/

static void tcp_conn_err(void *arg, err_t err)
{
  struct RxBuffer  *tempstruct;
  tempstruct = (struct RxBuffer  *)arg;

  mem_free(tempstruct);
}


/*
*********************************************************************************************************
*                                             AppTaskClient()
*
* Description : ucos-ii task. This task creates tcp connection dynamically by configuration file.
*               After connection is established, modbus request will be sent.
*
* Argument(s) : p_arg    argument passed by caller of this task.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void AppTaskClient (void *p_arg)
{
  (void)p_arg;
  
  OSTimeDlyHMSM(0, 0, 15, 0);
  
  tcpclient_connect(&TcpClient_1);
  OSTimeDlyHMSM(0, 0, 5, 0);
  
  tcpclient_connect(&TcpClient_2);
  OSTimeDlyHMSM(0, 0, 5, 0);
  
  while(1)
  {
      //测控1
      //遥测请求
      tx_len=tcp_getAnalogInput(1,MD_request,1,0x1c);
      if(TcpClient_1.state == NET_CONNECTED)
      {
        tcp_write(TcpClient_1.TCP_pcb, MD_request, tx_len, 1);
        tcp_output(TcpClient_1.TCP_pcb);
      }
      else
      {
        printf("\n\rCH1重连接");
        tcpclient_connect(&TcpClient_1);
      }
      memset(MD_request,0,20);
      OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
   
      
      //测控2
      //遥测请求
      tx_len=tcp_getAnalogInput(1,MD_request,1,0x1c);
      if(TcpClient_2.state == NET_CONNECTED)
      {
        tcp_write(TcpClient_2.TCP_pcb, MD_request, tx_len, 1);
        tcp_output(TcpClient_2.TCP_pcb);
      }
      else
      {
        printf("\n\rCH2重连接");
        tcpclient_connect(&TcpClient_2);
      }
      memset(MD_request,0,20);
      OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
      
  }
}
