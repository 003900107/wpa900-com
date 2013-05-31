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
#include "data_handle.h"

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
static uint8_t MD_response[100];
static uint8_t tx_len;
//static uint8_t rx_len;

/*
enum Net_States
{
  NET_NOT_CONNECTED = 0,
  NET_CONNECTED,
  NET_RECEIVED,
  NET_CLOSING,
};


struct TCP_CLIENT_FIELD
{
  enum Net_States Link_State; // ����״̬ 
  struct tcp_pcb *pcb;        // ��ǰtcp_pcb 
};
*/

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

/* Initialize all TCP link state */
enum net_state state_CH1 = NET_CLOSED;
enum net_state state_CH2 = NET_CLOSED;
enum net_state state_CH3 = NET_CLOSED;
enum net_state state_CH4 = NET_CLOSED;
enum net_state state_CH5 = NET_CLOSED;
enum net_state state_CH6 = NET_CLOSED;
enum net_state state_CH7 = NET_CLOSED;
enum net_state state_CH8 = NET_CLOSED;
enum net_state state_CH9 = NET_CLOSED;
enum net_state state_CH10 = NET_CLOSED;
enum net_state state_CH11 = NET_CLOSED;

/* Define tcp_pcb of tcp link */
static struct tcp_pcb *TCP_pcb_CH1;
static struct tcp_pcb *TCP_pcb_CH2;
static struct tcp_pcb *TCP_pcb_CH3;
static struct tcp_pcb *TCP_pcb_CH4;
static struct tcp_pcb *TCP_pcb_CH5;
static struct tcp_pcb *TCP_pcb_CH6;
static struct tcp_pcb *TCP_pcb_CH7;
static struct tcp_pcb *TCP_pcb_CH8;
static struct tcp_pcb *TCP_pcb_CH9;
static struct tcp_pcb *TCP_pcb_CH10;
static struct tcp_pcb *TCP_pcb_CH11;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void  Get_TCP_Connect_CH1(void);
static void  Get_TCP_Connect_CH2(void);
static void  Get_TCP_Connect_CH3(void);
static void  Get_TCP_Connect_CH4(void);
static void  Get_TCP_Connect_CH5(void);
static void  Get_TCP_Connect_CH6(void);
static void  Get_TCP_Connect_CH7(void);
static void  Get_TCP_Connect_CH8(void);
static void  Get_TCP_Connect_CH9(void);
static void  Get_TCP_Connect_CH10(void);
static void  Get_TCP_Connect_CH11(void);

static err_t tcp_connect_callback_1(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_connect_callback_2(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_connect_callback_3(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_connect_callback_4(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_connect_callback_5(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_connect_callback_6(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_connect_callback_7(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_connect_callback_8(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_connect_callback_9(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_connect_callback_10(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_connect_callback_11(void *arg, struct tcp_pcb *pcb, err_t err);

static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_recv_callback_test(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_recv_callback_test2(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

static err_t tcp_recv_callback_1(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_recv_callback_2(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_recv_callback_3(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_recv_callback_4(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_recv_callback_5(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_recv_callback_6(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_recv_callback_7(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_recv_callback_8(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_recv_callback_9(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_recv_callback_10(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_recv_callback_11(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

static void tcp_conn_err(void *arg, err_t err);


/* FUNCTION */
/*
static void Get_TCP_Connect_CH1(void)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb1;

  pcb1 = tcp_new();
  IP4_ADDR( &DestIPaddr, 172, 18, 154, 50 );
  tcp_bind(pcb1, IP_ADDR_ANY, 501);
  tcp_arg(pcb1, NULL);
  tcp_connect(pcb1,&DestIPaddr,502,tcp_connect_callback_1);
  
  //tcp_arg(pcb1, mem_calloc(sizeof(struct RxBuffer), 1));
  //tcp_err(pcb1, tcp_conn_err);
  tcp_recv(pcb1,tcp_recv_callback_1);
}
*/

static void Get_TCP_Connect_CH1(void)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb1;

  pcb1 = tcp_new();
  IP4_ADDR( &DestIPaddr, 172, 18, 154, 50 );
  tcp_bind(pcb1, IP_ADDR_ANY, 501);
  tcp_arg(pcb1, NULL);
  tcp_connect(pcb1,&DestIPaddr,502,tcp_connect_callback_1);
  
  tcp_arg(pcb1, mem_calloc(sizeof(struct RxBuffer), 1));
  tcp_err(pcb1, tcp_conn_err);
  tcp_recv(pcb1,tcp_recv_callback_test);
}

static void Get_TCP_Connect_CH2(void)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb2;

  pcb2 = tcp_new();
  IP4_ADDR( &DestIPaddr, 172, 18, 154, 52 );
  tcp_bind(pcb2, IP_ADDR_ANY, 502);
  tcp_arg(pcb2, NULL);
  tcp_connect(pcb2,&DestIPaddr,502,tcp_connect_callback_2);
  
  tcp_arg(pcb2, mem_calloc(sizeof(struct RxBuffer), 1));
  tcp_err(pcb2, tcp_conn_err);
  tcp_recv(pcb2,tcp_recv_callback_test2);
}

/*
static void Get_TCP_Connect_CH2(void)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb2;

  pcb2 = tcp_new();
  IP4_ADDR( &DestIPaddr, 172, 18, 154, 52 );
  tcp_bind(pcb2, IP_ADDR_ANY, 502);
  tcp_arg(pcb2, NULL);
  tcp_connect(pcb2,&DestIPaddr,502,tcp_connect_callback_2);
  
  //tcp_arg(pcb2, mem_calloc(sizeof(struct RxBuffer), 1));
  //tcp_err(pcb2, tcp_conn_err);
  tcp_recv(pcb2,tcp_recv_callback_2);
}
*/
static void Get_TCP_Connect_CH3(void)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb3;

  pcb3 = tcp_new();
  //pcb3->so_options |= SOF_KEEPALIVE;//��Ӳ���2012��12��28��
  
  IP4_ADDR( &DestIPaddr, 172, 20, 100, 103 );
  tcp_bind(pcb3, IP_ADDR_ANY, 503);
  
  tcp_arg(pcb3, NULL);
  tcp_connect(pcb3,&DestIPaddr,5185,tcp_connect_callback_3);
  
  //tcp_arg(pcb3, mem_calloc(sizeof(struct RxBuffer), 1));
  //tcp_err(pcb3, tcp_conn_err);
  tcp_recv(pcb3,tcp_recv_callback_3);
}

static void Get_TCP_Connect_CH4(void)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb4;

  pcb4 = tcp_new();
  //pcb4->so_options |= SOF_KEEPALIVE;//��Ӳ���2012��12��28��
  
  IP4_ADDR( &DestIPaddr, 172, 20, 100, 104 );
  tcp_bind(pcb4, IP_ADDR_ANY, 504);
  
  tcp_arg(pcb4, NULL);
  tcp_connect(pcb4,&DestIPaddr,5185,tcp_connect_callback_4);
  
  //tcp_arg(pcb4, mem_calloc(sizeof(struct RxBuffer), 1));
  //tcp_err(pcb4, tcp_conn_err);
  tcp_recv(pcb4,tcp_recv_callback_4);
}

static void Get_TCP_Connect_CH5(void)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb5;

  pcb5 = tcp_new();
  //pcb5->so_options |= SOF_KEEPALIVE;//��Ӳ���2012��12��28��
  
  IP4_ADDR( &DestIPaddr, 172, 20, 100, 105 );
  tcp_bind(pcb5, IP_ADDR_ANY, 505);
  
  tcp_arg(pcb5, NULL);
  tcp_connect(pcb5,&DestIPaddr,5185,tcp_connect_callback_5);
  
  //tcp_arg(pcb5, mem_calloc(sizeof(struct RxBuffer), 1));
  //tcp_err(pcb5, tcp_conn_err);
  tcp_recv(pcb5,tcp_recv_callback_5);
}

static void Get_TCP_Connect_CH6(void)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb6;

  pcb6 = tcp_new();
  IP4_ADDR( &DestIPaddr, 172, 20, 100, 106 );
  tcp_bind(pcb6, IP_ADDR_ANY, 506);
  tcp_arg(pcb6, NULL);
  tcp_connect(pcb6,&DestIPaddr,5185,tcp_connect_callback_6);
  
  //tcp_arg(pcb6, mem_calloc(sizeof(struct RxBuffer), 1));
  //tcp_err(pcb6, tcp_conn_err);
  tcp_recv(pcb6,tcp_recv_callback_6);
}

static void Get_TCP_Connect_CH7(void)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb7;

  pcb7 = tcp_new();
  IP4_ADDR( &DestIPaddr, 172, 20, 100, 107 );
  tcp_bind(pcb7, IP_ADDR_ANY, 507);
  tcp_arg(pcb7, NULL);
  tcp_connect(pcb7,&DestIPaddr,5185,tcp_connect_callback_7);
  
  //tcp_arg(pcb7, mem_calloc(sizeof(struct RxBuffer), 1));
  //tcp_err(pcb7, tcp_conn_err);
  tcp_recv(pcb7,tcp_recv_callback_7);
}

static void Get_TCP_Connect_CH8(void)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb8;

  pcb8 = tcp_new();
  IP4_ADDR( &DestIPaddr, 172, 20, 100, 108 );
  tcp_bind(pcb8, IP_ADDR_ANY, 508);
  tcp_arg(pcb8, NULL);
  tcp_connect(pcb8,&DestIPaddr,5185,tcp_connect_callback_8);
  
  //tcp_arg(pcb8, mem_calloc(sizeof(struct RxBuffer), 1));
  //tcp_err(pcb8, tcp_conn_err);
  tcp_recv(pcb8,tcp_recv_callback_8);
}

static void Get_TCP_Connect_CH9(void)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb9;

  pcb9 = tcp_new();
  IP4_ADDR( &DestIPaddr, 172, 20, 100, 109 );
  tcp_bind(pcb9, IP_ADDR_ANY, 509);
  tcp_arg(pcb9, NULL);
  tcp_connect(pcb9,&DestIPaddr,5185,tcp_connect_callback_9);
  
  //tcp_arg(pcb9, mem_calloc(sizeof(struct RxBuffer), 1));
  //tcp_err(pcb9, tcp_conn_err);
  tcp_recv(pcb9,tcp_recv_callback_9);
}

static void Get_TCP_Connect_CH10(void)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb10;

  pcb10 = tcp_new();
  IP4_ADDR( &DestIPaddr, 172, 20, 100, 110 );
  tcp_bind(pcb10, IP_ADDR_ANY, 510);
  tcp_arg(pcb10, NULL);
  tcp_connect(pcb10,&DestIPaddr,5185,tcp_connect_callback_10);
  
  //tcp_arg(pcb10, mem_calloc(sizeof(struct RxBuffer), 1));
  //tcp_err(pcb10, tcp_conn_err);
  tcp_recv(pcb10,tcp_recv_callback_10);
}

static void Get_TCP_Connect_CH11(void)
{
  struct ip_addr DestIPaddr;
  struct tcp_pcb *pcb11;

  pcb11 = tcp_new();
  IP4_ADDR( &DestIPaddr, 172, 20, 100, 111 );
  tcp_bind(pcb11, IP_ADDR_ANY, 511);
  tcp_arg(pcb11, NULL);
  tcp_connect(pcb11,&DestIPaddr,5185,tcp_connect_callback_11);
  
  //tcp_arg(pcb11, mem_calloc(sizeof(struct RxBuffer), 1));
  //tcp_err(pcb11, tcp_conn_err);
  tcp_recv(pcb11,tcp_recv_callback_11);
}

static err_t tcp_connect_callback_1(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  printf("CH1���ӳɹ�\n");
  TCP_pcb_CH1 = tpcb;
  state_CH1 = NET_CONNECTED;
  return ERR_OK;
}

static err_t tcp_connect_callback_2(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  printf("CH2���ӳɹ�\n");
  TCP_pcb_CH2 = tpcb;
  state_CH2 = NET_CONNECTED;
  return ERR_OK;
}

static err_t tcp_connect_callback_3(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  printf("CH3���ӳɹ�\n");
  TCP_pcb_CH3 = tpcb;
  state_CH3 = NET_CONNECTED;
  return ERR_OK;
}

static err_t tcp_connect_callback_4(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  printf("CH4���ӳɹ�\n");
  TCP_pcb_CH4 = tpcb;
  state_CH4 = NET_CONNECTED;
  return ERR_OK;
}

static err_t tcp_connect_callback_5(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  printf("CH5���ӳɹ�\n");
  TCP_pcb_CH5 = tpcb;
  state_CH5 = NET_CONNECTED;
  return ERR_OK;
}

static err_t tcp_connect_callback_6(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  printf("CH6���ӳɹ�\n");
  TCP_pcb_CH6 = tpcb;
  state_CH6 = NET_CONNECTED;
  return ERR_OK;
}

static err_t tcp_connect_callback_7(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  printf("CH7���ӳɹ�\n");
  TCP_pcb_CH7 = tpcb;
  state_CH7 = NET_CONNECTED;
  return ERR_OK;
}

static err_t tcp_connect_callback_8(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  printf("CH8���ӳɹ�\n");
  TCP_pcb_CH8 = tpcb;
  state_CH8 = NET_CONNECTED;
  return ERR_OK;
}

static err_t tcp_connect_callback_9(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  printf("CH9���ӳɹ�\n");
  TCP_pcb_CH9 = tpcb;
  state_CH9 = NET_CONNECTED;
  return ERR_OK;
}

static err_t tcp_connect_callback_10(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  printf("CH10���ӳɹ�\n");
  TCP_pcb_CH10 = tpcb;
  state_CH10 = NET_CONNECTED;
  return ERR_OK;
}

static err_t tcp_connect_callback_11(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  printf("CH11���ӳɹ�\n");
  TCP_pcb_CH11 = tpcb;
  state_CH11 = NET_CONNECTED;
  return ERR_OK;
}


static err_t tcp_recv_callback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  struct RxBuffer *RxBuffer = (struct RxBuffer *)arg;
  uint8_t i,done;
  char *c;
  
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
        if(RxBuffer->length < MAX_BUFF_SIZE) 
	{
          RxBuffer->bytes[RxBuffer->length++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==c[5]));
        }
      }
    }
    if(done) 
    {
      netdata_handle(&(RxBuffer->bytes[6]));
      
      RxBuffer->length = 0;
    }
    pbuf_free(p);
  }
  else if (err == ERR_OK) 
  {
    //��� 2012��12��26��
    pbuf_free(p);
    
    mem_free(RxBuffer);
    
    return tcp_close(pcb);
  }
  return ERR_OK;
}

static err_t tcp_recv_callback_test(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  struct RxBuffer *RxBuffer = (struct RxBuffer *)arg;
  //���ò���arg��2012��12��26��
  uint8_t RxBuf[500];
  uint8_t j=0;
  uint8_t i,done;
  char *c;
  
  //printf("���ջص���������\n");
  
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
        
        /*
        if(j<MAX_BUFF_SIZE)
        {
          RxBuf[j++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(4+c[5])));
        }
        */
      }
    }
    if(done) 
    {
      printf("\n���1");
    }
    pbuf_free(p);
  }
  else if (err == ERR_OK) 
  {
    state_CH1 = NET_CLOSED;
    
    mem_free(RxBuffer);
    
    return tcp_close(pcb);
  }
  return ERR_OK;
}

static err_t tcp_recv_callback_test2(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  struct RxBuffer *RxBuffer = (struct RxBuffer *)arg;
  //���ò���arg��2012��12��26��
  uint8_t RxBuf[500];
  uint8_t j=0;
  uint8_t i,done;
  char *c;
  
  //printf("���ջص���������\n");
  
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
        
        /*
        if(j<MAX_BUFF_SIZE)
        {
          RxBuf[j++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(4+c[5])));
        }
        */
      }
    }
    if(done) 
    {
      printf("\n\r���2");
    }
    pbuf_free(p);
  }
  else if (err == ERR_OK) 
  {
    state_CH2 = NET_CLOSED;
    
    mem_free(RxBuffer);
    
    return tcp_close(pcb);
  }
  return ERR_OK;
}

static err_t tcp_recv_callback_1(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  uint8_t RxBuf[100];
  uint8_t j=0;
  uint8_t i,done;
  char *c;
  OS_CPU_SR  cpu_sr;

  if (p != NULL) 
  {
    /* We call this function to tell the LwIp that we have processed the data */
    /* This lets the stack advertise a larger window, so more data can be received*/
    tcp_recved(pcb, p->tot_len);

    done = 0;
    
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len && !done ; i++) 
      {
        if(j<MAX_BUFF_SIZE)
        {
          RxBuf[j++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(5+c[5])));
        }
      }
    }
    if(done) 
    {
      //OS_ENTER_CRITICAL();
      //netdata_handle(&(RxBuf[6]));
      //OS_EXIT_CRITICAL();
      
      printf("\n���1");
      //for(i=0;i<(6+c[5]);i++)
      //{
      //  printf("%x ",RxBuf[i]);
      //}
    }
    
    pbuf_free(p);
  }
  else
  {
    state_CH1 = NET_CLOSED;
    tcp_close(pcb);
  }
  
  return ERR_OK;
}

static err_t tcp_recv_callback_2(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  uint8_t RxBuf[100];
  uint8_t j=0;
  uint8_t i,done;
  char *c;
  OS_CPU_SR  cpu_sr;

  if (p != NULL) 
  {
    /* We call this function to tell the LwIp that we have processed the data */
    /* This lets the stack advertise a larger window, so more data can be received*/
    tcp_recved(pcb, p->tot_len);

    done = 0;
    
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len && !done ; i++) 
      {
        if(j<MAX_BUFF_SIZE)
        {
          RxBuf[j++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(5+c[5])));
        }
      }
    }
    if(done) 
    {
      //OS_ENTER_CRITICAL();
      netdata_handle(&(RxBuf[6]));
      //OS_EXIT_CRITICAL();
      
      printf("\n\r���2");
    }
    
    pbuf_free(p);
  }
  else
  {
    printf("\n\rCH2�ر�����");
    state_CH2 = NET_CLOSED;
    tcp_close(pcb);
  }
  
  return ERR_OK;
}

static err_t tcp_recv_callback_3(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  uint8_t RxBuf[100];
  uint8_t j=0;
  uint8_t i,done;
  char *c;
  OS_CPU_SR  cpu_sr;

  if (p != NULL) 
  {
    /* We call this function to tell the LwIp that we have processed the data */
    /* This lets the stack advertise a larger window, so more data can be received*/
    tcp_recved(pcb, p->tot_len);

    done = 0;
    
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len && !done ; i++) 
      {
        if(j<MAX_BUFF_SIZE)
        {
          RxBuf[j++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(5+c[5])));
        }
      }
    }
    if(done) 
    {
      //OS_ENTER_CRITICAL();
      netdata_handle(&(RxBuf[6]));
      //OS_EXIT_CRITICAL();
      printf("\n\r���3");
    }
    
    pbuf_free(p);
  }
  else
  {
    printf("\n\rCH3�ر�����");
    state_CH3 = NET_CLOSED;
    tcp_close(pcb);
  }
  
  return ERR_OK;
}

static err_t tcp_recv_callback_4(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  uint8_t RxBuf[100];
  uint8_t j=0;
  uint8_t i,done;
  char *c;
  OS_CPU_SR  cpu_sr;

  if (p != NULL) 
  {
    /* We call this function to tell the LwIp that we have processed the data */
    /* This lets the stack advertise a larger window, so more data can be received*/
    tcp_recved(pcb, p->tot_len);

    done = 0;
    
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len && !done ; i++) 
      {
        if(j<MAX_BUFF_SIZE)
        {
          RxBuf[j++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(5+c[5])));
        }
      }
    }
    if(done) 
    {
      //OS_ENTER_CRITICAL();
      netdata_handle(&(RxBuf[6]));
      //OS_EXIT_CRITICAL();
      printf("\n\r���4");
    }
    
    pbuf_free(p);
  }
  else
  {
    printf("\n\rCH4�ر�����");
    state_CH4 = NET_CLOSED;
    tcp_close(pcb);
  }
  
  return ERR_OK;
}

static err_t tcp_recv_callback_5(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  uint8_t RxBuf[100];
  uint8_t j=0;
  uint8_t i,done;
  char *c;
  OS_CPU_SR  cpu_sr;

  if (p != NULL) 
  {
    /* We call this function to tell the LwIp that we have processed the data */
    /* This lets the stack advertise a larger window, so more data can be received*/
    tcp_recved(pcb, p->tot_len);

    done = 0;
    
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len && !done ; i++) 
      {
        if(j<MAX_BUFF_SIZE)
        {
          RxBuf[j++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(5+c[5])));
        }
      }
    }
    if(done) 
    {
      //OS_ENTER_CRITICAL();
      netdata_handle(&(RxBuf[6]));
      //OS_EXIT_CRITICAL();
      printf("\n\r���5");
    }
    
    pbuf_free(p);
  }
  else
  {
    printf("\n\rCH5�ر�����");
    state_CH5 = NET_CLOSED;
    tcp_close(pcb);
  }
  
  return ERR_OK;
}

static err_t tcp_recv_callback_6(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  uint8_t RxBuf[100];
  uint8_t j=0;
  uint8_t i,done;
  char *c;
  OS_CPU_SR  cpu_sr;

  if (p != NULL) 
  {
    /* We call this function to tell the LwIp that we have processed the data */
    /* This lets the stack advertise a larger window, so more data can be received*/
    tcp_recved(pcb, p->tot_len);

    done = 0;
    
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len && !done ; i++) 
      {
        if(j<MAX_BUFF_SIZE)
        {
          RxBuf[j++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(5+c[5])));
        }
      }
    }
    if(done) 
    {
      //OS_ENTER_CRITICAL();
      netdata_handle(&(RxBuf[6]));
      //OS_EXIT_CRITICAL();
      printf("\n\r���6");
    }
    
    pbuf_free(p);
  }
  else
  {
    printf("\n\rCH6�ر�����");
    state_CH6 = NET_CLOSED;
    tcp_close(pcb);
  }
  
  return ERR_OK;
}

static err_t tcp_recv_callback_7(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  uint8_t RxBuf[100];
  uint8_t j=0;
  uint8_t i,done;
  char *c;
  OS_CPU_SR  cpu_sr;

  if (p != NULL) 
  {
    /* We call this function to tell the LwIp that we have processed the data */
    /* This lets the stack advertise a larger window, so more data can be received*/
    tcp_recved(pcb, p->tot_len);

    done = 0;
    
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len && !done ; i++) 
      {
        if(j<MAX_BUFF_SIZE)
        {
          RxBuf[j++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(5+c[5])));
        }
      }
    }
    if(done) 
    {
      //OS_ENTER_CRITICAL();
      netdata_handle(&(RxBuf[6]));
      //OS_EXIT_CRITICAL();
      printf("\n\r���7");
    }
    
    pbuf_free(p);
  }
  else
  {
    printf("\n\rCH7�ر�����");
    state_CH7 = NET_CLOSED;
    tcp_close(pcb);
  }
  
  return ERR_OK;
}

static err_t tcp_recv_callback_8(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  uint8_t RxBuf[100];
  uint8_t j=0;
  uint8_t i,done;
  char *c;
  OS_CPU_SR  cpu_sr;

  if (p != NULL) 
  {
    /* We call this function to tell the LwIp that we have processed the data */
    /* This lets the stack advertise a larger window, so more data can be received*/
    tcp_recved(pcb, p->tot_len);

    done = 0;
    
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len && !done ; i++) 
      {
        if(j<MAX_BUFF_SIZE)
        {
          RxBuf[j++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(5+c[5])));
        }
      }
    }
    if(done) 
    {
      //OS_ENTER_CRITICAL();
      netdata_handle(&(RxBuf[6]));
      //OS_EXIT_CRITICAL();
      printf("\n\r���8");
    }
    
    pbuf_free(p);
  }
  else
  {
    printf("\n\rCH8�ر�����");
    state_CH8 = NET_CLOSED;
    tcp_close(pcb);
  }
  
  return ERR_OK;
}

static err_t tcp_recv_callback_9(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  uint8_t RxBuf[100];
  uint8_t j=0;
  uint8_t i,done;
  char *c;
  OS_CPU_SR  cpu_sr;

  if (p != NULL) 
  {
    /* We call this function to tell the LwIp that we have processed the data */
    /* This lets the stack advertise a larger window, so more data can be received*/
    tcp_recved(pcb, p->tot_len);

    done = 0;
    
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len && !done ; i++) 
      {
        if(j<MAX_BUFF_SIZE)
        {
          RxBuf[j++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(5+c[5])));
        }
      }
    }
    if(done) 
    {
      //OS_ENTER_CRITICAL();
      netdata_handle(&(RxBuf[6]));
      //OS_EXIT_CRITICAL();
      printf("\n\r���9");
    }
    
    pbuf_free(p);
  }
  else
  {
    printf("\n\rCH9�ر�����");
    state_CH9 = NET_CLOSED;
    tcp_close(pcb);
  }
  
  return ERR_OK;
}

static err_t tcp_recv_callback_10(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  uint8_t RxBuf[100];
  uint8_t j=0;
  uint8_t i,done;
  char *c;
  OS_CPU_SR  cpu_sr;

  if (p != NULL) 
  {
    /* We call this function to tell the LwIp that we have processed the data */
    /* This lets the stack advertise a larger window, so more data can be received*/
    tcp_recved(pcb, p->tot_len);

    done = 0;
    
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len && !done ; i++) 
      {
        if(j<MAX_BUFF_SIZE)
        {
          RxBuf[j++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(5+c[5])));
        }
      }
    }
    if(done) 
    {
      //OS_ENTER_CRITICAL();
      netdata_handle(&(RxBuf[6]));
      //OS_EXIT_CRITICAL();
      printf("\n\r���10");
    }
    
    pbuf_free(p);
  }
  else
  {
    printf("\n\rCH10�ر�����");
    state_CH10 = NET_CLOSED;
    tcp_close(pcb);
  }
  
  return ERR_OK;
}

static err_t tcp_recv_callback_11(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{ 
  struct pbuf *q;
  uint8_t RxBuf[100];
  uint8_t j=0;
  uint8_t i,done;
  char *c;
  OS_CPU_SR  cpu_sr;

  if (p != NULL) 
  {
    /* We call this function to tell the LwIp that we have processed the data */
    /* This lets the stack advertise a larger window, so more data can be received*/
    tcp_recved(pcb, p->tot_len);

    done = 0;
    
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len && !done ; i++) 
      {
        if(j<MAX_BUFF_SIZE)
        {
          RxBuf[j++] = c[i];
          done=((c[0]==0x00)&&(c[1]==0x00)&&(c[2]==0x00)&&(c[3]==0x00)&&(c[4]==0x00)&&(i==(5+c[5])));
        }
      }
    }
    if(done) 
    {
      //OS_ENTER_CRITICAL();
      netdata_handle(&(RxBuf[6]));
      //OS_EXIT_CRITICAL();
      printf("\n\r���11");
    }
    
    pbuf_free(p);
  }
  else
  {
    printf("\n\rCH11�ر�����");
    state_CH11 = NET_CLOSED;
    tcp_close(pcb);
  }
  
  return ERR_OK;
}

static void tcp_conn_err(void *arg, err_t err)
{
  struct RxBuffer  *tempstruct;
  tempstruct = (struct RxBuffer  *)arg;

  mem_free(tempstruct);
}




void AppTaskClient (void *p_arg)
{
  (void)p_arg;
  INT8U  err1,err2;
  
  OSTimeDlyHMSM(0, 0, 15, 0);
  
  //printf("��һ������......");
  Get_TCP_Connect_CH1();
  OSTimeDlyHMSM(0, 0, 5, 0);
  
  Get_TCP_Connect_CH2();
  OSTimeDlyHMSM(0, 0, 5, 0);
  /*
  Get_TCP_Connect_CH3();
  OSTimeDlyHMSM(0, 0, 5, 0);
  
  Get_TCP_Connect_CH4();
  OSTimeDlyHMSM(0, 0, 5, 0);
  
  Get_TCP_Connect_CH5();
  OSTimeDlyHMSM(0, 0, 5, 0);
  
  Get_TCP_Connect_CH6();
  OSTimeDlyHMSM(0, 0, 5, 0);
  
  Get_TCP_Connect_CH7();
  OSTimeDlyHMSM(0, 0, 5, 0);
  
  Get_TCP_Connect_CH8();
  OSTimeDlyHMSM(0, 0, 5, 0);
  
  Get_TCP_Connect_CH9();
  OSTimeDlyHMSM(0, 0, 5, 0);
  
  Get_TCP_Connect_CH10();
  OSTimeDlyHMSM(0, 0, 5, 0);
  
  Get_TCP_Connect_CH11();
  OSTimeDlyHMSM(0, 0, 5, 0);
  */
  
  while(1)
  {
    
    
    //���1
    
      //ң������
      //tx_len=tcp_getInputStatus(1,MD_request,1,1);
      //if(state_CH1 == NET_CONNECTED)
      //{
      //  tcp_write(TCP_pcb_CH1, MD_request, tx_len, 1); 
      //  tcp_output(TCP_pcb_CH1);
      //}
      //else
      //{
      //  printf("\n\rCH1������");
      //  Get_TCP_Connect_CH1();
      //}
      //memset(MD_request,0,20);
      //OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
    
      //���1
      //ң������
      tx_len=tcp_getAnalogInput(1,MD_request,1,0x1c);
      if(state_CH1 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH1, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH1);
      }
      else
      {
        printf("\n\rCH1������");
        Get_TCP_Connect_CH1();
      }
      memset(MD_request,0,20);
      OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
   
      
      //���2
      //ң������
      tx_len=tcp_getAnalogInput(1,MD_request,1,0x1c);
      if(state_CH2 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH2, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH2);
      }
      else
      {
        printf("\n\rCH2������");
        Get_TCP_Connect_CH2();
      }
      memset(MD_request,0,20);
      OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
      /*
      //���3
      //ң������
      tx_len=tcp_getAnalogInput(1,MD_request,1,0x1c);
      if(state_CH3 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH3, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH3);
      }
      else
      {
        printf("\n\rCH3������");
        Get_TCP_Connect_CH3();
      }
      memset(MD_request,0,20);
      OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
      
      //���4
      //ң������
      tx_len=tcp_getAnalogInput(1,MD_request,1,0x1c);
      if(state_CH4 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH4, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH4);
      }
      else
      {
        printf("\n\rCH4������");
        Get_TCP_Connect_CH4();
      }
      memset(MD_request,0,20);
      OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
      
      //���5
      //ң������
      tx_len=tcp_getAnalogInput(1,MD_request,1,0x1c);
      if(state_CH5 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH5, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH5);
      }
      else
      {
        printf("\n\rCH5������");
        Get_TCP_Connect_CH5();
      }
      memset(MD_request,0,20);
      OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
      
      //���6
      //ң������
      tx_len=tcp_getAnalogInput(1,MD_request,1,0x1c);
      if(state_CH6 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH6, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH6);
      }
      else
      {
        printf("\n\rCH6������");
        Get_TCP_Connect_CH6();
      }
      memset(MD_request,0,20);
      OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
      
      //���7
      //ң������
      tx_len=tcp_getAnalogInput(1,MD_request,1,0x1c);
      if(state_CH7 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH7, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH7);
      }
      else
      {
        printf("\n\rCH7������");
        Get_TCP_Connect_CH7();
      }
      memset(MD_request,0,20);
      OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
      
      //���8
      //ң������
      tx_len=tcp_getAnalogInput(1,MD_request,1,0x1c);
      if(state_CH8 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH8, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH8);
      }
      else
      {
        printf("\n\rCH8������");
        Get_TCP_Connect_CH8();
      }
      memset(MD_request,0,20);
      OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
      
      //���9
      //ң������
      tx_len=tcp_getAnalogInput(1,MD_request,1,0x1c);
      if(state_CH9 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH9, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH9);
      }
      else
      {
        printf("\n\rCH9������");
        Get_TCP_Connect_CH9();
      }
      memset(MD_request,0,20);
      OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
      
      //���10
      //ң������
      tx_len=tcp_getAnalogInput(1,MD_request,1,0x1c);
      if(state_CH10 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH10, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH10);
      }
      else
      {
        printf("\n\rCH10������");
        Get_TCP_Connect_CH10();
      }
      memset(MD_request,0,20);
      OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
      
      //���11
      //ң������
      tx_len=tcp_getAnalogInput(1,MD_request,1,0x1c);
      if(state_CH11 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH11, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH11);
      }
      else
      {
        printf("\n\rCH11������");
        Get_TCP_Connect_CH11();
      }
      memset(MD_request,0,20);
      OSTimeDlyHMSM(0, 0, 0, MODBUS_TCP_INTERVAL_MS);
      */
  }
}

void command_execute(uint16_t id,uint16_t startaddr,uint16_t state)
{
  switch(id)
  {
  case 1://���1
    {
      tx_len=tcp_setCoil(1,MD_request,startaddr,state);
      if(state_CH1 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH1, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH1);
      }
      break;
    }
  case 2://���2
    {
      tx_len=tcp_setCoil(2,MD_request,startaddr,state);
      if(state_CH2 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH2, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH2);
      }
      break;
    }
  case 3://���3
    {
      tx_len=tcp_setCoil(3,MD_request,startaddr,state);
      if(state_CH3 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH3, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH3);
      }
      break;
    }
  case 4://���4
    {
      tx_len=tcp_setCoil(4,MD_request,startaddr,state);
      if(state_CH4 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH4, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH4);
      }
      break;
    }
  case 5://���5
    {
      tx_len=tcp_setCoil(5,MD_request,startaddr,state);
      if(state_CH5 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH5, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH5);
      }
      break;
    }
  case 6://���6
    {
      tx_len=tcp_setCoil(6,MD_request,startaddr,state);
      if(state_CH6 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH6, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH6);
      }
      break;
    }
  case 7://���7
    {
      tx_len=tcp_setCoil(7,MD_request,startaddr,state);
      if(state_CH7 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH7, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH7);
      }
      break;
    }
  case 8://���8
    {
      tx_len=tcp_setCoil(8,MD_request,startaddr,state);
      if(state_CH8 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH8, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH8);
      }
      break;
    }
  case 9://���9
    {
      tx_len=tcp_setCoil(9,MD_request,startaddr,state);
      if(state_CH9 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH9, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH9);
      }
      break;
    }
  case 10://���10
    {
      tx_len=tcp_setCoil(10,MD_request,startaddr,state);
      if(state_CH10 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH10, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH10);
      }
      break;
    }
  case 11://���11
    {
      tx_len=tcp_setCoil(11,MD_request,startaddr,state);
      if(state_CH11 == NET_CONNECTED)
      {
        tcp_write(TCP_pcb_CH11, MD_request, tx_len, 1);
        tcp_output(TCP_pcb_CH11);
      }
      break;
    }
  default:
    break;
  }
}