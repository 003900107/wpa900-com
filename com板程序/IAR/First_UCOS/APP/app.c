/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>
#include "stm32f10x.h"
#include "lwIP.h"
#include "helloworld.h"

#include "TcpClient.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static   OS_STK      AppTaskStartStk      [APP_TASK_START_STK_SIZE];
static   OS_STK      AppTaskClientStk     [APP_TASK_START_STK_SIZE];
static   OS_STK      AppTaskRtuClientStk  [APP_TASK_START_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void    AppTaskStart      (void *p_arg);
static  void    AppTaskCreate();

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
    CPU_INT08U  err;


    BSP_IntDisAll();                                            /* Disable all interrupts until we are ready to accept them */

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"              */

    OSTaskCreateExt(AppTaskStart,                               /* Create the start task                                    */
                    (void *)0,
                    (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1],
                    APP_TASK_START_PRIO,
                    APP_TASK_START_PRIO,
                    (OS_STK *)&AppTaskStartStk[0],
                    APP_TASK_START_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_TASK_START_PRIO, "Start Task", &err);
#endif

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
  (void)p_arg;
  
  /*system initiate*/
  BSP_Init();
  
  /* Initilaize the LwIP satck */
  Init_lwIP();
  
  /* Initilaize the HelloWorld module */
  HelloWorld_init();
  
  /* Create application task */
  AppTaskCreate();   
  
  while(DEF_TRUE)
  {
    //Display_IPAddress();
    
    //USART_SendData(USART1,'a');
    //USART_SendData(USART2,'b');
     
    OSTimeDlyHMSM(0, 0, 1, 0);
  }
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*
* Description:  This function creates the application tasks.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static void AppTaskCreate()
{
  /* Create tcp client task : AppTaskClient */
  OSTaskCreate(AppTaskClient,                                   
               (void *)0,
               (OS_STK *)&AppTaskClientStk[APP_TASK_START_STK_SIZE - 1],
               APP_TASK_CLIENT_PRIO);
  
  /* Create USART task : AppTaskRtuClient */
  //OSTaskCreate(AppTaskRtuClient,                              
  //             (void *)0,
  //             (OS_STK *)&AppTaskRtuClientStk[APP_TASK_CLIENT_STK_SIZE - 1],
  //             APP_TASK_RTU_CLIENT_PRIO);
}



