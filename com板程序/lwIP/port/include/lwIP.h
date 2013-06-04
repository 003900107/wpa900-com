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


#ifndef __LW_IP_H
#define __LW_IP_H

#ifdef __cplusplus
extern "C" { /* Make sure we have C-declarations in C++ programs */
#endif


/* Includes ------------------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/


/* Exported macro ------------------------------------------------------------*/


#ifndef __LW_IP_C
/* Exported variables --------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */
void lwIP_Init(void);

#endif /* !defined(__LW_IP_C) */


/*
#error section
-- The standard C preprocessor directive #error should be used to notify the
programmer when #define constants or macros are not present and to indicate
that a #define value is out of range. These statements are normally found in
a module��s .H file. The #error directive will display the message within the
double quotes when the condition is not met.
*/


#ifdef __cplusplus
}
#endif


#endif /* #ifndef __LW_IP_H */
/*-- File end --*/

