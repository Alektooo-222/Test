/**
 ******************************************************************************
 * @file    USB_Device/CDC_Standalone/Src/main.c
 * @author  MCD Application Team
 * @brief   USB device CDC application main file.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2016 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------ */
#include "main.h"
#include "application.h"
#include "tim.h"
#include "etl/map.h"

extern uint8_t UserRxBufferUSB[APP_TX_DATA_SIZE];
extern uint8_t UserRxBufferUART[APP_TX_DATA_SIZE];
extern uint32_t UserTxBufPtrIn;
extern uint8_t RxMessFlag_USB;
extern uint8_t RxMessFlag_UART;

extern UART_HandleTypeDef UartHandle;
extern TIM_HandleTypeDef TimHandle;

extern etl::map<TIM_TypeDef *, TIM_HandleTypeDef *, 12> tim_handl_table;

void TIM_SW_PWM_Init();
void TIM_SW_MEASURE_Init();
void UARTx_Init();
void TIMx_Init();

/** @addtogroup STM32F1xx_HAL_Validation
 * @{
 */

/** @addtogroup STANDARD_CHECK
 * @{
 */

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
USBD_HandleTypeDef USBD_Device;

/* Private function prototypes ----------------------------------------------- */
void SystemClock_Config(void);

/* Private functions --------------------------------------------------------- */

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* Configure the system clock to 72 MHz */
  SystemClock_Config();

  /* Init Device Library */
  USBD_Init(&USBD_Device, &VCP_Desc, 0);

  /* Add Supported Class */
  USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);

  /* Add CDC Interface Class */
  USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);

  /* Start Device Process */
  USBD_Start(&USBD_Device);

  /* UartHandle.Instance = USARTx;
  UartHandle.Init.BaudRate = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits = UART_STOPBITS_1;
  UartHandle.Init.Parity = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode = UART_MODE_TX_RX;

  if (HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_UART_Receive_IT(&UartHandle, (uint8_t *)UserRxBufferUART, 1) != HAL_OK)
  {
    Error_Handler();
  } */

  /* TimHandle.Instance = TIMx;

  TimHandle.Init.Period = (CDC_POLLING_INTERVAL * 1000) - 1;
  TimHandle.Init.Prescaler = 72 - 1;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
  {
    Error_Handler();
  } */

  UARTx_Init();
  TIMx_Init();

  TIM_SW_PWM_Init();
  TIM_SW_MEASURE_Init();

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* HAL_TIM_Base_Start_IT(&htim6); */

  dwtInit();
  /* Infinite loop */
  while (1)
  {
    if (RxMessFlag_USB == 1)
    {
      CommandManager(UserRxBufferUSB);
      RxMessFlag_USB = 0;
    }

    if (RxMessFlag_UART == 1)
    {
      CommandManager(UserRxBufferUART);
      RxMessFlag_UART = 0;
    }
  }
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 72000000
 *            HCLK(Hz)                       = 72000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 2
 *            APB2 Prescaler                 = 1
 *            HSE Frequency(Hz)              = 8000000
 *            HSE PREDIV1                    = 1
 *            PLLMUL                         = 9
 *            Flash Latency(WS)              = 2
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {};

  GPIO_InitTypeDef GPIO_InitStruct = {};

  //* Initializes the RCC Oscillators according to the specified parameters
  //* in the RCC_OscInitTypeDef structure.

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_1);

  // Configure GPIO pin : PA8
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void TIM_SW_PWM_Init()
{
  TIM_HandleTypeDef *htim = tim_handl_table[TIM_SW_PWM];

  TIM_MasterConfigTypeDef sMasterConfig = {};

  htim->Instance = TIM_SW_PWM;

  TIM_Base_MspInit(htim);

  htim->Init.Prescaler = 0;
  htim->Init.CounterMode = TIM_COUNTERMODE_UP;
  htim->Init.Period = 720 - 1;
  htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(htim) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* TIM6 interrupt Init */
  HAL_NVIC_SetPriority(TIM6_IRQn, 5, 0);

  HAL_TIM_Base_Start_IT(htim);
}

void TIM_SW_MEASURE_Init()
{
  TIM_HandleTypeDef *htim = tim_handl_table[TIM_SW_MEASURE];

  TIM_MasterConfigTypeDef sMasterConfig = {};

  htim->Instance = TIM_SW_MEASURE;
  
  TIM_Base_MspInit(htim);

  htim->Init.Prescaler = 1151;
  htim->Init.CounterMode = TIM_COUNTERMODE_UP;
  htim->Init.Period = 62499;
  htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(htim) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OnePulse_Init(htim, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM7_IRQn);
}

void UARTx_Init()
{
  UartHandle.Instance = USARTx;
  UartHandle.Init.BaudRate = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits = UART_STOPBITS_1;
  UartHandle.Init.Parity = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode = UART_MODE_TX_RX;

  if (HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_UART_Receive_IT(&UartHandle, (uint8_t *)UserRxBufferUART, 1) != HAL_OK)
  {
    Error_Handler();
  }
}

void TIMx_Init()
{
  TimHandle.Instance = TIMx;

  TimHandle.Init.Period = (CDC_POLLING_INTERVAL * 1000) - 1;
  TimHandle.Init.Prescaler = 72 - 1;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler(void)
{
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line
   * number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
   * line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
 * @}
 */

/**
 * @}
 */
