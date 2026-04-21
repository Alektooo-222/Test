#include "main.h"
#include "application.h"
#include "application_pwm.h"
#include "application_transmit.h"
#include "tim.h"
#include "etl/to_string.h"
#include "etl/string.h"
#include "etl/format_spec.h"
#include "etl/map.h"

const uint32_t MAX_SW_PWM_FREQ = 1000;

extern Pin ListPins[37];
extern etl::map<TIM_TypeDef *, TIM_HandleTypeDef *, 4> tim_handl_table;
extern etl::map<Pin_configured, const char *, 16> staus_table;

etl::vector<pin_software_pwm, 20> registered_sw_pins;

etl::vector<Peripheral_capabilities, 20> registered_pwm_tim;

uint8_t GPIO_Init_SW_PWM(const Pin_name &pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {};

    HAL_GPIO_WritePin(pin.port, pin.pin_number, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = pin.pin_number;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(pin.port, &GPIO_InitStruct);

    return 0;
}

StatusConfigPWM registration_pin(const Pin_name &pin, uint32_t freq, uint32_t pulse)
{
    const double TIM_SOFTWARE_PWM_clk = 72000000.0;

    const double time_sample = ((TIM_SOFTWARE_PWM->PSC + 1) * (TIM_SOFTWARE_PWM->ARR + 1)) / TIM_SOFTWARE_PWM_clk;

    uint32_t period = ((1 / static_cast<double>(freq)) / time_sample);
    uint32_t pulse_t = pulse * period / 100;

    pin_software_pwm pin_pwm{pin, period, pulse_t};

    GPIO_Init_SW_PWM(pin);

    registered_sw_pins.push_back(pin_pwm);

    return StatusConfigPWM::REGISTRATION_PIN_OK;
}

StatusConfigPWM config_software_pwm(const Pin_name &pin, uint32_t freq, uint32_t pulse)
{
    StatusConfigPWM status_reg_pin;

    if (freq > MAX_SW_PWM_FREQ)
    {
        return StatusConfigPWM::CONFIG_SW_PWM_PIN_ERROR_FREQ_MORE_MAX;
    }

    if (pulse > 100)
    {
        return StatusConfigPWM::CONFIG_SW_PWM_PIN_ERROR_PULSE_MORE_100;
    }

    status_reg_pin = registration_pin(pin, freq, pulse);

    if (status_reg_pin == StatusConfigPWM::REGISTRATION_PIN_FAIL)
    {
        return StatusConfigPWM::REGISTRATION_PIN_FAIL;
    }

    return StatusConfigPWM::CONFIG_SW_PWM_PIN_OK;
}

StatusConfigPWM config_hardware_pwm(const Pin_name &pin, const Peripheral_capabilities &tim, uint32_t freq, uint32_t pulse)
{

    uint32_t tim_clk;

    /* uint8_t bits = (tim.timer == TIM2 || tim.timer == TIM5) ? 32 : 16; */
    uint8_t bits = 16;

    tim_cfg_t tmr_cnf{};

    tim_clk = get_freq_clk_tim(tim);

    if (freq > (tim_clk / 2))
    {
        return StatusConfigPWM::CONFIG_HW_PWM_ERROR_BIG_FREQ;
    }

    tmr_cnf = tim_pick(tim_clk, freq, bits, static_cast<double>(pulse) / 100.0);

    if (!registered_pwm_tim.full())
    {
        registered_pwm_tim.push_back(tim);
    }
    else if (registered_pwm_tim.full())
    {
        return StatusConfigPWM::CONFIG_HW_PWM_FAIL;
    }

    config_tim_pwm(pin, tim, tmr_cnf);

    return StatusConfigPWM::CONFIG_HW_PWM_OK;
}

void cmd_config_gen_handler(string_rx_mess &str)
{
    StringTxQueue mes;

    etl::string<20> max_sw_pwm_freq_pulse_str;
    max_sw_pwm_freq_pulse_str.clear();

    mes.clear();

    size_t first_space = str.find(' ');
    size_t second_space = str.find(' ', first_space + 1);
    size_t third_space = str.find(' ', second_space + 1);
    size_t new_line = str.find('\n');

    uint8_t status_pwm_hard = 0;
    StatusConfigPWM status_config_hardware_pwm = StatusConfigPWM::CONFIG_HW_PWM_NONE;

    uint32_t freq_clk_tim = 0;
    uint32_t freq_busy_tim = 0;

    if (first_space == string_rx_mess::npos || second_space == string_rx_mess::npos || third_space == string_rx_mess::npos)
    {
        add_str(mes, "Incorrect command\n", tx_uart_queue);
        push_with_timeout(tx_uart_queue, mes, 100);
        return;
    }

    if (new_line == string_rx_mess::npos)
    {
        new_line = str.size();
    }

    etl::string<4> pin_str(str, first_space + 1, second_space - first_space - 1);
    etl::string<10> freq_str(str, second_space + 1, third_space - second_space - 1);
    etl::string<4> pulse_str(str, third_space + 1, new_line - third_space - 1);

    uint32_t freq = atoi(freq_str.c_str());
    uint32_t pulse = atoi(pulse_str.c_str());

    for (auto &pin : ListPins)
    {
        if (pin_str == pin.name.pin_name)
        {
            if (pin.conf == Pin_configured::NO_CONFIG)
            {
                if (pin.hardware_features.size() == 0) // Если ли аппаратного таймера нет и частота меньше MAX_SW_PWM_FREQ, то запустить программный шим
                {
                    StatusConfigPWM status_config_software_pwm = config_software_pwm(pin.name, freq, pulse);

                    if (status_config_software_pwm == StatusConfigPWM::CONFIG_SW_PWM_PIN_OK)
                    {
                        pin.conf = Pin_configured::PWM_SOFT;
                        add_str(mes, "GEN CONFIGURED on ", tx_uart_queue);
                        add_str(mes, pin.name.pin_name, tx_uart_queue);
                        add_str(mes, ": ", tx_uart_queue);
                        add_str(mes, freq_str, tx_uart_queue);
                        add_str(mes, " Hz, ", tx_uart_queue);
                        add_str(mes, pulse_str, tx_uart_queue);
                        add_str(mes, "% (software)\n", tx_uart_queue);
                        push_with_timeout(tx_uart_queue, mes, 100);
                        return;
                    }
                    else if (status_config_software_pwm == StatusConfigPWM::CONFIG_SW_PWM_PIN_ERROR_FREQ_MORE_MAX)
                    {
                        etl::to_string(MAX_SW_PWM_FREQ, max_sw_pwm_freq_pulse_str);

                        add_str(mes, "Can not set Freq Software PWM more ", tx_uart_queue);
                        add_str(mes, max_sw_pwm_freq_pulse_str, tx_uart_queue);
                        add_str(mes, "\n", tx_uart_queue);
                        push_with_timeout(tx_uart_queue, mes, 100);
                        return;
                    }
                    else if (status_config_software_pwm == StatusConfigPWM::CONFIG_SW_PWM_PIN_ERROR_PULSE_MORE_100)
                    {
                        etl::to_string(100, max_sw_pwm_freq_pulse_str);

                        add_str(mes, "Can not set Pulse Software PWM more ", tx_uart_queue);
                        add_str(mes, max_sw_pwm_freq_pulse_str, tx_uart_queue);
                        add_str(mes, "%\n", tx_uart_queue);
                        push_with_timeout(tx_uart_queue, mes, 100);
                        return;
                    }
                }
                else
                {
                    for (auto &tim : pin.hardware_features)
                    {
                        bool TIMx_Busy = (tim_handl_table[tim.timer]->Instance == tim.timer);
                        bool Chx_Busy = tim_handl_table[tim.timer]->ChannelState[static_cast<uint32_t>(static_cast<double>(tim.channel) / static_cast<double>(4))] == HAL_TIM_CHANNEL_STATE_BUSY;
                        bool ChxN_Busy = tim_handl_table[tim.timer]->ChannelNState[static_cast<uint32_t>(static_cast<double>(tim.channel) / static_cast<double>(4))] == HAL_TIM_CHANNEL_STATE_BUSY;

                        if (TIMx_Busy && !(Chx_Busy || ChxN_Busy)) // Если таймер занят, но требуемые каналы свободны, то настроить аппаратный pwm при условии, что частота работы таймера равна требуемой
                        {
                            freq_clk_tim = get_freq_clk_tim(tim);
                            freq_busy_tim = freq_clk_tim / ((tim.timer->PSC + 1) * (tim.timer->ARR + 1));

                            if (freq_busy_tim == freq)
                            {
                                status_config_hardware_pwm = config_hardware_pwm(pin.name, tim, freq, pulse);

                                status_pwm_hard = (status_config_hardware_pwm == StatusConfigPWM::CONFIG_HW_PWM_OK) ? 1 : 0;
                            }
                        }
                        else if (TIMx_Busy)
                        {
                            continue;
                        }

                        if (status_pwm_hard == 0)
                            status_config_hardware_pwm = config_hardware_pwm(pin.name, tim, freq, pulse);

                        if (status_config_hardware_pwm == StatusConfigPWM::CONFIG_HW_PWM_OK)
                        {
                            pin.conf = Pin_configured::PWM_HARD;
                            status_pwm_hard = 1;
                            add_str(mes, "GEN CONFIGURED on ", tx_uart_queue);
                            add_str(mes, pin.name.pin_name, tx_uart_queue);
                            add_str(mes, ": ", tx_uart_queue);
                            add_str(mes, freq_str, tx_uart_queue);
                            add_str(mes, " Hz, ", tx_uart_queue);
                            add_str(mes, pulse_str, tx_uart_queue);
                            add_str(mes, "% (hardware)\n", tx_uart_queue);
                            push_with_timeout(tx_uart_queue, mes, 100);
                            break;
                        }
                        else if (status_config_hardware_pwm == StatusConfigPWM::CONFIG_HW_PWM_ERROR_BIG_FREQ)
                        {
                            status_pwm_hard = 1;
                            etl::to_string(static_cast<uint32_t>(static_cast<double>(get_freq_clk_tim(tim)) / 2.0), max_sw_pwm_freq_pulse_str);

                            add_str(mes, "Can not set Freq Hardware PWM more ", tx_uart_queue);
                            add_str(mes, max_sw_pwm_freq_pulse_str, tx_uart_queue);
                            add_str(mes, " for this pin\n", tx_uart_queue);
                            push_with_timeout(tx_uart_queue, mes, 100);
                            return;
                        }
                    }

                    if (!status_pwm_hard)
                    {
                        StatusConfigPWM status_config_software_pwm = config_software_pwm(pin.name, freq, pulse);

                        if (status_config_software_pwm == StatusConfigPWM::CONFIG_SW_PWM_PIN_OK)
                        {
                            pin.conf = Pin_configured::PWM_SOFT;
                            add_str(mes, "Only software PWM is available for this pin\n", tx_uart_queue);
                            add_str(mes, "GEN CONFIGURED on ", tx_uart_queue);
                            add_str(mes, pin.name.pin_name, tx_uart_queue);
                            add_str(mes, ": ", tx_uart_queue);
                            add_str(mes, freq_str, tx_uart_queue);
                            add_str(mes, " Hz, ", tx_uart_queue);
                            add_str(mes, pulse_str, tx_uart_queue);
                            add_str(mes, "% (software)\n", tx_uart_queue);
                            push_with_timeout(tx_uart_queue, mes, 100);
                            return;
                        }
                        else if (status_config_software_pwm == StatusConfigPWM::CONFIG_SW_PWM_PIN_ERROR_FREQ_MORE_MAX)
                        {
                            etl::to_string(MAX_SW_PWM_FREQ, max_sw_pwm_freq_pulse_str);

                            add_str(mes, "Only software PWM is available for this pin\n", tx_uart_queue);
                            add_str(mes, "Can not set Freq Software PWM more ", tx_uart_queue);
                            add_str(mes, max_sw_pwm_freq_pulse_str, tx_uart_queue);
                            add_str(mes, "\n", tx_uart_queue);
                            push_with_timeout(tx_uart_queue, mes, 100);
                            return;
                        }
                        else if (status_config_software_pwm == StatusConfigPWM::CONFIG_SW_PWM_PIN_ERROR_PULSE_MORE_100)
                        {
                            etl::to_string(100, max_sw_pwm_freq_pulse_str);

                            add_str(mes, "Only software PWM is available for this pin\n", tx_uart_queue);
                            add_str(mes, "Can not set Pulse Software PWM more ", tx_uart_queue);
                            add_str(mes, max_sw_pwm_freq_pulse_str, tx_uart_queue);
                            add_str(mes, "%\n", tx_uart_queue);
                            push_with_timeout(tx_uart_queue, mes, 100);
                            return;
                        }
                    }
                }
            }
            else
            {
                mes.clear();
                mes += pin_str;
                mes += " is used by ";
                mes += staus_table[pin.conf];
                mes += "\n";
                push_with_timeout(tx_uart_queue, mes, 100);
            }
        }
    }
}

void cmd_gen_handler(string_rx_mess &str)
{
    StringTxQueue mes(str);

    size_t space = str.find(' ');

    if (space == string_rx_mess::npos)
    {
        add_str(mes, "Incorrect command\n", tx_uart_queue);
        push_with_timeout(tx_uart_queue, mes, 100);
        return;
    }

    etl::transform(mes.begin(),
                   mes.end(),
                   mes.begin(),
                   [](unsigned char c)
                   { return std::toupper(c); });

    if (mes == "GEN START\n")
    {
        mes.erase(mes.end() - 1);
        add_str(mes, "ED\n", tx_uart_queue);
        push_with_timeout(tx_uart_queue, mes, 100);
        HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);

        for (auto &tim : registered_pwm_tim)
        {
            if (tim.inv_ch == Inverse_Ch::P)
            {
                HAL_TIM_PWM_Start(tim_handl_table[tim.timer], tim.channel);
            }

            if (tim.inv_ch == Inverse_Ch::N)
            {
                HAL_TIMEx_PWMN_Start(tim_handl_table[tim.timer], tim.channel);
            }
        }
    }
    else if (mes == "GEN STOP\n")
    {
        mes.erase(mes.end() - 1);
        add_str(mes, "PED\n", tx_uart_queue);
        push_with_timeout(tx_uart_queue, mes, 100);
        HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);

        for (auto &tim : registered_pwm_tim)
        {
            if (tim.inv_ch == Inverse_Ch::P)
            {
                HAL_TIM_PWM_Stop(tim_handl_table[tim.timer], tim.channel);
            }

            if (tim.inv_ch == Inverse_Ch::N)
            {
                HAL_TIMEx_PWMN_Stop(tim_handl_table[tim.timer], tim.channel);
            }
        }
    }
    else
    {
        mes.clear();
        add_str(mes, "Incorrect command\n", tx_uart_queue);
        push_with_timeout(tx_uart_queue, mes, 100);
        return;
    }
}