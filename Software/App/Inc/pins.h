#ifndef __PINS_H
#define __PINS_H

#include "main.h"
#include <etl/vector.h>
#include "status_conf.h"

inline constexpr size_t NUMBER_OF_AVAILABLE = 51;

template <typename T, std::size_t N>
constexpr std::size_t CalcSizeArr(T (&)[N])
{
    return N;
}

struct Pin_name
{
    const char *pin_name;
    GPIO_TypeDef *port;
    uint16_t pin_number;
};

struct Peripheral_capabilities
{
    TIM_TypeDef *timer;
    uint32_t channel;
    Inverse_Ch inv_ch;
};

struct Pin
{
    const Pin_name name;
    etl::vector<Peripheral_capabilities, 4> hardware_features;
    Pin_configured conf;
};

extern Pin ListPins[NUMBER_OF_AVAILABLE];

#endif