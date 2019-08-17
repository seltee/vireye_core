// Definitions
#include <stdint.h>
#define __IO 		 volatile
#define __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define __OM     volatile            /*! Defines 'write only' structure member permissions */
#define __IOM    volatile            /*! Defines 'read / write' structure member permissions */

typedef struct
{
  __IO uint16_t CR1;
  uint16_t  RESERVED0;
  __IO uint16_t CR2;
  uint16_t  RESERVED1;
  __IO uint16_t SR;
  uint16_t  RESERVED2;
  __IO uint16_t DR;
  uint16_t  RESERVED3;
  __IO uint16_t CRCPR;
  uint16_t  RESERVED4;
  __IO uint16_t RXCRCR;
  uint16_t  RESERVED5;
  __IO uint16_t TXCRCR;
  uint16_t  RESERVED6;
  __IO uint16_t I2SCFGR;
  uint16_t  RESERVED7;
  __IO uint16_t I2SPR;
  uint16_t  RESERVED8;  
} SPI_Def;

typedef struct
{
  __IO uint32_t CRL;
  __IO uint32_t CRH;
  __IO uint32_t IDR;
  __IO uint32_t ODR;
  __IO uint32_t BSRR;
  __IO uint32_t BRR;
  __IO uint32_t LCKR;
} GPIO_Def;

typedef struct
{
  __IO uint32_t SR;
  __IO uint32_t CR1;
  __IO uint32_t CR2;
  __IO uint32_t SMPR1;
  __IO uint32_t SMPR2;
  __IO uint32_t JOFR1;
  __IO uint32_t JOFR2;
  __IO uint32_t JOFR3;
  __IO uint32_t JOFR4;
  __IO uint32_t HTR;
  __IO uint32_t LTR;
  __IO uint32_t SQR1;
  __IO uint32_t SQR2;
  __IO uint32_t SQR3;
  __IO uint32_t JSQR;
  __IO uint32_t JDR1;
  __IO uint32_t JDR2;
  __IO uint32_t JDR3;
  __IO uint32_t JDR4;
  __IO uint32_t DR;
} ADC_Def;

typedef struct
{
  __IO uint32_t CR;
  __IO uint32_t CFGR;
  __IO uint32_t CIR;
  __IO uint32_t APB2RSTR;
  __IO uint32_t APB1RSTR;
  __IO uint32_t AHBENR;
  __IO uint32_t APB2ENR;
  __IO uint32_t APB1ENR;
  __IO uint32_t BDCR;
  __IO uint32_t CSR;
} RCC_Def;

typedef struct
{
  __IO uint16_t CR1;
  uint16_t  RESERVED0;
  __IO uint16_t CR2;
  uint16_t  RESERVED1;
  __IO uint16_t SMCR;
  uint16_t  RESERVED2;
  __IO uint16_t DIER;
  uint16_t  RESERVED3;
  __IO uint16_t SR;
  uint16_t  RESERVED4;
  __IO uint16_t EGR;
  uint16_t  RESERVED5;
  __IO uint16_t CCMR1;
  uint16_t  RESERVED6;
  __IO uint16_t CCMR2;
  uint16_t  RESERVED7;
  __IO uint16_t CCER;
  uint16_t  RESERVED8;
  __IO uint16_t CNT;
  uint16_t  RESERVED9;
  __IO uint16_t PSC;
  uint16_t  RESERVED10;
  __IO uint16_t ARR;
  uint16_t  RESERVED11;
  __IO uint16_t RCR;
  uint16_t  RESERVED12;
  __IO uint16_t CCR1;
  uint16_t  RESERVED13;
  __IO uint16_t CCR2;
  uint16_t  RESERVED14;
  __IO uint16_t CCR3;
  uint16_t  RESERVED15;
  __IO uint16_t CCR4;
  uint16_t  RESERVED16;
  __IO uint16_t BDTR;
  uint16_t  RESERVED17;
  __IO uint16_t DCR;
  uint16_t  RESERVED18;
  __IO uint16_t DMAR;
  uint16_t  RESERVED19;
} TIM_Def;

typedef struct
{
  __IOM uint32_t ISER[8U];               /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register */
        uint32_t RESERVED0[24U];
  __IOM uint32_t ICER[8U];               /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register */
        uint32_t RESERVED1[24U];
  __IOM uint32_t ISPR[8U];               /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register */
        uint32_t RESERVED2[24U];
  __IOM uint32_t ICPR[8U];               /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register */
        uint32_t RESERVED3[24U];
  __IOM uint32_t IABR[8U];               /*!< Offset: 0x200 (R/W)  Interrupt Active bit Register */
        uint32_t RESERVED4[56U];
  __IOM uint8_t  IP[240U];               /*!< Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide) */
        uint32_t RESERVED5[644U];
  __OM  uint32_t STIR;                   /*!< Offset: 0xE00 ( /W)  Software Trigger Interrupt Register */
} NVIC_Def;

#define PERIPH_BASE           ((uint32_t)0x40000000) // base peripheral address

#define APB1PERIPH_BASE	PERIPH_BASE
#define APB2PERIPH_BASE	(PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE  (PERIPH_BASE + 0x20000)
#define SCS_BASE        (0xE000E000UL) 

#define H_RCC ((RCC_Def *)(AHBPERIPH_BASE + 0x1000))

#define H_SPI1 ((SPI_Def *)(APB2PERIPH_BASE + 0x3000))
#define H_SPI2 ((SPI_Def *)(APB1PERIPH_BASE + 0x3800))

#define H_ADC1             ((ADC_Def *)(APB2PERIPH_BASE + 0x2400))
#define H_ADC2             ((ADC_Def *)(APB2PERIPH_BASE + 0x2800))

#define H_GPIOA            ((GPIO_Def *)(APB2PERIPH_BASE + 0x0800))
#define H_GPIOB            ((GPIO_Def *)(APB2PERIPH_BASE + 0x0C00))
#define H_GPIOC            ((GPIO_Def *)(APB2PERIPH_BASE + 0x1000))

#define H_TIM2                ((TIM_Def *) (APB1PERIPH_BASE + 0x0000))
#define H_TIM3                ((TIM_Def *) (APB1PERIPH_BASE + 0x0400))
#define H_TIM4                ((TIM_Def *) (APB1PERIPH_BASE + 0x0800))
#define H_TIM5                ((TIM_Def *) (APB1PERIPH_BASE + 0x0C00))
#define H_TIM6                ((TIM_Def *) (APB1PERIPH_BASE + 0x1000))
#define H_TIM7                ((TIM_Def *) (APB1PERIPH_BASE + 0x1400))
#define H_TIM12               ((TIM_Def *) (APB1PERIPH_BASE + 0x1800))
#define H_TIM13               ((TIM_Def *) (APB1PERIPH_BASE + 0x1C00))
#define H_TIM14               ((TIM_Def *) (APB1PERIPH_BASE + 0x2000))

#define H_NVIC                ((NVIC_Def *) (SCS_BASE +  0x0100UL))

// RCC definitions
#define RCC_APB2Periph_AFIO              ((uint32_t)0x00000001)
#define RCC_APB2Periph_GPIOA             ((uint32_t)0x00000004)
#define RCC_APB2Periph_GPIOB             ((uint32_t)0x00000008)
#define RCC_APB2Periph_GPIOC             ((uint32_t)0x00000010)
#define RCC_APB2Periph_GPIOD             ((uint32_t)0x00000020)
#define RCC_APB2Periph_GPIOE             ((uint32_t)0x00000040)
#define RCC_APB2Periph_GPIOF             ((uint32_t)0x00000080)
#define RCC_APB2Periph_GPIOG             ((uint32_t)0x00000100)
#define RCC_APB2Periph_ADC1              ((uint32_t)0x00000200)
#define RCC_APB2Periph_ADC2              ((uint32_t)0x00000400)
#define RCC_APB2Periph_TIM1              ((uint32_t)0x00000800)
#define RCC_APB2Periph_SPI1              ((uint32_t)0x00001000)
#define RCC_APB2Periph_TIM8              ((uint32_t)0x00002000)
#define RCC_APB2Periph_USART1            ((uint32_t)0x00004000)
#define RCC_APB2Periph_ADC3              ((uint32_t)0x00008000)
#define RCC_APB2Periph_TIM15             ((uint32_t)0x00010000)
#define RCC_APB2Periph_TIM16             ((uint32_t)0x00020000)
#define RCC_APB2Periph_TIM17             ((uint32_t)0x00040000)
#define RCC_APB2Periph_TIM9              ((uint32_t)0x00080000)
#define RCC_APB2Periph_TIM10             ((uint32_t)0x00100000)
#define RCC_APB2Periph_TIM11             ((uint32_t)0x00200000)

#define RCC_APB1Periph_TIM2              ((uint32_t)0x00000001)
#define RCC_APB1Periph_TIM3              ((uint32_t)0x00000002)
#define RCC_APB1Periph_TIM4              ((uint32_t)0x00000004)
#define RCC_APB1Periph_TIM5              ((uint32_t)0x00000008)
#define RCC_APB1Periph_TIM6              ((uint32_t)0x00000010)
#define RCC_APB1Periph_TIM7              ((uint32_t)0x00000020)
#define RCC_APB1Periph_TIM12             ((uint32_t)0x00000040)
#define RCC_APB1Periph_TIM13             ((uint32_t)0x00000080)
#define RCC_APB1Periph_TIM14             ((uint32_t)0x00000100)
#define RCC_APB1Periph_WWDG              ((uint32_t)0x00000800)
#define RCC_APB1Periph_SPI2              ((uint32_t)0x00004000)
#define RCC_APB1Periph_SPI3              ((uint32_t)0x00008000)
#define RCC_APB1Periph_USART2            ((uint32_t)0x00020000)
#define RCC_APB1Periph_USART3            ((uint32_t)0x00040000)
#define RCC_APB1Periph_UART4             ((uint32_t)0x00080000)
#define RCC_APB1Periph_UART5             ((uint32_t)0x00100000)
#define RCC_APB1Periph_I2C1              ((uint32_t)0x00200000)
#define RCC_APB1Periph_I2C2              ((uint32_t)0x00400000)
#define RCC_APB1Periph_USB               ((uint32_t)0x00800000)
#define RCC_APB1Periph_CAN1              ((uint32_t)0x02000000)
#define RCC_APB1Periph_CAN2              ((uint32_t)0x04000000)
#define RCC_APB1Periph_BKP               ((uint32_t)0x08000000)
#define RCC_APB1Periph_PWR               ((uint32_t)0x10000000)
#define RCC_APB1Periph_DAC               ((uint32_t)0x20000000)
#define RCC_APB1Periph_CEC               ((uint32_t)0x40000000)

#define RCC_AHBPeriph_DMA1               ((uint32_t)0x00000001)
#define RCC_AHBPeriph_DMA2               ((uint32_t)0x00000002)
#define RCC_AHBPeriph_SRAM               ((uint32_t)0x00000004)
#define RCC_AHBPeriph_FLITF              ((uint32_t)0x00000010)
#define RCC_AHBPeriph_CRC                ((uint32_t)0x00000040)

// Spi bit definitions
#define SPI_CR1_CPHA_Pos                    (0U)
#define SPI_CR1_CPOL_Pos                    (1U)
#define SPI_CR1_MSTR_Pos                    (2U)
#define SPI_CR1_BR_Pos                      (3U)
#define SPI_CR1_SPE_Pos                     (6U)
#define SPI_CR1_LSBFIRST_Pos                (7U)
#define SPI_CR1_SSI_Pos                     (8U)
#define SPI_CR1_SSM_Pos                     (9U)
#define SPI_CR1_RXONLY_Pos                  (10U)
#define SPI_CR1_DFF_Pos                     (11U)
#define SPI_CR1_CRCNEXT_Pos                 (12U)
#define SPI_CR1_CRCEN_Pos                   (13U)
#define SPI_CR1_BIDIOE_Pos                  (14U)
#define SPI_CR1_BIDIMODE_Pos                (15U)

// Gpio pins
#define GPIO_PIN_0_Pos	(0U)
#define GPIO_PIN_1_Pos	(4U)
#define GPIO_PIN_2_Pos	(8U)
#define GPIO_PIN_3_Pos	(12U)
#define GPIO_PIN_4_Pos	(16U)
#define GPIO_PIN_5_Pos	(20U)
#define GPIO_PIN_6_Pos	(24U)
#define GPIO_PIN_7_Pos	(28U)
#define GPIO_PIN_8_Pos	(0U)
#define GPIO_PIN_9_Pos	(4U)
#define GPIO_PIN_10_Pos	(8U)
#define GPIO_PIN_11_Pos	(12U)
#define GPIO_PIN_12_Pos	(16U)
#define GPIO_PIN_13_Pos	(20U)
#define GPIO_PIN_14_Pos	(24U)
#define GPIO_PIN_15_Pos	(28U)

#define GPIO_Pin_0                 ((uint16_t)0x0001)  /*!< Pin 0 selected */
#define GPIO_Pin_1                 ((uint16_t)0x0002)  /*!< Pin 1 selected */
#define GPIO_Pin_2                 ((uint16_t)0x0004)  /*!< Pin 2 selected */
#define GPIO_Pin_3                 ((uint16_t)0x0008)  /*!< Pin 3 selected */
#define GPIO_Pin_4                 ((uint16_t)0x0010)  /*!< Pin 4 selected */
#define GPIO_Pin_5                 ((uint16_t)0x0020)  /*!< Pin 5 selected */
#define GPIO_Pin_6                 ((uint16_t)0x0040)  /*!< Pin 6 selected */
#define GPIO_Pin_7                 ((uint16_t)0x0080)  /*!< Pin 7 selected */
#define GPIO_Pin_8                 ((uint16_t)0x0100)  /*!< Pin 8 selected */
#define GPIO_Pin_9                 ((uint16_t)0x0200)  /*!< Pin 9 selected */
#define GPIO_Pin_10                ((uint16_t)0x0400)  /*!< Pin 10 selected */
#define GPIO_Pin_11                ((uint16_t)0x0800)  /*!< Pin 11 selected */
#define GPIO_Pin_12                ((uint16_t)0x1000)  /*!< Pin 12 selected */
#define GPIO_Pin_13                ((uint16_t)0x2000)  /*!< Pin 13 selected */
#define GPIO_Pin_14                ((uint16_t)0x4000)  /*!< Pin 14 selected */
#define GPIO_Pin_15                ((uint16_t)0x8000)  /*!< Pin 15 selected */
#define GPIO_Pin_All               ((uint16_t)0xFFFF)  /*!< All pins selected */

// ADC
#define ADC_InjectedChannel_1               ((uint8_t)0x14)
#define ADC_InjectedChannel_2               ((uint8_t)0x18)

#define RCC_CFGR_ADCPRE                     ((uint32_t)0x0000C000)
#define RCC_CFGR_ADCPRE_DIV2                ((uint32_t)0x00000000)
#define RCC_CFGR_ADCPRE_DIV4                ((uint32_t)0x00004000)
#define RCC_CFGR_ADCPRE_DIV6                ((uint32_t)0x00008000)
#define RCC_CFGR_ADCPRE_DIV8                ((uint32_t)0x0000C000)

#define ADC_SampleTime_1Cycles5                    ((uint8_t)0x00)
#define ADC_SampleTime_7Cycles5                    ((uint8_t)0x01)
#define ADC_SampleTime_13Cycles5                   ((uint8_t)0x02)
#define ADC_SampleTime_28Cycles5                   ((uint8_t)0x03)
#define ADC_SampleTime_41Cycles5                   ((uint8_t)0x04)
#define ADC_SampleTime_55Cycles5                   ((uint8_t)0x05)
#define ADC_SampleTime_71Cycles5                   ((uint8_t)0x06)
#define ADC_SampleTime_239Cycles5                  ((uint8_t)0x07)

#define ADC_ExternalTrigInjecConv_T2_TRGO           ((uint32_t)0x00002000) /*!< For ADC1 and ADC2 */
#define ADC_ExternalTrigInjecConv_T2_CC1            ((uint32_t)0x00003000) /*!< For ADC1 and ADC2 */
#define ADC_ExternalTrigInjecConv_T3_CC4            ((uint32_t)0x00004000) /*!< For ADC1 and ADC2 */
#define ADC_ExternalTrigInjecConv_T4_TRGO           ((uint32_t)0x00005000) /*!< For ADC1 and ADC2 */
#define ADC_ExternalTrigInjecConv_Ext_IT15_TIM8_CC4 ((uint32_t)0x00006000) /*!< For ADC1 and ADC2 */

#define ADC_ExternalTrigInjecConv_T1_TRGO           ((uint32_t)0x00000000) /*!< For ADC1, ADC2 and ADC3 */
#define ADC_ExternalTrigInjecConv_T1_CC4            ((uint32_t)0x00001000) /*!< For ADC1, ADC2 and ADC3 */
#define ADC_ExternalTrigInjecConv_None              ((uint32_t)0x00007000) /*!< For ADC1, ADC2 and ADC3 */

#define ADC_ExternalTrigInjecConv_T4_CC3            ((uint32_t)0x00002000) /*!< For ADC3 only */
#define ADC_ExternalTrigInjecConv_T8_CC2            ((uint32_t)0x00003000) /*!< For ADC3 only */
#define ADC_ExternalTrigInjecConv_T8_CC4            ((uint32_t)0x00004000) /*!< For ADC3 only */
#define ADC_ExternalTrigInjecConv_T5_TRGO           ((uint32_t)0x00005000) /*!< For ADC3 only */
#define ADC_ExternalTrigInjecConv_T5_CC4            ((uint32_t)0x00006000) /*!< For ADC3 only */

#define ADC_CR2_ADON                        ((uint32_t)0x00000001)        /*!< A/D Converter ON / OFF */
#define ADC_CR2_CONT                        ((uint32_t)0x00000002)        /*!< Continuous Conversion */
#define ADC_CR2_CAL                         ((uint32_t)0x00000004)        /*!< A/D Calibration */
#define ADC_CR2_RSTCAL                      ((uint32_t)0x00000008)        /*!< Reset Calibration */
#define ADC_CR2_DMA                         ((uint32_t)0x00000100)        /*!< Direct Memory access mode */
#define ADC_CR2_ALIGN                       ((uint32_t)0x00000800)        /*!< Data Alignment */

#define ADC_CR2_JEXTSEL                     ((uint32_t)0x00007000)        /*!< JEXTSEL[2:0] bits (External event select for injected group) */
#define ADC_CR2_JEXTSEL_0                   ((uint32_t)0x00001000)        /*!< Bit 0 */
#define ADC_CR2_JEXTSEL_1                   ((uint32_t)0x00002000)        /*!< Bit 1 */
#define ADC_CR2_JEXTSEL_2                   ((uint32_t)0x00004000)        /*!< Bit 2 */

#define ADC_CR2_JEXTTRIG                    ((uint32_t)0x00008000)        /*!< External Trigger Conversion mode for injected channels */

#define ADC_CR2_EXTSEL                      ((uint32_t)0x000E0000)        /*!< EXTSEL[2:0] bits (External Event Select for regular group) */
#define ADC_CR2_EXTSEL_0                    ((uint32_t)0x00020000)        /*!< Bit 0 */
#define ADC_CR2_EXTSEL_1                    ((uint32_t)0x00040000)        /*!< Bit 1 */
#define ADC_CR2_EXTSEL_2                    ((uint32_t)0x00080000)        /*!< Bit 2 */

#define ADC_CR2_EXTTRIG                     ((uint32_t)0x00100000)        /*!< External Trigger Conversion mode for regular channels */
#define ADC_CR2_JSWSTART                    ((uint32_t)0x00200000)        /*!< Start Conversion of injected channels */
#define ADC_CR2_SWSTART                     ((uint32_t)0x00400000)        /*!< Start Conversion of regular channels */
#define ADC_CR2_TSVREFE                     ((uint32_t)0x00800000)        /*!< Temperature Sensor and VREFINT Enable */

#define ADC_CR1_AWDCH                       ((uint32_t)0x0000001F)        /*!< AWDCH[4:0] bits (Analog watchdog channel select bits) */
#define ADC_CR1_AWDCH_0                     ((uint32_t)0x00000001)        /*!< Bit 0 */
#define ADC_CR1_AWDCH_1                     ((uint32_t)0x00000002)        /*!< Bit 1 */
#define ADC_CR1_AWDCH_2                     ((uint32_t)0x00000004)        /*!< Bit 2 */
#define ADC_CR1_AWDCH_3                     ((uint32_t)0x00000008)        /*!< Bit 3 */
#define ADC_CR1_AWDCH_4                     ((uint32_t)0x00000010)        /*!< Bit 4 */

#define ADC_CR1_EOCIE                       ((uint32_t)0x00000020)        /*!< Interrupt enable for EOC */
#define ADC_CR1_AWDIE                       ((uint32_t)0x00000040)        /*!< Analog Watchdog interrupt enable */
#define ADC_CR1_JEOCIE                      ((uint32_t)0x00000080)        /*!< Interrupt enable for injected channels */
#define ADC_CR1_SCAN                        ((uint32_t)0x00000100)        /*!< Scan mode */
#define ADC_CR1_AWDSGL                      ((uint32_t)0x00000200)        /*!< Enable the watchdog on a single channel in scan mode */
#define ADC_CR1_JAUTO                       ((uint32_t)0x00000400)        /*!< Automatic injected group conversion */
#define ADC_CR1_DISCEN                      ((uint32_t)0x00000800)        /*!< Discontinuous mode on regular channels */
#define ADC_CR1_JDISCEN                     ((uint32_t)0x00001000)        /*!< Discontinuous mode on injected channels */

#define ADC_CR1_DISCNUM                     ((uint32_t)0x0000E000)        /*!< DISCNUM[2:0] bits (Discontinuous mode channel count) */
#define ADC_CR1_DISCNUM_0                   ((uint32_t)0x00002000)        /*!< Bit 0 */
#define ADC_CR1_DISCNUM_1                   ((uint32_t)0x00004000)        /*!< Bit 1 */
#define ADC_CR1_DISCNUM_2                   ((uint32_t)0x00008000)        /*!< Bit 2 */

#define ADC_CR1_DUALMOD                     ((uint32_t)0x000F0000)        /*!< DUALMOD[3:0] bits (Dual mode selection) */
#define ADC_CR1_DUALMOD_0                   ((uint32_t)0x00010000)        /*!< Bit 0 */
#define ADC_CR1_DUALMOD_1                   ((uint32_t)0x00020000)        /*!< Bit 1 */
#define ADC_CR1_DUALMOD_2                   ((uint32_t)0x00040000)        /*!< Bit 2 */
#define ADC_CR1_DUALMOD_3                   ((uint32_t)0x00080000)        /*!< Bit 3 */

#define ADC_CR1_JAWDEN                      ((uint32_t)0x00400000)        /*!< Analog watchdog enable on injected channels */
#define ADC_CR1_AWDEN                       ((uint32_t)0x00800000)        /*!< Analog watchdog enable on regular channels */

// Timer
#define TIM_DIER_UIE                        ((uint16_t)0x0001)            /*!< Update interrupt enable */
#define TIM_CR1_CEN                         ((uint16_t)0x0001)
#define TIM_SR_UIF                          ((uint16_t)0x0001) 

#define TIM2_IRQn														28
#define TIM3_IRQn														29
