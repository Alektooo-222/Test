#include "tim.h"
#include "etl/map.h"
#include "application.h"

void TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle);

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim8;
/* TIM_HandleTypeDef htim9;
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;
TIM_HandleTypeDef htim12;
TIM_HandleTypeDef htim13;
TIM_HandleTypeDef htim14; */

extern etl::map<TIM_TypeDef *, TIM_HandleTypeDef *, 12> tim_handl_table;
extern etl::map<TIM_TypeDef *, uint8_t, 12> tim_af_table;
extern etl::map<TIM_TypeDef *, IRQn_Type, 16> tim_cc_irq_table;

uint32_t get_freq_clk_tim(const Peripheral_capabilities &tim)
{
  uint32_t tim_clk = 0;

  if (reinterpret_cast<unsigned long>(tim.timer) >= APB1PERIPH_BASE && reinterpret_cast<unsigned long>(tim.timer) < APB2PERIPH_BASE)
  {
    tim_clk = TIM_CLK_APB1;
  }
  else
  {
    tim_clk = TIM_CLK_APB2;
  }

  return tim_clk;
}

static uint32_t u32_abs_diff(uint32_t a, uint32_t b)
{
  return (a > b) ? (a - b) : (b - a);
}

tim_cfg_t tim_pick(uint32_t f_tim, uint32_t f_hz, uint8_t arr_bits, double duty)
{
  tim_cfg_t best = {};
  uint32_t arr_max = (arr_bits == 32) ? 0xFFFFFFFFu : 0xFFFFu;

  // защита от деления на 0
  if (f_hz == 0 || f_tim == 0)
  {
    best.psc = 0;
    best.arr = 0;
    best.ccr = 0;
    best.f_real = 0;
    return best;
  }

  uint32_t best_err = 0xFFFFFFFFu;

  for (uint32_t psc = 0; psc <= 0xFFFFu; psc++)
  {
    // вычисляем ARR примерно как round(f_tim/((psc+1)*f_hz) - 1)
    // используем целочисленное округление:
    uint64_t denom = static_cast<uint64_t>(psc + 1) * static_cast<uint64_t>(f_hz);
    uint64_t q = static_cast<uint64_t>(f_tim);

    if (denom == 0)
      continue;

    // значение (ARR+1) ≈ round(f_tim/denom)
    uint64_t arrp1 = static_cast<uint64_t>((static_cast<double>(q) + static_cast<double>(denom) / 2.0) / static_cast<double>(denom)); // round()

    if (arrp1 == 0)
      continue; // ARR был бы -1
    if (arrp1 > static_cast<uint64_t>(arr_max) + 1u)
      continue;

    uint32_t arr = static_cast<uint32_t>(arrp1 - 1u);

    // фактическая частота
    uint32_t f_real = static_cast<uint32_t>(static_cast<double>(f_tim) / (static_cast<double>(psc + 1u) * static_cast<double>(arr + 1u)));

    uint32_t err = u32_abs_diff(f_real, f_hz);
    if (err < best_err)
    {
      best_err = err;
      best.psc = static_cast<uint16_t>(psc);
      best.arr = arr;
      best.f_real = f_real;

      // CCR по duty (0..1)
      if (duty < 0.0f)
        duty = 0.0f;
      if (duty > 1.0f)
        duty = 1.0f;

      uint32_t ccr = static_cast<uint32_t>((duty * static_cast<double>(arr + 1u)) + 0.5f); // round
      if (ccr > arr)
        ccr = arr;
      best.ccr = ccr;

      if (best_err == 0)
        break; // идеальное попадание
    }
  }

  return best;
}

StatusConfigPWM config_tim_pwm(const Pin_name &pin, const Peripheral_capabilities &tim, tim_cfg_t &tim_cfg)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {};
  TIM_MasterConfigTypeDef sMasterConfig = {};
  TIM_OC_InitTypeDef sConfigOC = {};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {};
  GPIO_InitTypeDef GPIO_InitStruct = {};

  TIM_HandleTypeDef *htim = tim_handl_table[tim.timer];

  /* htim->Instance = tim.timer;

  TIM_Base_MspInit(htim); */

  htim->Init.Prescaler = tim_cfg.psc;
  htim->Init.CounterMode = TIM_COUNTERMODE_UP;
  htim->Init.Period = tim_cfg.arr;
  htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim->Init.RepetitionCounter = 0;
  htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (htim->Instance != tim.timer/* tim.timer->PSC != tim_cfg.psc && tim.timer->ARR != tim_cfg.arr */)
  {
    htim->Instance = tim.timer;
    TIM_Base_MspInit(htim);
    if (HAL_TIM_Base_Init(htim) != HAL_OK)
    {
      // Error_Handler();
      return StatusConfigPWM::CONFIG_HW_PWM_ERROR;
    }

    if (HAL_TIM_PWM_Init(htim) != HAL_OK)
    {
      // Error_Handler();
      return StatusConfigPWM::CONFIG_HW_PWM_ERROR;
    }
  }

  /* sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig) != HAL_OK)
  {
    // Error_Handler();
    return StatusConfigPWM::CONFIG_HW_PWM_ERROR;
  } */
  /* if (tim.timer->PSC != tim_cfg.psc && tim.timer->ARR != tim_cfg.arr)
  { */
  /* if (HAL_TIM_PWM_Init(htim) != HAL_OK)
  {
    // Error_Handler();
    return StatusConfigPWM::CONFIG_HW_PWM_ERROR;
  } */
  /* } */
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig) != HAL_OK)
  {
    // Error_Handler();
    return StatusConfigPWM::CONFIG_HW_PWM_ERROR;
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = tim_cfg.ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, tim.channel) != HAL_OK)
  {
    return StatusConfigPWM::CONFIG_HW_PWM_ERROR;
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(htim, &sBreakDeadTimeConfig) != HAL_OK)
  {
    return StatusConfigPWM::CONFIG_HW_PWM_ERROR;
  }

  GPIO_InitStruct.Pin = pin.pin_number;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  /* GPIO_InitStruct.Alternate = tim_af_table[tim.timer]; */
  HAL_GPIO_Init(pin.port, &GPIO_InitStruct);

  /* if (tim.inv_ch == Inverse_Ch::P)
  {
    if (HAL_TIM_PWM_Start(htim, tim.channel) != HAL_OK)
    {
      return StatusConfigPWM::CONFIG_HW_PWM_ERROR_START;
    }
  }

  if (tim.inv_ch == Inverse_Ch::N)
  {
    if (HAL_TIMEx_PWMN_Start(htim, tim.channel) != HAL_OK)
    {
      return StatusConfigPWM::CONFIG_HW_PWM_ERROR_START;
    }
  } */
  return StatusConfigPWM::CONFIG_HW_PWM_OK;
}

StatusConfigMeasure config_tim_measure(const Pin_name &pin, const Peripheral_capabilities &tim, uint32_t bits, uint32_t tim_clk)
{

  TIM_ClockConfigTypeDef sClockSourceConfig = {};
  TIM_MasterConfigTypeDef sMasterConfig = {};
  TIM_IC_InitTypeDef sConfigIC = {};

  GPIO_InitTypeDef GPIO_InitStruct = {};

  GPIO_InitStruct.Pin = pin.pin_number;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  /* GPIO_InitStruct.Alternate = tim_af_table[tim.timer]; */
  HAL_GPIO_Init(pin.port, &GPIO_InitStruct);

  TIM_HandleTypeDef *htim = tim_handl_table[tim.timer];

  htim->Instance = tim.timer;

  TIM_Base_MspInit(htim);

  htim->Init.Prescaler = static_cast<uint32_t>(static_cast<double>(tim_clk) / 1000000.0) - 1;
  htim->Init.CounterMode = TIM_COUNTERMODE_UP;
  htim->Init.Period = (bits == 32) ? 0xFFFFFFFFu : 0xFFFFu;
  htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(htim) != HAL_OK)
  {
    return StatusConfigMeasure::CONFIG_HW_MEASURE_PIN_FAIL;
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(htim, &sClockSourceConfig) != HAL_OK)
  {
    return StatusConfigMeasure::CONFIG_HW_MEASURE_PIN_FAIL;
  }
  if (HAL_TIM_IC_Init(htim) != HAL_OK)
  {
    return StatusConfigMeasure::CONFIG_HW_MEASURE_PIN_FAIL;
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig) != HAL_OK)
  {
    return StatusConfigMeasure::CONFIG_HW_MEASURE_PIN_FAIL;
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(htim, &sConfigIC, tim.channel) != HAL_OK)
  {
    return StatusConfigMeasure::CONFIG_HW_MEASURE_PIN_FAIL;
  }

  HAL_NVIC_SetPriority(tim_cc_irq_table[tim.timer], 1, 0);
  /* HAL_NVIC_EnableIRQ(tim_cc_irq_table[tim.timer]); */

  return StatusConfigMeasure::CONFIG_HW_MEASURE_PIN_OK;
}

void TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle)
{
  if (tim_baseHandle->Instance == TIM1)
  {
    __HAL_RCC_TIM1_CLK_ENABLE();
  }
  else if (tim_baseHandle->Instance == TIM2)
  {
    __HAL_RCC_TIM2_CLK_ENABLE();
  }
  else if (tim_baseHandle->Instance == TIM3)
  {
    __HAL_RCC_TIM3_CLK_ENABLE();
  }
  else if (tim_baseHandle->Instance == TIM4)
  {
    __HAL_RCC_TIM4_CLK_ENABLE();
  }
  else if (tim_baseHandle->Instance == TIM5)
  {
    __HAL_RCC_TIM5_CLK_ENABLE();
  }
  else if (tim_baseHandle->Instance == TIM6)
  {
    __HAL_RCC_TIM6_CLK_ENABLE();
  }
  else if (tim_baseHandle->Instance == TIM7)
  {
    __HAL_RCC_TIM7_CLK_ENABLE();
  }
  else if (tim_baseHandle->Instance == TIM8)
  {
    __HAL_RCC_TIM8_CLK_ENABLE();
  }
  /* else if (tim_baseHandle->Instance == TIM9)
  {
    __HAL_RCC_TIM9_CLK_ENABLE();
  }
  else if (tim_baseHandle->Instance == TIM10)
  {
    __HAL_RCC_TIM10_CLK_ENABLE();
  }
  else if (tim_baseHandle->Instance == TIM11)
  {
    __HAL_RCC_TIM11_CLK_ENABLE();
  }
  else if (tim_baseHandle->Instance == TIM12)
  {
    __HAL_RCC_TIM12_CLK_ENABLE();
  }
  else if (tim_baseHandle->Instance == TIM13)
  {
    __HAL_RCC_TIM13_CLK_ENABLE();
  }
  else if (tim_baseHandle->Instance == TIM14)
  {
    __HAL_RCC_TIM14_CLK_ENABLE();
  } */
}