/*
*********************************************************************************************************
*
*                                               COM Board 
*
*                                               STM32F107
*                                             ucos-II v2.92
*                                              lwip v1.3.1
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

#include <includes.h>
#include "stm32f10x.h"
#include "lwIP.h"
#include "telnet.h"
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
//static   OS_STK      AppTaskRtuClientStk  [APP_TASK_START_STK_SIZE];

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

    /* Disable all interrupts until we are ready to accept them */
    BSP_IntDisAll();
    
    /* Initialize "uC/OS-II, The Real-Time Kernel" */
    OSInit(); 
    
    /* Create the start task */
    OSTaskCreateExt(AppTaskStart,                               
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

    /* Start multitasking (i.e. give control to uC/OS-II) */
    OSStart();                                                  
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This task will initialize hardware 、lwip 、tcp server and create other tasks.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
  (void)p_arg;
  
  /* Init BSP */
  BSP_Init();
  
  /* Init lwip stack 、add network interface */
  lwIP_Init();
  
  /* Init Tcp server */
  Telnet_init();
  
  /* Create application task */
  AppTaskCreate();   
  
  while(DEF_TRUE)
  { 
    OSTimeDlyHMSM(0, 0, 1, 0);
  }
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*
* Description:  This function creates tasks : AppTaskClient 、AppTaskRtuClient.
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



