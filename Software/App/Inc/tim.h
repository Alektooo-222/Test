#ifndef __TIM_H
#define __TIM_H

#include "main.h"
#include "pins.h"
#include "status_conf.h"

typedef struct
{
    uint32_t psc;
    uint32_t arr;
    uint32_t ccr;    // если PWM не нужен — можно игнорировать
    uint32_t f_real; // фактическая частота, Гц
} tim_cfg_t;

tim_cfg_t tim_pick(uint32_t f_tim, uint32_t f_hz, uint8_t arr_bits, double duty);
StatusConfigPWM config_tim_pwm(const Pin_name &pin, const Peripheral_capabilities &tim, tim_cfg_t &tim_cfg);
StatusConfigMeasure config_tim_measure(const Pin_name &pin, const Peripheral_capabilities &tim, uint32_t bits, uint32_t tim_clk);
uint32_t get_freq_clk_tim(const Peripheral_capabilities &tim);

/* extern TIM_HandleTypeDef htim1; */
/* extern TIM_HandleTypeDef htim6; */
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim10;
extern TIM_HandleTypeDef htim11;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim13;
extern TIM_HandleTypeDef htim14;

#endif