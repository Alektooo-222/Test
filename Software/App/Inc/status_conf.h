#ifndef __STATUS_CONF_H
#define __STATUS_CONF_H

#include <stdint.h>

enum class Pin_configured : uint32_t
{
    PWM_HARD = 0,
    PWM_SOFT,
    MEASURE_HARD,
    MEASURE_SOFT,
    USART1_TX,
    USART1_RX,
    SWCLK,
    SWDIO,
    USB_DP,
    USB_DM,
    OSC_IN,
    OSC_OUT,
    OSC32_IN,
    OSC32_OUT,
    NRST,
    NO_CONFIG
};

enum class Inverse_Ch : uint8_t
{
    P = 0,
    N
};

enum class StatusConfigPWM : uint8_t
{
    CONFIG_SW_PWM_PIN_OK = 0,
    CONFIG_SW_PWM_PIN_FAIL,
    CONFIG_SW_PWM_PIN_ERROR_FREQ_MORE_MAX,
    CONFIG_SW_PWM_PIN_ERROR_PULSE_MORE_100,
    REGISTRATION_PIN_OK,
    REGISTRATION_PIN_FAIL,
    CONFIG_HW_PWM_NONE,
    CONFIG_HW_PWM_ERROR_BIG_FREQ,
    CONFIG_HW_PWM_OK,
    CONFIG_HW_PWM_FAIL,
    CONFIG_HW_PWM_ERROR,
    CONFIG_HW_PWM_ERROR_START,
};

enum class StatusConfigMeasure : uint8_t
{
    CONFIG_SW_MEASURE_PIN_OK,
    CONFIG_SW_MEASURE_PIN_FAIL,
    CONFIG_HW_MEASURE_PIN_OK,
    CONFIG_HW_MEASURE_PIN_FAIL,
    REGISTRATION_PIN_OK,
    REGISTRATION_PIN_FAIL
};

enum class StateMeasurement : uint8_t
{
    MEASURMENT_COMPLETED,
    MEASURMENT_IN_PROGRESS,
    MEASURMENT_NONE
};

enum class ConfigMeasurement : uint8_t
{
    HARDWARE_MEASUREMENT,
    SOFTWARE_MEASUREMENT
};

#endif