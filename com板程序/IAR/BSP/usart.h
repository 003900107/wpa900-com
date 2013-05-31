/**
  ******************************************************************************
  * @file    USART.h
  * @author  www.armjishu.com
  * @version V1.0
  * @Library Using STM32F10X_STDPERIPH_VERSION V3.3.0
  * @date    04/16/2010
  * @brief   USART program body
  *****************************************************************************/

/* Includes ------------------------------------------------------------------*/

#ifndef EVAL_COM1_STR
  #ifdef STM32F10X_CL
    #define EVAL_COM1_STR                    "USART2"
  #else
    #define EVAL_COM1_STR                    "USARTx"
  #endif
#endif

/* Constants used by Serial Command Line Mode */
#define CMD_STRING_SIZE       128


void USART_COM1_Init(void);
void USART_GetInputString (uint8_t * buffP);
uint32_t USART_Key_Pressed(uint8_t *key);
uint8_t USART_Get_Key(void);
uint8_t USART_Put_Char(uint8_t ch);


void USART_COM2_Init(void);
uint8_t USART2_Put_Char(uint8_t ch);
void USART2_Put_String(uint8_t *Str);
uint32_t USART2_Key_Pressed(uint8_t *key);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2010 www.armjishu.com *****END OF FILE****/
