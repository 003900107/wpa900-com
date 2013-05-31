/*
*********************************************************************************************************
*
*                                            MODBUS RTU (485)
*
*                                               STM32F107
*                                             ucos-II v2.92
*                                              lwip v1.3.1
*
* Filename      : RtuClient.c
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
#include "RtuClient.h"
#include "modbus_rtu_master.h"
#include "bsp.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define IN  1
#define OUT 0


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

INT8U rtu_request[10];
INT8U len;
uint8_t rx_buf[50];

/*
//modbus子站1
uint8_t SinglePoint_s1[32];
uint8_t MeasuredValue_s1[100];
//modbus子站2
uint8_t SinglePoint_s2[32];
uint8_t MeasuredValue_s2[100];
//modbus子站3
uint8_t SinglePoint_s3[32];
uint8_t MeasuredValue_s3[100];
//modbus子站4
uint8_t SinglePoint_s4[32];
uint8_t MeasuredValue_s4[100];
//modbus子站5
uint8_t SinglePoint_s5[32];
uint8_t MeasuredValue_s5[100];
*/


/*
*********************************************************************************************************
*                                            AppTaskRtuClient()
*
* Description : ucos-ii task. This task sends modbus request by 485 bus.
*
* Argument(s) : p_arg    is the argument passed to 'AppTaskRtuClient()' by 'AppTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : AppTaskCreate().
*
* Note(s)     : modbus-rtu request.
*********************************************************************************************************
*/

void AppTaskRtuClient (void *p_arg)
{
  (void)p_arg;
  
  OSTimeDlyHMSM(0, 0, 5, 0);
  
  while(1)
  {
    //节点1
    GPIO_SetBits(GPIOA,GPIO_Pin_8);
    len=rtu_getCoilStatus(1,rtu_request,0x0000,8);
    USART_send(USART1,rtu_request,len);
    GPIO_ResetBits(GPIOA,GPIO_Pin_8);
    OSTimeDlyHMSM(0, 0, 0, MODBUS_RTU_INTERVAL_MS);
    
    //节点2   
    GPIO_SetBits(GPIOA,GPIO_Pin_8);
    len=rtu_getCoilStatus(2,rtu_request,0x0000,8);
    USART_send(USART1,rtu_request,len);
    GPIO_ResetBits(GPIOA,GPIO_Pin_8);
    OSTimeDlyHMSM(0, 0, 0, MODBUS_RTU_INTERVAL_MS);
  }
}


/*
*********************************************************************************************************
*                                          BSP_IntHandlerUSART1()
*
* Description : USART1 interrupt handler function. This function receives modbus response.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : modbus-rtu response.
*********************************************************************************************************
*/

void  BSP_IntHandlerUSART1(void)
{
  //printf("中断？");
  uint8_t temp;
  static uint8_t i=0;
  static uint8_t rx_state=OUT;
  static uint8_t len=0,rx_len=0;
  
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
  {
    temp = USART_ReceiveData(USART2);
    
    switch(rx_state)
    {
    /* "STATE OUT" means last string has been received completely . It is free in 485. */
    case OUT:  
      {
        if(temp==0x01)
        {
          rx_state = IN;
          rx_buf[0]=temp;
          i++;
        }
        else
        {
          rx_state=OUT;
          i=0;
        }
        break;
      }
    /* "STATE IN" means the USART1 is receiving a string now . The string is not complete. */
    case IN:  
      {
        
        if(i==1)
        {
          if((temp==0x01)||(temp==0x02)||(temp==0x03)||(temp==0x04)
             ||(temp==0x05)||(temp==0x06))
          {
            rx_buf[1]=temp;
            i++;
            break;
          }
          else
          {
            rx_state=OUT;
            i=0;
            len=0;
          }
          break;
        }
        
        else if(i==2)
        {
          if(temp)
          {
            len=temp;
            rx_len=temp;
            rx_buf[2]=temp;
            i++;
          }
          else
          {
            rx_state=OUT;
            i=0;
            len=0;
          }
          break;
        }
        
        if(len)
        {
          rx_buf[i++]=temp;
          len--;
        }
        if(len==0)
        {
          //printf("串口modbus响应！\n");
          //uart1_send(rx_buf,3+rx_len);
          memset(rx_buf,0,50);
          rx_state=OUT;
          i=0;
          len=0;
          rx_len=0;
        }
        break;
      }
    default:break;
    }
    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
  }
}