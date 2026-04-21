#include "pins.h"

Pin ListPins[50] = {
    {{"PA0", GPIOA, GPIO_PIN_0}, {{TIM2, TIM_CHANNEL_1, Inverse_Ch::P}, {TIM5, TIM_CHANNEL_1, Inverse_Ch::P}}, Pin_configured::NO_CONFIG},
    {{"PA1", GPIOA, GPIO_PIN_1}, {{TIM2, TIM_CHANNEL_2, Inverse_Ch::P}, {TIM5, TIM_CHANNEL_2, Inverse_Ch::P}}, Pin_configured::NO_CONFIG},
    {{"PA2", GPIOA, GPIO_PIN_2}, {{TIM2, TIM_CHANNEL_3, Inverse_Ch::P}, {TIM5, TIM_CHANNEL_3, Inverse_Ch::P}}, Pin_configured::NO_CONFIG},
    {{"PA3", GPIOA, GPIO_PIN_3}, {{TIM2, TIM_CHANNEL_4, Inverse_Ch::P}, {TIM5, TIM_CHANNEL_4, Inverse_Ch::P}}, Pin_configured::NO_CONFIG},
    {{"PA4", GPIOA, GPIO_PIN_4}, {}, Pin_configured::NO_CONFIG},
    {{"PA5", GPIOA, GPIO_PIN_5}, {{TIM8, TIM_CHANNEL_1, Inverse_Ch::N}, {TIM2, TIM_CHANNEL_1, Inverse_Ch::P}}, Pin_configured::NO_CONFIG}, // TIM8, TIM_CHANNEL_1N
    {{"PA6", GPIOA, GPIO_PIN_6}, {/* {TIM3, TIM_CHANNEL_1, Inverse_Ch::P},  */}, Pin_configured::NO_CONFIG},
    {{"PA7", GPIOA, GPIO_PIN_7}, {{TIM1, TIM_CHANNEL_1, Inverse_Ch::N}, {TIM8, TIM_CHANNEL_1, Inverse_Ch::N}, /* {TIM3, TIM_CHANNEL_2, Inverse_Ch::P},  */}, Pin_configured::NO_CONFIG}, // TIM1_CH1N, TIM8_CH1N
    {{"PA8", GPIOA, GPIO_PIN_8}, {{TIM1, TIM_CHANNEL_1, Inverse_Ch::P}}, Pin_configured::NO_CONFIG},
    {{"PA9", GPIOA, GPIO_PIN_9}, {{TIM1, TIM_CHANNEL_2, Inverse_Ch::P}}, Pin_configured::USART1_TX},
    {{"PA10", GPIOA, GPIO_PIN_10}, {{TIM1, TIM_CHANNEL_3, Inverse_Ch::P}}, Pin_configured::USART1_RX},
    {{"PA11", GPIOA, GPIO_PIN_11}, {{TIM1, TIM_CHANNEL_4, Inverse_Ch::P}}, Pin_configured::USB_DM},
    {{"PA12", GPIOA, GPIO_PIN_12}, {}, Pin_configured::USB_DP},
    {{"PA13", GPIOA, GPIO_PIN_13}, {}, Pin_configured::SWDIO},
    {{"PA14", GPIOA, GPIO_PIN_14}, {}, Pin_configured::SWCLK},
    {{"PA15", GPIOA, GPIO_PIN_15}, {{TIM2, TIM_CHANNEL_1, Inverse_Ch::P}}, Pin_configured::NO_CONFIG},

    {{"PB0", GPIOB, GPIO_PIN_0}, {{TIM1, TIM_CHANNEL_2, Inverse_Ch::N}, {TIM8, TIM_CHANNEL_2, Inverse_Ch::N}, /* {TIM3, TIM_CHANNEL_3, Inverse_Ch::P} */}, Pin_configured::NO_CONFIG}, // TIM1_CH2N TIM8_CH2N
    {{"PB1", GPIOB, GPIO_PIN_1}, {{TIM1, TIM_CHANNEL_3, Inverse_Ch::N}, {TIM8, TIM_CHANNEL_3, Inverse_Ch::N}, /* {TIM3, TIM_CHANNEL_4, Inverse_Ch::P} */}, Pin_configured::NO_CONFIG}, // TIM1_CH3N TIM8_CH3N
    {{"PB2", GPIOB, GPIO_PIN_2}, {{TIM2, TIM_CHANNEL_4, Inverse_Ch::P}}, Pin_configured::NO_CONFIG},
    {{"PB3", GPIOB, GPIO_PIN_3}, {{TIM2, TIM_CHANNEL_2, Inverse_Ch::P}}, Pin_configured::NO_CONFIG},
    {{"PB4", GPIOB, GPIO_PIN_4}, {/* {TIM3, TIM_CHANNEL_1, Inverse_Ch::P} */}, Pin_configured::NO_CONFIG},
    {{"PB5", GPIOB, GPIO_PIN_5}, {/* {TIM3, TIM_CHANNEL_2, Inverse_Ch::P} */}, Pin_configured::NO_CONFIG},
    {{"PB6", GPIOB, GPIO_PIN_6}, {{TIM4, TIM_CHANNEL_1, Inverse_Ch::P}}, Pin_configured::NO_CONFIG},
    {{"PB7", GPIOB, GPIO_PIN_7}, {{TIM4, TIM_CHANNEL_2, Inverse_Ch::P}}, Pin_configured::NO_CONFIG},
    {{"PB8", GPIOB, GPIO_PIN_8}, {{TIM2, TIM_CHANNEL_1, Inverse_Ch::P}, {TIM4, TIM_CHANNEL_3, Inverse_Ch::P}}, Pin_configured::NO_CONFIG},
    {{"PB9", GPIOB, GPIO_PIN_9}, {{TIM2, TIM_CHANNEL_2, Inverse_Ch::P}, {TIM4, TIM_CHANNEL_4, Inverse_Ch::P}}, Pin_configured::NO_CONFIG},
    {{"PB10", GPIOB, GPIO_PIN_10}, {{TIM2, TIM_CHANNEL_3, Inverse_Ch::P}}, Pin_configured::NO_CONFIG},
    {{"PB11", GPIOB, GPIO_PIN_11}, {}, Pin_configured::NO_CONFIG},
    {{"PB12", GPIOB, GPIO_PIN_12}, {}, Pin_configured::NO_CONFIG},
    {{"PB13", GPIOB, GPIO_PIN_13}, {{TIM1, TIM_CHANNEL_1, Inverse_Ch::N}}, Pin_configured::NO_CONFIG},                                                // TIM1_CH1N
    {{"PB14", GPIOB, GPIO_PIN_14}, {{TIM1, TIM_CHANNEL_2, Inverse_Ch::N}, {TIM8, TIM_CHANNEL_2, Inverse_Ch::N}}, Pin_configured::NO_CONFIG}, // TIM1_CH2N TIM8_CH2N
    {{"PB15", GPIOB, GPIO_PIN_15}, {{TIM1, TIM_CHANNEL_3, Inverse_Ch::N}, {TIM8, TIM_CHANNEL_3, Inverse_Ch::N}}, Pin_configured::NO_CONFIG}, // TIM1_CH3N TIM8_CH3N

    {{"PC0", GPIOC, GPIO_PIN_0}, {}, Pin_configured::NO_CONFIG},
    {{"PC1", GPIOC, GPIO_PIN_1}, {}, Pin_configured::NO_CONFIG},
    {{"PC2", GPIOC, GPIO_PIN_2}, {}, Pin_configured::NO_CONFIG},
    {{"PC3", GPIOC, GPIO_PIN_3}, {}, Pin_configured::NO_CONFIG},
    {{"PC4", GPIOC, GPIO_PIN_4}, {}, Pin_configured::NO_CONFIG},
    {{"PC5", GPIOC, GPIO_PIN_5}, {}, Pin_configured::NO_CONFIG},
    {{"PC6", GPIOC, GPIO_PIN_6}, {{TIM8, TIM_CHANNEL_1, Inverse_Ch::P}, /* {TIM3, TIM_CHANNEL_1, Inverse_Ch::P} */}, Pin_configured::NO_CONFIG},
    {{"PC7", GPIOC, GPIO_PIN_7}, {{TIM8, TIM_CHANNEL_2, Inverse_Ch::P}, /* {TIM3, TIM_CHANNEL_2, Inverse_Ch::P} */}, Pin_configured::NO_CONFIG},
    {{"PC8", GPIOC, GPIO_PIN_8}, {{TIM8, TIM_CHANNEL_3, Inverse_Ch::P}, /* {TIM3, TIM_CHANNEL_3, Inverse_Ch::P} */}, Pin_configured::NO_CONFIG},
    {{"PC9", GPIOC, GPIO_PIN_9}, {{TIM8, TIM_CHANNEL_4, Inverse_Ch::P}, /* {TIM3, TIM_CHANNEL_4, Inverse_Ch::P} */}, Pin_configured::NO_CONFIG},
    {{"PC10", GPIOC, GPIO_PIN_10}, {}, Pin_configured::NO_CONFIG},
    {{"PC11", GPIOC, GPIO_PIN_11}, {}, Pin_configured::NO_CONFIG},
    {{"PC12", GPIOC, GPIO_PIN_12}, {}, Pin_configured::NO_CONFIG},
    {{"PC13", GPIOC, GPIO_PIN_13}, {}, Pin_configured::NO_CONFIG},
    {{"PC14", GPIOC, GPIO_PIN_14}, {}, Pin_configured::OSC32_IN},
    {{"PC15", GPIOC, GPIO_PIN_15}, {}, Pin_configured::OSC32_OUT},

    {{"PD0", GPIOD, GPIO_PIN_0}, {}, Pin_configured::OSC_IN},
    {{"PD1", GPIOD, GPIO_PIN_1}, {}, Pin_configured::OSC_OUT}
};