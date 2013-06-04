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

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "telnet.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define MAX_NAME_SIZE 1500

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

struct name 
{
  int length;
  char bytes[MAX_NAME_SIZE];
};

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static err_t Telnet_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
static err_t Telnet_accept(void *arg, struct tcp_pcb *pcb, err_t err);
static void Telnet_conn_err(void *arg, err_t err);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Called when a data is received on the telnet connection
  * @param  arg	the user argument
  * @param  pcb	the tcp_pcb that has received the data
  * @param  p	the packet buffer
  * @param  err	the error value linked with the received data
  * @retval error value
  */
static err_t Telnet_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  struct pbuf *q;
  struct name *name = (struct name *)arg;
  int done;
  char *c;
  int i;
 

  /* We perform here any necessary processing on the pbuf */
  if (p != NULL) 
  {        
	/* We call this function to tell the LwIp that we have processed the data */
	/* This lets the stack advertise a larger window, so more data can be received*/
	tcp_recved(pcb, p->tot_len);

    /* Check the name if NULL, no data passed, return withh illegal argument error */
	if(!name) 
    {
      pbuf_free(p);
      return ERR_ARG;
    }

    //done = 0;
    done = 1;
    for(q=p; q != NULL; q = q->next) 
    {
      c = q->payload;
      for(i=0; i<q->len; i++) 
	  {
        if(name->length < MAX_NAME_SIZE) 
	    {
          name->bytes[name->length++] = c[i];
        }
      }
    }
    if(done) 
    {
      tcp_write(pcb, name->bytes, name->length, TCP_WRITE_FLAG_COPY);
      
      name->length = 0;
    }
	
	/* End of processing, we free the pbuf */
    pbuf_free(p);
  }  
  else if (err == ERR_OK) 
  {
    /* When the pbuf is NULL and the err is ERR_OK, the remote end 
                                        is closing the connection. */
    /* We free the allocated memory and we close the connection */
    mem_free(name);
    return tcp_close(pcb);
  }
  return ERR_OK;

}

/**
  * @brief  This function when the Telnet connection is established
  * @param  arg  user supplied argument 
  * @param  pcb	 the tcp_pcb which accepted the connection
  * @param  err	 error value
  * @retval ERR_OK
  */
static err_t Telnet_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{     
  /* Tell LwIP to associate this structure with this connection. */
  tcp_arg(pcb, mem_calloc(sizeof(struct name), 1));	
  
  /* Configure LwIP to use our call back functions. */
  tcp_err(pcb, Telnet_conn_err);
  tcp_recv(pcb, Telnet_recv);      
  
  return ERR_OK;
}

/**
  * @brief  Initialize the telnet application  
  * @param  None 
  * @retval None 
  */
void Telnet_init(void)
{
  struct tcp_pcb *pcb;	            		
  
  /* Create a new TCP control block  */
  pcb = tcp_new();	                		 	

  /* Assign to the new pcb a local IP address and a port number */
  /* Using IP_ADDR_ANY allow the pcb to be used by any local interface */
  tcp_bind(pcb, IP_ADDR_ANY, 23);       //IP_ADDR_ANY  IP_ADDR_1
  
  /* Set the connection to the LISTEN state */
  pcb = tcp_listen(pcb);				

  /* Specify the function to be called when a connection is established */	
  tcp_accept(pcb, Telnet_accept);   
										
}

/**
  * @brief  This function is called when an error occurs on the connection 
  * @param  arg
  * @parm   err
  * @retval None 
  */
static void Telnet_conn_err(void *arg, err_t err)
{
  struct name *name;
  name = (struct name *)arg;

  mem_free(name);
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/


