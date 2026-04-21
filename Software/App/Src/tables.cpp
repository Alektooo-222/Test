#include "main.h"
#include "etl/map.h"
#include "status_conf.h"
#include "tim.h"

// Таблица соответствия таймера и его IRQ для Capture/Compare событий
etl::map<TIM_TypeDef *, IRQn_Type, 16> tim_cc_irq_table = {
    {TIM1, TIM1_CC_IRQn},
    {TIM2, TIM2_IRQn},
    {TIM3, TIM3_IRQn},
    {TIM4, TIM4_IRQn},
    /* {TIM5, TIM5_IRQn},
    {TIM8, TIM8_CC_IRQn},
    {TIM9, TIM1_BRK_TIM9_IRQn},
    {TIM10, TIM1_UP_TIM10_IRQn},
    {TIM11, TIM1_TRG_COM_TIM11_IRQn},
    {TIM12, TIM8_BRK_TIM12_IRQn},
    {TIM13, TIM8_UP_TIM13_IRQn},
    {TIM14, TIM8_TRG_COM_TIM14_IRQn}, */
};

// Таблица соответствия GPIO-пина и линии внешнего прерывания EXTI
etl::map<uint16_t, IRQn_Type, 16> exti_irq_table = {
    {GPIO_PIN_0, EXTI0_IRQn},
    {GPIO_PIN_1, EXTI1_IRQn},
    {GPIO_PIN_2, EXTI2_IRQn},
    {GPIO_PIN_3, EXTI3_IRQn},
    {GPIO_PIN_4, EXTI4_IRQn},
    {GPIO_PIN_5, EXTI9_5_IRQn},
    {GPIO_PIN_6, EXTI9_5_IRQn},
    {GPIO_PIN_7, EXTI9_5_IRQn},
    {GPIO_PIN_8, EXTI9_5_IRQn},
    {GPIO_PIN_9, EXTI9_5_IRQn},
    {GPIO_PIN_10, EXTI15_10_IRQn},
    {GPIO_PIN_11, EXTI15_10_IRQn},
    {GPIO_PIN_12, EXTI15_10_IRQn},
    {GPIO_PIN_13, EXTI15_10_IRQn},
    {GPIO_PIN_14, EXTI15_10_IRQn},
    {GPIO_PIN_15, EXTI15_10_IRQn},
};

// Таблица соответствия таймера и его строкового имени (для отладки/логирования)
etl::map<TIM_TypeDef *, const char *, 12> tim_table = {
    {TIM1, "TIM1"},
    {TIM2, "TIM2"},
    {TIM3, "TIM3"},
    {TIM4, "TIM4"},
    /* {TIM5, "TIM5"},
    {TIM8, "TIM8"},
    {TIM9, "TIM9"},
    {TIM10, "TIM10"},
    {TIM11, "TIM11"},
    {TIM12, "TIM12"},
    {TIM13, "TIM13"},
    {TIM14, "TIM14"}, */
};

// Таблица соответствия таймера и его HAL-дескриптора
etl::map<TIM_TypeDef *, TIM_HandleTypeDef *, 4> tim_handl_table = {
    {TIM1, &htim1},
    {TIM2, &htim2},
    {TIM3, &htim3},
    {TIM4, &htim4},
    /* {TIM5, &htim5},
    {TIM8, &htim8},
    {TIM9, &htim9},
    {TIM10, &htim10},
    {TIM11, &htim11},
    {TIM12, &htim12},
    {TIM13, &htim13},
    {TIM14, &htim14}, */
};

// Таблица альтернативных функций GPIO (Alternate Function) для таймеров
/* etl::map<TIM_TypeDef *, uint8_t, 12> tim_af_table = {
    {TIM1, GPIO_AF1_TIM1},
    {TIM2, GPIO_AF1_TIM2},
    {TIM3, GPIO_AF2_TIM3},
    {TIM4, GPIO_AF2_TIM4},
    {TIM5, GPIO_AF2_TIM5},
    {TIM8, GPIO_AF3_TIM8 },
    {TIM9, GPIO_AF3_TIM9 },
    {TIM10,GPIO_AF3_TIM10},
    {TIM11,GPIO_AF3_TIM11},
    {TIM12, GPIO_AF9_TIM12},
    {TIM13, GPIO_AF9_TIM13},
    {TIM14, GPIO_AF9_TIM14},
}; */

// Таблица каналов таймера (Channel 1–4) в строковом виде
etl::map<uint32_t, const char *, 4> ch_table = {
    {TIM_CHANNEL_1, "CHANNEL_1"},
    {TIM_CHANNEL_2, "CHANNEL_2"},
    {TIM_CHANNEL_3, "CHANNEL_3"},
    {TIM_CHANNEL_4, "CHANNEL_4"},
};

// Таблица состояний/режимов конфигурации пинов
etl::map<Pin_configured, const char *, 16> staus_table = {
    {Pin_configured::PWM_HARD, "PWM_HARD"},
    {Pin_configured::PWM_SOFT, "PWM_SOFT"},
    {Pin_configured::MEASURE_HARD, "MEASURE_HARD"},
    {Pin_configured::MEASURE_SOFT, "MEASURE_SOFT"},
    {Pin_configured::USART1_TX, "USART1_TX"},
    {Pin_configured::USART1_RX, "USART1_RX"},
    {Pin_configured::SWCLK, "SWCLK"},
    {Pin_configured::SWDIO, "SWDIO"},
    {Pin_configured::USB_DP, "USB_DP"},
    {Pin_configured::USB_DM, "USB_DM"},
    {Pin_configured::OSC_IN, "OSC_IN"},
    {Pin_configured::OSC_OUT, "OSC_OUT"},
    {Pin_configured::OSC32_IN, "OSC32_IN"},
    {Pin_configured::OSC32_OUT, "OSC32_OUT"},
    {Pin_configured::NRST, "NRST"},
    {Pin_configured::NO_CONFIG, "NO_CONFIG"},
};