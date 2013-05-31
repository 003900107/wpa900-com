/**
  ******************************************************************************
  * @file       USART.c 
  * @author  www.armjishu.com
  * @version V1.0
  * @Library Using STM32F10X_STDPERIPH_VERSION V3.3.0
  * @date    04/16/2010
  * @brief   USART program body
  *****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_eval.h"
#include <stdio.h>
#include <usart.h>

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_Printf
  * @{
  */ 

const uint8_t STM32F10x_STR[] = {"\r\n"\
           "  _____ _______ __  __ ____ ___  ______ __  ___\r\n"\
           " / ____|__   __|  \\/  |___ \\__ \\|  ____/_ |/ _ \\\r\n"\
           "| (___    | |  | \\  / | __) | ) | |__   | | | | |_  __\r\n"\
           " \\___ \\   | |  | |\\/| ||__ < / /|  __|  | | | | \\ \\/ /\r\n"\
           " ____) |  | |  | |  | |___) / /_| |     | | |_| |>  <\r\n"\
           "|_____/   |_|  |_|  |_|____/____|_|     |_|\\___//_/\\_\\"\
           "神舟系列开发板\r\n"\
           "\r\n"};

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef USART_InitStructure;

/* Private function prototypes -----------------------------------------------*/
  
/* Private functions ---------------------------------------------------------*/
void USART_COM1_Init(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
       
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);

  /* Output a message on Hyperterminal using printf function */
  //printf("\n\n\n\rUSART Printf Example: retarget the C library printf function to the USART\n\r");
  //printf("\r\n\n\n\r WWW.ARMJISHU.COM  %s configured....", EVAL_COM1_STR);
  //printf("\n\r ############ WWW.ARMJISHU.COM! ############ ("__DATE__ " - " __TIME__ ")");

  //printf("%s", STM32F10x_STR);

  //printf("\n\r WWW.ARMJISHU.COM use __STM32F10X_STDPERIPH_VERSION %d.%d.%d",
	//		__STM32F10X_STDPERIPH_VERSION_MAIN,
	//		__STM32F10X_STDPERIPH_VERSION_SUB1,
	//		__STM32F10X_STDPERIPH_VERSION_SUB2);
  //printf("\n\r 产品内部Flash大小为：%dK字节！ \t www.armjishu.com",
  //          *(__IO uint16_t*)(0x1FFFF7E0));
#if(__STM32F10X_STDPERIPH_VERSION >= 0x00030300)
  //printf("\n\r 系统内核时钟频率(SystemCoreClock)为：%dHz.\n\r",
  //          SystemCoreClock);
#else
  //printf("\n\r 系统内核时钟频率(SystemCoreClock)为：%dHz.\n\r",
            SystemFrequency);
#endif
  
}



/**
  * @brief  Get Input string from the HyperTerminal
  * @param  buffP: The input string
  * @retval None
  */
void USART_GetInputString (uint8_t * buffP)
{
  uint32_t bytes_read = 0;
  uint8_t c = 0;
  do
  {
    c = GetKey();
    if (c == '\r')
      break;
    if (c == '\b') /* Backspace */
    {
      if (bytes_read > 0)
      {
        //printf("\b \b");
        bytes_read --;
      }
      continue;
    }
    if (bytes_read >= (CMD_STRING_SIZE))
    {
      //printf("Command string size overflow\r\n");
      bytes_read = 0;
      continue;
    }
    if (c >= 0x20 && c <= 0x7E)
    {
      buffP[bytes_read++] = c;
      SerialPutChar(c);
    }
  }
  while (1);
  //printf(("\n\r"));
  buffP[bytes_read] = '\0';
}


/**
  * @brief  Test to see if a key has been pressed on the HyperTerminal
  * @param  key: The key pressed
  * @retval 1: Correct
  *         0: Error
  */
uint32_t USART_Key_Pressed(uint8_t *key)
{

  if ( USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) != RESET)
  {
    *key = (uint8_t)EVAL_COM1->DR;
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  Get a key from the HyperTerminal
  * @param  None
  * @retval The Key Pressed
  */
uint8_t USART_Get_Key(void)
{
  uint8_t key = 0;

  /* Waiting for user input */
  while (1)
  {
    if (USART_Key_Pressed((uint8_t*)&key)) break;
  }
  return key;

}

uint8_t USART_Put_Char(uint8_t ch)
{
  /* Place your implementation of fputc here */
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {}
  
  /* write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  return ch;
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  //printf("\n\r !!! Wrong parameter value detected\n");
  //printf("\n\r - file %s", file);
  //printf("\n\r - line %lu", line);

#if 0
  /* Infinite loop */
  while (1)
  {
  }
#endif
}
#endif

/**
  * @}
  */ 


void USART_COM2_Init(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
       
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM2, &USART_InitStructure);

  /* Output a message on Hyperterminal using printf function */
  USART2_Put_String("\r\n WWW.ARMJISHU.COM  USART1 configured....");  
}


uint8_t USART2_Put_Char(uint8_t ch)
{
  /* Place your implementation of fputc here */
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM2, USART_FLAG_TC) == RESET)
  {}
  
  /* write a character to the USART */
  USART_SendData(EVAL_COM2, (uint8_t) ch);

  return ch;
}

void USART2_Put_String(uint8_t *Str)
{

   while(*Str)
   {
        USART2_Put_Char(*Str++);
   }

}

/**
  * @brief  Test to see if a key has been pressed on the HyperTerminal
  * @param  key: The key pressed
  * @retval 1: Correct
  *         0: Error
  */
uint32_t USART2_Key_Pressed(uint8_t *key)
{

  if ( USART_GetFlagStatus(EVAL_COM2, USART_FLAG_RXNE) != RESET)
  {
    *key = (uint8_t)EVAL_COM2->DR;
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2010 www.armjishu.com 神舟系列*****END OF FILE****/
