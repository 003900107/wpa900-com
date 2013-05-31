/**
  ******************************************************************************
  * @file    stm3210c_eval.h
  * @author  MCD Application Team
  * @version V3.1.2
  * @date    09/28/2009
  * @brief   This file contains definitions for STM3210C_EVAL's Leds, push-buttons
  *          and COM ports hardware resources.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM3210C_EVAL_H
#define __STM3210C_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM3210C_EVAL
  * @{
  */ 


/** @defgroup STM3210C_EVAL_Exported_Types
  * @{
  */
/**
  * @}
  */ 

/** @defgroup STM3210C_EVAL_Exported_Constants
  * @{
  */ 

/** @addtogroup STM3210C_EVAL_LED
  * @{
  */
#define LEDn                        4
#define LED1_GPIO_PORT              GPIOD
#define LED1_GPIO_CLK               RCC_APB2Periph_GPIOD  
#define LED1_GPIO_PIN               GPIO_Pin_2
  
#define LED2_GPIO_PORT              GPIOD
#define LED2_GPIO_CLK               RCC_APB2Periph_GPIOD  
#define LED2_GPIO_PIN               GPIO_Pin_3
  
#define LED3_GPIO_PORT              GPIOD
#define LED3_GPIO_CLK               RCC_APB2Periph_GPIOD  
#define LED3_GPIO_PIN               GPIO_Pin_4
  
#define LED4_GPIO_PORT              GPIOD
#define LED4_GPIO_CLK               RCC_APB2Periph_GPIOD  
#define LED4_GPIO_PIN               GPIO_Pin_7

/**
  * @}
  */ 
  
/** @addtogroup STM3210C_EVAL_BUTTON
  * @{
  */  
#define BUTTONn                     3 /*!< Joystick pins are connected to an IO Expander (accessible through I2C1 interface) */

/**
 * @brief Wakeup push-button
 */
#define WAKEUP_BUTTON_PORT          GPIOA
#define WAKEUP_BUTTON_CLK           RCC_APB2Periph_GPIOA
#define WAKEUP_BUTTON_PIN           GPIO_Pin_0
#define WAKEUP_BUTTON_EXTI_LINE     EXTI_Line0
#define WAKEUP_BUTTON_PORT_SOURCE   GPIO_PortSourceGPIOA
#define WAKEUP_BUTTON_PIN_SOURCE    GPIO_PinSource0
#define WAKEUP_BUTTON_IRQn          EXTI0_IRQn 

/**
 * @brief Tamper push-button
 */
#define TAMPER_BUTTON_PORT          GPIOC
#define TAMPER_BUTTON_CLK           RCC_APB2Periph_GPIOC
#define TAMPER_BUTTON_PIN           GPIO_Pin_13
#define TAMPER_BUTTON_EXTI_LINE     EXTI_Line13
#define TAMPER_BUTTON_PORT_SOURCE   GPIO_PortSourceGPIOC
#define TAMPER_BUTTON_PIN_SOURCE    GPIO_PinSource13
#define TAMPER_BUTTON_IRQn          EXTI15_10_IRQn 

/**
 * @brief Key push-button
 */
#define KEY_BUTTON_PORT             GPIOB
#define KEY_BUTTON_CLK              RCC_APB2Periph_GPIOB
#define KEY_BUTTON_PIN              GPIO_Pin_10
#define KEY_BUTTON_EXTI_LINE        EXTI_Line10
#define KEY_BUTTON_PORT_SOURCE      GPIO_PortSourceGPIOB
#define KEY_BUTTON_PIN_SOURCE       GPIO_PinSource10
#define KEY_BUTTON_IRQn             EXTI15_10_IRQn
/**
  * @}
  */ 

#define USER_BUTTON_PIN                   GPIO_Pin_4
#define USER_BUTTON_GPIO_PORT             GPIOC
#define USER_BUTTON_GPIO_CLK              RCC_APB2Periph_GPIOC
#define USER_BUTTON_EXTI_LINE             EXTI_Line4
#define USER_BUTTON_EXTI_PORT_SOURCE      GPIO_PortSourceGPIOC
#define USER_BUTTON_EXTI_PIN_SOURCE       GPIO_PinSource4
#define USER_BUTTON_EXTI_IRQn             EXTI4_IRQn  // notice lilhao

   
#define WIRELESS_315M_VT_PIN                   GPIO_Pin_3
#define WIRELESS_315M_VT_PORT                  GPIOA
#define WIRELESS_315M_VT_CLK                   RCC_APB2Periph_GPIOA
#define WIRELESS_315M_VT_EXTI_LINE             EXTI_Line3
#define WIRELESS_315M_VT_EXTI_PORT_SOURCE      GPIO_PortSourceGPIOA
#define WIRELESS_315M_VT_EXTI_PIN_SOURCE       GPIO_PinSource3
#define WIRELESS_315M_VT_EXTI_IRQn             EXTI3_IRQn  // notice lilhao   

#define BEEP_PIN                         WIRELESS_315M_VT_PIN
#define BEEP_PORT                        WIRELESS_315M_VT_PORT
#define BEEP_CLK                         WIRELESS_315M_VT_CLK


/* Exported constants --------------------------------------------------------*/
#define RCC_GPIO_LED                                 RCC_APB2Periph_GPIOD
#define GPIO_LED_PORT                                GPIOD    
#define GPIO_LED1                                    GPIO_Pin_2    
#define GPIO_LED2                                    GPIO_Pin_3    
#define GPIO_LED3                                    GPIO_Pin_4    
#define GPIO_LED4                                    GPIO_Pin_7
#define GPIO_LED_ALL                                 GPIO_LED1 |GPIO_LED2 |GPIO_LED3 |GPIO_LED4 

/* KEY °´¼ü  */
#define RCC_KEY1                                    RCC_APB2Periph_GPIOC
#define GPIO_KEY1_PORT                              GPIOC    
#define GPIO_KEY1                                   GPIO_Pin_4
#define GPIO_KEY1_EXTI_LINE                         EXTI_Line4
#define GPIO_KEY1_EXTI_PORT_SOURCE                  GPIO_PortSourceGPIOC
#define GPIO_KEY1_EXTI_PIN_SOURCE                   GPIO_PinSource4
#define GPIO_KEY1_EXTI_IRQn                         EXTI4_IRQn 

#define RCC_KEY2                                    RCC_APB2Periph_GPIOB
#define GPIO_KEY2_PORT                              GPIOB   
#define GPIO_KEY2                                   GPIO_Pin_10
#define GPIO_KEY2_EXTI_LINE                         EXTI_Line10
#define GPIO_KEY2_EXTI_PORT_SOURCE                  GPIO_PortSourceGPIOB
#define GPIO_KEY2_EXTI_PIN_SOURCE                   GPIO_PinSource10
#define GPIO_KEY2_EXTI_IRQn                         EXTI15_10_IRQn 

#define RCC_KEY3                                    RCC_APB2Periph_GPIOC
#define GPIO_KEY3_PORT                              GPIOC    
#define GPIO_KEY3                                   GPIO_Pin_13 
#define GPIO_KEY3_EXTI_LINE                         EXTI_Line13
#define GPIO_KEY3_EXTI_PORT_SOURCE                  GPIO_PortSourceGPIOC
#define GPIO_KEY3_EXTI_PIN_SOURCE                   GPIO_PinSource13
#define GPIO_KEY3_EXTI_IRQn                         EXTI15_10_IRQn 

#define RCC_KEY4                                    RCC_APB2Periph_GPIOA
#define GPIO_KEY4_PORT                              GPIOA    
#define GPIO_KEY4                                   GPIO_Pin_0 
#define GPIO_KEY4_EXTI_LINE                         EXTI_Line0
#define GPIO_KEY4_EXTI_PORT_SOURCE                  GPIO_PortSourceGPIOA
#define GPIO_KEY4_EXTI_PIN_SOURCE                   GPIO_PinSource0
#define GPIO_KEY4_EXTI_IRQn                         EXTI0_IRQn 


#define RCC_315M                                    RCC_APB2Periph_GPIOA
#define GPIO_315M_PORT                              GPIOA    
#define GPIO_315M                                   GPIO_Pin_3 
#define GPIO_315M_EXTI_LINE                         EXTI_Line3
#define GPIO_315M_EXTI_PORT_SOURCE                  GPIO_PortSourceGPIOA
#define GPIO_315M_EXTI_PIN_SOURCE                   GPIO_PinSource3
#define GPIO_315M_EXTI_IRQn                         EXTI3_IRQn 


#define GPIO_KEY_ANTI_TAMP                          GPIO_KEY3
#define GPIO_KEY_WEAK_UP                            GPIO_KEY4


/* Values magic to the Board keys */
#define  NOKEY  0
#define  KEY1   1
#define  KEY2   2
#define  KEY3   3
#define  KEY4   4
#define  KEY5   5


/** @addtogroup STM3210C_EVAL_COM
  * @{
  */
#define COMn                             2
   
/**
 * @brief Definition for COM port1, connected to USART2 (USART2 pins remapped on GPIOD)
 */ 
#define EVAL_COM1_STR                    "USART2"
#define EVAL_COM1                        USART2
#define EVAL_COM1_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM1_TX_PIN                 GPIO_Pin_5
#define EVAL_COM1_TX_GPIO_PORT           GPIOD
#define EVAL_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOD
#define EVAL_COM1_RX_PIN                 GPIO_Pin_6
#define EVAL_COM1_RX_GPIO_PORT           GPIOD
#define EVAL_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOD
#define EVAL_COM1_IRQn                   USART2_IRQn

   
/**
 * @brief Definition for COM port2, connected to USART1
 */ 
#define EVAL_COM2_STR                    "USART1"
#define EVAL_COM2                        USART1
#define EVAL_COM2_CLK                    RCC_APB2Periph_USART1
#define EVAL_COM2_TX_PIN                 GPIO_Pin_9
#define EVAL_COM2_TX_GPIO_PORT           GPIOA
#define EVAL_COM2_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_RX_PIN                 GPIO_Pin_10
#define EVAL_COM2_RX_GPIO_PORT           GPIOA
#define EVAL_COM2_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_IRQn                   USART1_IRQn

/**
  * @}
  */ 

/**
  * @}
  */ 
  
/** @defgroup STM3210C_EVAL_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM3210C_EVAL_Exported_Functions
  * @{
  */ 
/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM3210C_EVAL_H */
/**
  * @}
  */ 


/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
