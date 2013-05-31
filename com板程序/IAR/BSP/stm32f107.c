/**
  ******************************************************************************
  * @file       USART.c 
  * @author  www.armjishu.com 神舟系列
  * @version V1.0
  * @Library Using STM32F10X_STDPERIPH_VERSION V3.3.0
  * @date    04/16/2010
  * @brief   USART program body
  *****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32_eth.h"
#include "stm32f107.h"
#include "stm32_eval.h"
#include "i2c_ee.h"
#include <usart.h>
#include <stdio.h>


//#include "sd.h"
//#include "efs.h"
//#include "ls.h"

//#include "stm32f10x_rtc.h"
//#include "stm32f10x_pwr.h"
//#include "stm32f10x_can.h"
//#include "ARMJISHU_TouchScreen.h"

extern void Delay_ARMJISHU(__IO uint32_t nCount);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DP83848_PHY        /* Ethernet pins mapped on STM3210C-EVAL Board */
#define PHY_ADDRESS       0x00//lihao /* Relative to STM3210C-EVAL Board */

//#define MII_MODE          /* MII mode for STM3210C-EVAL Board (MB784) (check jumpers setting) */
#define RMII_MODE       /* RMII mode for STM3210C-EVAL Board (MB784) (check jumpers setting) */

/*--------------- LCD Messages ---------------*/
#define MESSAGE1   "  www.armjishu.com  "
#define MESSAGE2   "     STM32F107      "
#define MESSAGE3   " Connectivity Line  "
#define MESSAGE4   "   * LwIP demos *   "


/* Private macro -------------------------------------------------------------*/
/* Select MSD Card: ChipSelect pin low  */
//#define MSD_CS_LOW()     GPIO_ResetBits(GPIOD, GPIO_Pin_11)
/* Deselect MSD Card: ChipSelect pin high */
//#define MSD_CS_HIGH()    GPIO_SetBits(GPIOD, GPIO_Pin_11)

/* Private variables ---------------------------------------------------------*/
//EmbeddedFileSystem  efs;
//DirList             list;
//EmbeddedFile        file_SD_Card;

/* Private functions ---------------------------------------------------------*/
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void ADC_Configuration(void);
void Ethernet_Configuration(void);
TestStatus SdCardInit(void);
void SdCardTest(void);
void GPIO_KEY_Config(void);
TestStatus USART_COM_Check(void);

void Time_Display(uint32_t TimeVar);
void  RTC_Test(void);

TestStatus Dual_CAN_Test(void);
/**
  * @brief  Setup STM32 system (clocks, Ethernet, GPIO, NVIC) and STM3210C-EVAL resources.
  * @param  None
  * @retval None
  */
void System_Setup(void)
{
  RCC_ClocksTypeDef RCC_Clocks;

  /* Setup STM32 clock, PLL and Flash configuration) */
  SystemInit();

  /* Enable USART2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);


  /* Enable ETHERNET clock  */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx |
                        RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);

  /* Enable GPIOs and ADC1 clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO |
						 RCC_APB2Periph_ADC1, ENABLE);
  
  /* NVIC configuration */
  NVIC_Configuration();  

  /* ADC configuration */
  ADC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* Configure the BEEP 蜂鸣器 */
  BEEP_Configuration();
  STM_EVAL_BEEPOn();
  
  /* Configure the USART port */  
  USART_COM1_Init();
  
  STM_EVAL_BEEPOff();

  /* Initialize the STM3210C-EVAL's LCD */
  //STM3210C_LCD_Init();

  STM_EVAL_BEEPOff();
      
  /* Initialize STM3210C-EVAL's LEDs */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

  /* Turn on leds available on STM3210X-EVAL */
  STM_EVAL_LEDOn(LED1);
  STM_EVAL_LEDOn(LED2);
  STM_EVAL_LEDOn(LED3);
  STM_EVAL_LEDOn(LED4);

  /* Clear the LCD */
  //LCD_Clear(Blue);

  /* Set the LCD Back Color */
  //LCD_SetBackColor(Blue);

  /* Set the LCD Text Color */
  //LCD_SetTextColor(White);

  /* Display message on the LCD*/
  //LCD_DisplayStringLine(Line0, MESSAGE1);
  //LCD_DisplayStringLine(Line1, MESSAGE2);
  //LCD_DisplayStringLine(Line2, MESSAGE3);
  //LCD_DisplayStringLine(Line3, MESSAGE4);
  //LCD_DisplayWelcomeStr(Line9);  

  //LCD_DisplayStringLine(Line7, "   EEPROM TEST....   ");
  /* EEPROM 24C02 TEST */
  //if(FAILED == ARMJISHU_EEPROM_TEST())
  //{
  //  printf(" --->FAILED!\n\r"); 
  //  LCD_DisplayStringLine(Line7, "EEPROM TEST *FAILED!");
  //}
  //else
  //{
  //  printf(" --->PASSED!\n\r"); 
  //  LCD_DisplayStringLine(Line7, "EEPROM TEST PASSED!!");
  //}
  
  //ADS7843_Init();
  
  /* RTC TEST */
  //RTC_Test();

  //GPIO_KEY_Config();
  
  /* Configure the Ethernet peripheral */
  Ethernet_Configuration();

  /* SystTick configuration: an interrupt every 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.SYSCLK_Frequency / 100);

  /* Update the SysTick IRQ priority should be higher than the Ethernet IRQ */
  /* The Localtime should be updated during the Ethernet packets processing */
  NVIC_SetPriority (SysTick_IRQn, 1);  
  
  /* Configure the Key button */ 
  //STM_EVAL_PBInit(Button_KEY, Mode_GPIO);
}


/**
  * @brief  Configures the Ethernet Interface
  * @param  None
  * @retval None
  */
void Ethernet_Configuration(void)
{
  ETH_InitTypeDef ETH_InitStructure;

  /* MII/RMII Media interface selection ------------------------------------------*/
#ifdef MII_MODE /* Mode MII with STM3210C-EVAL  */
  GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_MII);

  /* Get HSE clock = 25MHz on PA8 pin (MCO) */
  RCC_MCOConfig(RCC_MCO_HSE);

#elif defined RMII_MODE  /* Mode RMII with STM3210C-EVAL */
  GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);

  /* Set PLL3 clock output to 50MHz (25MHz /5 *10 =50MHz) */
  RCC_PLL3Config(RCC_PLL3Mul_10);
  /* Enable PLL3 */
  RCC_PLL3Cmd(ENABLE);
  /* Wait till PLL3 is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET)
  {}

  /* Get PLL3 clock on PA8 pin (MCO) */
  RCC_MCOConfig(RCC_MCO_PLL3CLK);
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
  //ETH_Init(&ETH_InitStructure, PHY_ADDRESS);
  if(1 == ETH_Init(&ETH_InitStructure, PHY_ADDRESS))
    STM_EVAL_LEDOff(LED1);
  else
    STM_EVAL_LEDOff(LED2);
    //printf(" Ethernet_Configuration() ETH_ERROR");
  
  /* Enable the Ethernet Rx Interrupt */
  ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);

}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
#ifdef MII_MODE  //lihao
void GPIO_Configuration(void)
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
  - ETH_MII_PPS_OUT / ETH_RMII_PPS_OUT: PB5
  - ETH_MII_TXD3: PB8 */

  /* Configure PA2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure PC1, PC2 and PC3 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure PB5, PB8, PB11, PB12 and PB13 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_11 |
                                GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /**************************************************************/
  /*               For Remapped Ethernet pins                   */
  /*************************************************************/
  /* Input (Reset Value):
  - ETH_MII_CRS CRS: PA0
  - ETH_MII_RX_CLK / ETH_RMII_REF_CLK: PA1
  - ETH_MII_COL: PA3
  - ETH_MII_RX_DV / ETH_RMII_CRS_DV: PD8
  - ETH_MII_TX_CLK: PC3
  - ETH_MII_RXD0 / ETH_RMII_RXD0: PD9
  - ETH_MII_RXD1 / ETH_RMII_RXD1: PD10
  - ETH_MII_RXD2: PD11
  - ETH_MII_RXD3: PD12
  - ETH_MII_RX_ER: PB10 */

  /* ETHERNET pins remapp in STM3210C-EVAL board: RX_DV and RxD[3:0] */
  GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);

  /* Configure PA0, PA1 and PA3 as input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure PB10 as input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure PC3 as input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure PD8, PD9, PD10, PD11 and PD12 as input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure); /**/

  /* ADC Channel14 config --------------------------------------------------------*/
  /* Relative to STM3210D-EVAL Board   */
  /* Configure PC.04 (ADC Channel14) as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* MCO pin configuration------------------------------------------------- */
  /* Configure MCO (PA8) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

}

#else //RMII_MODE  By ARNJISHU.COM 该RMII模式由ARM技术论坛完成
void GPIO_Configuration(void)
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
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure PB5, PB8, PB11, PB12 and PB13 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /**************************************************************/
  /*               For Remapped Ethernet pins                   */
  /*************************************************************/
  /* Input (Reset Value):
  - ETH_MII_RX_CLK / ETH_RMII_REF_CLK: PA1
  - ETH_MII_RX_DV / ETH_RMII_CRS_DV: PD8
  - ETH_MII_RXD0 / ETH_RMII_RXD0: PD9
  - ETH_MII_RXD1 / ETH_RMII_RXD1: PD10
 */

  /* ETHERNET pins remapp in STM3210C-EVAL board: RX_DV and RxD[3:0] */
  GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);

  /* Configure PA1 as input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure PD8, PD9, PD10, PD11 and PD12 as input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure); /**/

  /* ADC Channel14 config ----------------------------------------*/
  /* Relative to STM3210D-EVAL Board   */
  /* Configure PC.00 (ADC Channel10) as analog input -------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* MCO pin configuration--------------------------------------- */
  /* Configure MCO (PA8) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

}
#endif

/**
  * @brief  Configures the ADC.
  * @param  None
  * @retval None
  */
void ADC_Configuration(void)
{
  ADC_InitTypeDef ADC_InitStructure;

  /* ADC1 Configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel14 configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_13Cycles5);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
  
  /* Enable the Ethernet global Interrupt 使能以太网中断 */
  NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    
  
}

/*******************************************************************************
* Function Name  : GPIO_KEY_Config.
* Description    : Configures the Extension Button.
* Input          : None.
* Output         : None.
* Return         : None.
* Update         ：www.armjishu.com 
*******************************************************************************/
void GPIO_KEY_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure KEY1 Button */
  RCC_APB2PeriphClockCmd(RCC_KEY1, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_KEY1;
  GPIO_Init(GPIO_KEY1_PORT, &GPIO_InitStructure);

  /* Configure KEY2 Button */
  RCC_APB2PeriphClockCmd(RCC_KEY2, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_KEY2;
  GPIO_Init(GPIO_KEY2_PORT, &GPIO_InitStructure);

  /* Configure KEY3 Button */
  RCC_APB2PeriphClockCmd(RCC_KEY3, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_KEY3;
  GPIO_Init(GPIO_KEY3_PORT, &GPIO_InitStructure);  

  /* Configure KEY4 Button */
  RCC_APB2PeriphClockCmd(RCC_KEY4, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_KEY4;
  GPIO_Init(GPIO_KEY4_PORT, &GPIO_InitStructure);

  /* Configure 315M Button */
  RCC_APB2PeriphClockCmd(RCC_315M, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_315M;
  GPIO_Init(GPIO_315M_PORT, &GPIO_InitStructure);

}

/*******************************************************************************
* Function Name  : Check_315M_Wireless
* Description    : Reads key from Check_315M_Wireless.
* Input          : None
* Output         : None
* Return         : Return 1 when data valid else 0
* Update         ：www.armjishu.com 
*******************************************************************************/
u8 Check_315M_Wireless(void)
{
  /* 315M wireless is pressed */
  if(!GPIO_ReadInputDataBit(GPIO_315M_PORT, GPIO_315M))
  {
    return 1; 
  }
  else
  {
    return 0; 
  } 
}
  
/*******************************************************************************
* Function Name  : ReadKeyDown
* Description    : Reads key from demoboard.
* Input          : None
* Output         : None
* Return         : Return KEY1, KEY2, KEY3, KEY4 or NOKEY
* Update         ：www.armjishu.com 
*******************************************************************************/
u8 ReadKeyDown(void)
{
  /* 1 key is pressed */
  if(!GPIO_ReadInputDataBit(GPIO_KEY1_PORT, GPIO_KEY1))
  {
    return KEY1; 
  }	
  /* 2 key is pressed */
  if(!GPIO_ReadInputDataBit(GPIO_KEY2_PORT, GPIO_KEY2))
  {
    return KEY2; 
  }
  /* 3 key is pressed */
  if(!GPIO_ReadInputDataBit(GPIO_KEY3_PORT, GPIO_KEY3))
  {
    return KEY3; 
  }
  /* 4 key is pressed */
  if(!GPIO_ReadInputDataBit(GPIO_KEY4_PORT, GPIO_KEY4))
  {
    return KEY4; 
  }

  /* No key is pressed */
  else 
  {
    return NOKEY;
  }
}

void Key_315M_Wireless_Display(void)
{
    static uint16_t Wireless_Debounce = 0, Key_Debounce = 0;
    __IO uint16_t TextColor, BackColor;
    
    LCD_GetColors(&TextColor, &BackColor);
    /* Set the LCD Text Color */
    LCD_SetTextColor(LCD_COLOR_YELLOW);
    LCD_SetBackColor(LCD_COLOR_RED);
      
    if(Check_315M_Wireless())
    {
        if(1 == Wireless_Debounce)
        {
            Wireless_Debounce++;
            switch(ReadKeyDown())
            {
            case KEY1:
                LCD_DisplayStringLine(Line3, " 315M Wireless Key1 ");
                break;

            case KEY2:
                LCD_DisplayStringLine(Line3, " 315M Wireless Key2 ");
                break;

            case KEY3:
                LCD_DisplayStringLine(Line3, " 315M Wireless Key3 ");
                break;

            case KEY4:
                LCD_DisplayStringLine(Line3, " 315M Wireless Key4 ");
                break;

            default:
                Wireless_Debounce = 0;
            }
        }
        else
        {
            Wireless_Debounce = 1;
        }
    }
    else
    {
        Wireless_Debounce = 0;

        if(NOKEY != ReadKeyDown())
        {
            if(0 == Key_Debounce)
            {
                Key_Debounce= 1;
            }
            else
            {
                if(1 == Key_Debounce)
                {
                    Key_Debounce++;

                    switch(ReadKeyDown())
                    {
                    case KEY1:
                        LCD_DisplayStringLine(Line3, " Key1               ");
                        break;

                    case KEY2:
                        LCD_DisplayStringLine(Line3, "      Key2          ");
                        break;

                    case KEY3:
                        LCD_DisplayStringLine(Line3, "           Key3     ");
                        break;

                    case KEY4:
                        LCD_DisplayStringLine(Line3, "               Key4 ");
                        break;

                    default:
                        Key_Debounce = 0;
                    }
                }
            }
        }
        else
        {
            Key_Debounce = 0;
        }
    }    

    /* Set back the LCD Color */
    LCD_SetTextColor(TextColor);
    LCD_SetBackColor(BackColor);
    
}



void Time_Display(uint32_t TimeVar)
{
  uint32_t THH = 0, TMM = 0, TSS = 0;
  uint8_t  TimeStr[] = "                    "; 

  /* Compute  hours */
  THH = (TimeVar / 3600)%24;
  /* Compute minutes */
  TMM = (TimeVar % 3600) / 60;
  /* Compute seconds */
  TSS = (TimeVar % 3600) % 60;

  //printf("Time: %0.2d:%0.2d:%0.2d\r", THH, TMM, TSS);
  sprintf((char *)TimeStr, "  Time:  %0.2d:%0.2d:%0.2d  ", THH, TMM, TSS);
  LCD_DisplayStringLine(Line5, TimeStr);
}


/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

void Delay_TouchScreen(__IO uint32_t nCount)
{
  for (; nCount != 0; nCount--)
    ARMJISHU_TouchScreen();
}


void  RTC_Test(void)
{
  static uint32_t localtime = 0;

  RTC_Configuration();
    
  //printf(" show Time:\n\r"); 
  localtime = RTC_GetCounter();
  Time_Display(localtime);
  while(RTC_GetCounter() == localtime );

  localtime = RTC_GetCounter();
  Time_Display(localtime);
  while(RTC_GetCounter() == localtime );
  
  localtime = RTC_GetCounter();
  Time_Display(localtime);

  //printf("\n\r");
}
/******************* (C) COPYRIGHT 2010 www.armjishu.com 神舟系列*****END OF FILE****/
