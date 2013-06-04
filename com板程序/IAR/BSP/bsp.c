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

#define  BSP_MODULE
#include "bsp.h"
#include "includes.h"
#include <usart.h>
#include "stm32_eth.h"
#include "enc28j60.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  USART1_232
//#define  USART1_485
#define  USART2_232
//#define  USART2_485
#define  USART3_232
//#define  USART3_485

#define PHY_ADDRESS       0x00
#define RMII_MODE

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/

uint8_t macaddress_2[6] = {0x32,0x12,0x35,0x11,0x01,0x51};


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq_MHz;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void USART_1_Init(void);
void USART_2_Init(void);
void USART_3_Init(void);
void Ethernet_1_Init(void);
void Ethernet_1_GPIOConfig(void);
void Ethernet_1_ETHConfig(void);
void Ethernet_2_Init(void);
void SPI_Initiate(void);
void NVIC_Configuration(void);


/*
*********************************************************************************************************
*                                             REGISTERS
*********************************************************************************************************
*/

#define  DWT_CR      *(CPU_REG32 *)0xE0001000
#define  DWT_CYCCNT  *(CPU_REG32 *)0xE0001004
#define  DEM_CR      *(CPU_REG32 *)0xE000EDFC
#define  DBGMCU_CR   *(CPU_REG32 *)0xE0042004


/*
*********************************************************************************************************
*                                            REGISTER BITS
*********************************************************************************************************
*/

#define  DBGMCU_CR_TRACE_IOEN_MASK       0x10
#define  DBGMCU_CR_TRACE_MODE_ASYNC      0x00
#define  DBGMCU_CR_TRACE_MODE_SYNC_01    0x40
#define  DBGMCU_CR_TRACE_MODE_SYNC_02    0x80
#define  DBGMCU_CR_TRACE_MODE_SYNC_04    0xC0
#define  DBGMCU_CR_TRACE_MODE_MASK       0xC0

#define  DEM_CR_TRCENA                   (1 << 24)

#define  DWT_CR_CYCCNTENA                (1 <<  0)


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

#if ((CPU_CFG_TS_TMR_EN          != DEF_ENABLED) && \
     (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN          >  0u))
#error  "CPU_CFG_TS_EN                  illegally #define'd in 'cpu.h'"
#error  "                              [MUST be  DEF_ENABLED] when    "
#error  "                               using uC/Probe COM modules    "
#endif


/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
   SystemInit();
   
   /* Config systick */
   RCC_ClocksTypeDef RCC_Clocks;
   RCC_GetClocksFreq(&RCC_Clocks);
   SysTick_Config(RCC_Clocks.HCLK_Frequency / (INT32U)OS_TICKS_PER_SEC); /*RCC_Clocks.SYSCLK_Frequency*/
   
   /* Config GPIO clock */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
                          | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
                            | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO
                              ,ENABLE);
   
   /* Config USART1 and USART2 clock */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3,ENABLE);
   
   /* Init USART_1 */
   USART_1_Init();
   
   /* Init USART_2 */
   USART_2_Init();
   
   /* Init USART_3 */
   USART_3_Init();
   
   /* Init Ethernet_1 */
   Ethernet_1_Init();
   
   /* Init Ethernet_2 */
   Ethernet_2_Init();
   
   /* Config NVIC */
   NVIC_Configuration();
}


/*
*********************************************************************************************************
*                                               USART_1_Init()
*
* Description : Initialize USART1.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : 232/485 config.
*********************************************************************************************************
*/

void USART_1_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   
#ifdef USART1_232
   /*PA9 for 232 Tx*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;          
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
   GPIO_Init(GPIOA,&GPIO_InitStructure);
   
   /*PA10 for 232 Rx*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;         
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA,&GPIO_InitStructure);
   
   /*Initiate USART1*/
   USART_InitTypeDef USART_InitStructure;
   USART_InitStructure.USART_BaudRate=9600;
   USART_InitStructure.USART_WordLength=USART_WordLength_8b;
   USART_InitStructure.USART_StopBits=USART_StopBits_1;
   USART_InitStructure.USART_Parity=USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART1,&USART_InitStructure);
   
#else  //USART1_485
   /*PA8 for 485 DIR*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;          
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIO_Init(GPIOA,&GPIO_InitStructure);
   
   /*PA9 for 485 Tx*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;          
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
   GPIO_Init(GPIOA,&GPIO_InitStructure);
   
   /*PA10 for 485 Rx*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;         
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA,&GPIO_InitStructure);
   
   /*Initiate USART1*/
   USART_InitTypeDef USART_InitStructure;
   USART_InitStructure.USART_BaudRate=9600;
   USART_InitStructure.USART_WordLength=USART_WordLength_8b;
   USART_InitStructure.USART_StopBits=USART_StopBits_1;
   USART_InitStructure.USART_Parity=USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART1,&USART_InitStructure);
#endif
   
   /* Ebable Rx interrupt */
   USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
   
   /* Enable USART1 */
   USART_Cmd(USART1,ENABLE);
}


/*
*********************************************************************************************************
*                                               USART_2_Init()
*
* Description : Initialize USART2.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : 232/485 config.
*********************************************************************************************************
*/

void USART_2_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   
   /* Remap USART2 */
   GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
   
#ifdef USART2_232
   /*PD5 for 232 Tx*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;          
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
   GPIO_Init(GPIOD,&GPIO_InitStructure);
   
   /*PD6 for 232 Rx*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;          
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOD,&GPIO_InitStructure);
   
   /*Initiate USART2*/
   USART_InitTypeDef USART_InitStructure;
   USART_InitStructure.USART_BaudRate=9600;
   USART_InitStructure.USART_WordLength=USART_WordLength_8b;
   USART_InitStructure.USART_StopBits=USART_StopBits_1;
   USART_InitStructure.USART_Parity=USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART2,&USART_InitStructure);
   
#else  //USART2_485
   /*PD5 for 485 Tx*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;          
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
   GPIO_Init(GPIOD,&GPIO_InitStructure);
   
   /*PD6 for 485 Rx*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;          
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOD,&GPIO_InitStructure);
   
   /*PD7 for 485 DIR*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;          
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIO_Init(GPIOD,&GPIO_InitStructure);
   
   /*Initiate USART2*/
   USART_InitTypeDef USART_InitStructure;
   USART_InitStructure.USART_BaudRate=9600;
   USART_InitStructure.USART_WordLength=USART_WordLength_8b;
   USART_InitStructure.USART_StopBits=USART_StopBits_1;
   USART_InitStructure.USART_Parity=USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART2,&USART_InitStructure);
   
#endif
   
   /* Enable Rx interrupt */
   USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
   
   /* Enable USART2 */
   USART_Cmd(USART2,ENABLE);
}


/*
*********************************************************************************************************
*                                               USART_3_Init()
*
* Description : Initialize USART3.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : 232/485 config.
*********************************************************************************************************
*/

void USART_3_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   
   /* FullRemap USART3 */
   GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE);
   
#ifdef USART3_232
   /*PD8 for 232 Tx*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;          
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
   GPIO_Init(GPIOD,&GPIO_InitStructure);
   
   /*PD9 for 232 Rx*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;          
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOD,&GPIO_InitStructure);
   
   /*Initiate USART3*/
   USART_InitTypeDef USART_InitStructure;
   USART_InitStructure.USART_BaudRate=9600;
   USART_InitStructure.USART_WordLength=USART_WordLength_8b;
   USART_InitStructure.USART_StopBits=USART_StopBits_1;
   USART_InitStructure.USART_Parity=USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART3,&USART_InitStructure);
   
#else  //USART3_485
   /*PD8 for 485 Tx*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;          
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
   GPIO_Init(GPIOD,&GPIO_InitStructure);
   
   /*PD9 for 485 Rx*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;          
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOD,&GPIO_InitStructure);
   
   /*PD10 for 485 DIR*/
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;          
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIO_Init(GPIOD,&GPIO_InitStructure);
   
   /*Initiate USART3*/
   USART_InitTypeDef USART_InitStructure;
   USART_InitStructure.USART_BaudRate=9600;
   USART_InitStructure.USART_WordLength=USART_WordLength_8b;
   USART_InitStructure.USART_StopBits=USART_StopBits_1;
   USART_InitStructure.USART_Parity=USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART3,&USART_InitStructure);
   
#endif
   
   /* Enable Rx interrupt */
   USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
   
   /* Enable USART3 */
   USART_Cmd(USART3,ENABLE);
}

/*
*********************************************************************************************************
*                                               USART_send()
*
* Description : This function sends array by USARTx.
*
* Argument(s) : USARTx     is one USART for sending array:
*                           
*                          USART1   first USART
*                          USART2   second USART
*                          USART3   third USART
*
*               buf        is a pointer to array.      
*
*               len        is the length of array.
*
* Return(s)   : none.
*
* Note(s)     : none.
*
*********************************************************************************************************
*/

void USART_send(USART_TypeDef* USARTx,uint8_t *buf,uint8_t len)
{
  while(len--)
  {
    /* Transmit Data */
    USART_SendData(USARTx, *buf);
    
    /* loop while transmiting is not finished */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
    {}
    
    /* point to next data */
    buf++;
  }
}


/*
*********************************************************************************************************
*                                               Ethernet_1_Init()
*
* Description : Initialize Ethernet_1.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void Ethernet_1_Init(void)
{
  /* Enable clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx |
                        RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);
   
   
  /* Configure the GPIO ports */
  Ethernet_1_GPIOConfig();
  
  /* Configure the Ethernet peripheral */
  Ethernet_1_ETHConfig();
}


/*
*********************************************************************************************************
*                                            Ethernet_1_GPIOConfig()
*
* Description : This function configure GPIO of Ethernet_1.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : RMII mode between MAC and PHY.
*********************************************************************************************************
*/

void Ethernet_1_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* ETHERNET pins configuration */
  /* AF Output Push Pull:
  - ETH_MII_MDIO / ETH_RMII_MDIO: PA2
  - ETH_MII_MDC / ETH_RMII_MDC: PC1
  - ETH_MII_TXD2: PC2
  - ETH_MII_TX_EN / ETH_RMII_TX_EN: PB11
  - ETH_MII_TXD0 / ETH_RMII_TXD0: PB12
  - ETH_MII_TXD1 / ETH_RMII_TXD1: PB13
 */

  /* Configure PA2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure PC1, PC2 and PC3 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  // | GPIO_Pin_2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure PB5, PB8, PB11, PB12 and PB13 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  
  /* Input (Reset Value):
  - ETH_MII_RX_CLK / ETH_RMII_REF_CLK: PA1
  - ETH_MII_RX_DV / ETH_RMII_CRS_DV: PA7
  - ETH_MII_RXD0 / ETH_RMII_RXD0: PC4
  - ETH_MII_RXD1 / ETH_RMII_RXD1: PC5
 */

  /* Configure PA1¡¢PA7 as input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure PC4¡¢PC5 as input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
}


/*
*********************************************************************************************************
*                                            Ethernet_1_ETHConfig()
*
* Description : This function configure MAC controller module in STM32F107.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void Ethernet_1_ETHConfig(void)
{
  ETH_InitTypeDef ETH_InitStructure;

  /* MII/RMII Media interface selection ------------------------------------------*/
#ifdef MII_MODE /* Mode MII with STM3210C-EVAL  */
  GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_MII);

  /* Get HSE clock = 25MHz on PA8 pin (MCO) */
  RCC_MCOConfig(RCC_MCO_HSE);

#elif defined RMII_MODE  /* Mode RMII with STM3210C-EVAL */
  GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);

#endif

  /* Reset ETHERNET on AHB Bus */
  ETH_DeInit();

  /* Software reset */
  ETH_SoftwareReset();

  /* Wait for software reset */
  while (ETH_GetSoftwareResetStatus() == SET);

  /* ETHERNET Configuration ------------------------------------------------------*/
  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
  ETH_StructInit(&ETH_InitStructure);

  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable  ;
  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

  /*------------------------   DMA   -----------------------------------*/  
  
  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; 
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;         
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;     
 
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;       
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;   
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;                                                          
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;      
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;          
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;                                                                 
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

  /* Configure Ethernet */
  ETH_Init(&ETH_InitStructure, PHY_ADDRESS);
  
  /* Enable the Ethernet Rx Interrupt */
  /* Receive net packets : interrupt or polling .*/
  //ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
}


/*
*********************************************************************************************************
*                                            Ethernet_2_Init()
*
* Description : Initialize Ethernet_2.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void Ethernet_2_Init(void)
{
  /* Initialize SPI */
  SPI_Initiate();
   
  /* Initialize ENC28J60 */
  enc28j60Init(macaddress_2);
}


/*
*********************************************************************************************************
*                                            SPI_Initiate()
*
* Description : Initialize SPI.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void SPI_Initiate(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
  
  /* Enable clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
  
  /*SPI3 GPIO £º*/
  /*PA15£º SPI3_ENC_CS
    PC10£º SPI3_ENC_SCK
    PC11£º SPI3_ENC_MISO
    PC12£º SPI3_ENC_MOSI
    PD0£º  SPI3_WOL
    PD1£º  SPI3_INT  */
  
  /* Disable JTAG to free PA15 */
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
  
  /* PA15 for SPI3_ENC_CS */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_SetBits(GPIOA, GPIO_Pin_15);
  
  /*PC10 PC11 PC12 for SPI3_ENC_SCK¡¢SPI3_ENC_MISO¡¢SPI3_ENC_MOSI*/
  GPIO_PinRemapConfig(GPIO_Remap_SPI3, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /*PD1 for SPI3_INT*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* Interrupt from ENC28J60 */
  //EXTI_InitTypeDef EXTI_InitStructure;

  /* Connect  EXTI1 Line to PD1 */
  //GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource1);  

  /* Configure EXTI1 line */
  //EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  //EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  //EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  //EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  //EXTI_Init(&EXTI_InitStructure);
  
  /* SPI3 param config */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI3, &SPI_InitStructure);
  
  /* Enable SPI3 */
  SPI_Cmd(SPI3, ENABLE); 
}


/*
*********************************************************************************************************
*                                            NVIC_Configuration()
*
* Description : Configure NVIC.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void NVIC_Configuration(void)
{
   NVIC_InitTypeDef   NVIC_InitStructure;
   
   /* Sets the vector table location and Offset */
   NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
   
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
   
   /* Set USART1 interrupt priority */
   //NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
   //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
   //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   //NVIC_Init(&NVIC_InitStructure);
   
   /* Set USART2 interrupt priority */
   //NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
   //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
   //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
   //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   //NVIC_Init(&NVIC_InitStructure);
   
   /* Set ETH_1 interrupt priority */
   NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure); 
   
   /* Set ETH_2 interrupt priority */
   NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure); 
}


/*
*********************************************************************************************************
*                                            SPI3_ReadWrite()
*
* Description : This function reads/writes one byte from/to ENC28J60.
*
* Argument(s) : Data to write.
*
* Return(s)   : Data read from ENC28J60.
*********************************************************************************************************
*/

unsigned char	SPI3_ReadWrite(unsigned char writedat)
{
  /* Loop while DR register in not emplty */
  while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE) == RESET);
	
  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI3, writedat);
	
  /* Wait to receive a byte */
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
	
  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI3);
}

/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;

    RCC_GetClocksFreq(&rcc_clocks);

    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}


/*
*********************************************************************************************************
*                                            PUTCHAR_PROTOTYPE
*
* Description : Retargets the C library printf function to the USART.
*
* Argument(s) : None.
*
* Return(s)   : None.
*********************************************************************************************************
*/

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}

  return ch;
}


/*
*********************************************************************************************************
*                                           OSProbe_TmrInit()
*
* Description : Select & initialize a timer for use with the uC/Probe Plug-In for uC/OS-II.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : OSProbe_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if ((APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN          == 1))
void  OSProbe_TmrInit (void)
{
}
#endif


/*
*********************************************************************************************************
*                                            OSProbe_TmrRd()
*
* Description : Read the current counts of a free running timer.
*
* Argument(s) : none.
*
* Return(s)   : The 32-bit timer counts.
*
* Caller(s)   : OSProbe_TimeGetCycles().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if ((APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN          == 1))
CPU_INT32U  OSProbe_TmrRd (void)
{
    return ((CPU_INT32U)DWT_CYCCNT);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                          CPU_TS_TmrInit()
*
* Description : Initialize & start CPU timestamp timer.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : CPU_TS_Init().
*
*               This function is an INTERNAL CPU module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but MUST NOT be called by application function(s).
*
* Note(s)     : (1) CPU_TS_TmrInit() is an application/BSP function that MUST be defined by the developer
*                   if either of the following CPU features is enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR'
*                       data type.
*
*                       (1) If timer has more bits, truncate timer values' higher-order bits greater
*                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
*
*                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR'
*                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be
*                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
*
*                           In other words, if timer size is not a binary-multiple of 8-bit octets
*                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple
*                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the
*                           minimum supported word size for CPU timestamp timers is 8-bits.
*
*                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
*                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                   (c) When applicable, timer period SHOULD be less than the typical measured time
*                       but MUST be less than the maximum measured time; otherwise, timer resolution
*                       inadequate to measure desired times.
*
*                   See also 'CPU_TS_TmrRd()  Note #2'.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
    CPU_INT32U  cpu_clk_freq_hz;
      
      
    DEM_CR         |= (CPU_INT32U)DEM_CR_TRCENA;                /* Enable Cortex-M3's DWT CYCCNT reg.                   */
    DWT_CYCCNT      = (CPU_INT32U)0u;
    DWT_CR         |= (CPU_INT32U)DWT_CR_CYCCNTENA;

    cpu_clk_freq_hz = BSP_CPU_ClkFreq();    
    CPU_TS_TmrFreqSet(cpu_clk_freq_hz);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                           CPU_TS_TmrRd()
*
* Description : Get current CPU timestamp timer count value.
*
* Argument(s) : none.
*
* Return(s)   : Timestamp timer count (see Notes #2a & #2b).
*
* Caller(s)   : CPU_TS_Init(),
*               CPU_TS_Get32(),
*               CPU_TS_Get64(),
*               CPU_IntDisMeasStart(),
*               CPU_IntDisMeasStop().
*
*               This function is an INTERNAL CPU module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but SHOULD NOT be called by application function(s).
*
* Note(s)     : (1) CPU_TS_TmrRd() is an application/BSP function that MUST be defined by the developer
*                   if either of the following CPU features is enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR'
*                       data type.
*
*                       (1) If timer has more bits, truncate timer values' higher-order bits greater
*                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
*
*                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR'
*                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be
*                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
*
*                           In other words, if timer size is not a binary-multiple of 8-bit octets
*                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple
*                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the
*                           minimum supported word size for CPU timestamp timers is 8-bits.
*
*                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
*                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                       (1) If timer is a 'down' counter whose values decrease with each time count,
*                           then the returned timer value MUST be ones-complemented.
*
*                   (c) (1) When applicable, the amount of time measured by CPU timestamps is
*                           calculated by either of the following equations :
*
*                           (A) Time measured  =  Number timer counts  *  Timer period
*
*                                   where
*
*                                       Number timer counts     Number of timer counts measured
*                                       Timer period            Timer's period in some units of
*                                                                   (fractional) seconds
*                                       Time measured           Amount of time measured, in same
*                                                                   units of (fractional) seconds
*                                                                   as the Timer period
*
*                                                  Number timer counts
*                           (B) Time measured  =  ---------------------
*                                                    Timer frequency
*
*                                   where
*
*                                       Number timer counts     Number of timer counts measured
*                                       Timer frequency         Timer's frequency in some units
*                                                                   of counts per second
*                                       Time measured           Amount of time measured, in seconds
*
*                       (2) Timer period SHOULD be less than the typical measured time but MUST be less
*                           than the maximum measured time; otherwise, timer resolution inadequate to
*                           measure desired times.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    return ((CPU_TS_TMR)DWT_CYCCNT);
}
#endif
