#include "main.h"
#include "application_measure.h"
#include "application_transmit.h"
#include "tim.h"
#include "pins.h"
#include "etl/map.h"
#include <etl/to_string.h>
#include <etl/string.h>
#include <etl/format_spec.h>

Registered_measure_pin measure_pin;
uint32_t software_counter_pwm = 0;
uint32_t software_counter_ic = 0;

extern etl::map<uint16_t, IRQn_Type, 16> exti_irq_table;
extern etl::map<TIM_TypeDef *, IRQn_Type, 16> tim_cc_irq_table;
extern etl::map<TIM_TypeDef *, TIM_HandleTypeDef *, 12> tim_handl_table;
extern etl::map<Pin_configured, const char *, 16> staus_table;

StatusConfigMeasure registration_measure_pin(const Pin_name &pin)
{
    measure_pin.pin = pin;

    return StatusConfigMeasure::REGISTRATION_PIN_OK;
}

StatusConfigMeasure config_software_measure(const Pin_name &pin)
{
    StatusConfigMeasure status_registration_measure_pin;

    GPIO_InitTypeDef GPIO_InitStruct = {};

    GPIO_InitStruct.Pin = pin.pin_number;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(pin.port, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(exti_irq_table[pin.pin_number], 1, 0);
    // HAL_NVIC_EnableIRQ(exti_irq_table[pin.pin_number]);

    status_registration_measure_pin = registration_measure_pin(pin);

    if (status_registration_measure_pin != StatusConfigMeasure::REGISTRATION_PIN_OK)
    {
        return StatusConfigMeasure::CONFIG_SW_MEASURE_PIN_FAIL;
    }

    measure_pin.measurement_config = ConfigMeasurement::SOFTWARE_MEASUREMENT;

    return StatusConfigMeasure::CONFIG_SW_MEASURE_PIN_OK;
}

StatusConfigMeasure config_hardware_measure(const Pin_name &pin, const Peripheral_capabilities &tim)
{
    uint32_t tim_clk;

    //uint8_t bits = (tim.timer == TIM2 || tim.timer == TIM5) ? 32 : 16;
    uint8_t bits = 16;

    tim_clk = get_freq_clk_tim(tim);

    StatusConfigMeasure status_config_tim_measure = config_tim_measure(pin, tim, bits, tim_clk);

    StatusConfigMeasure status_registration_measure_pin = registration_measure_pin(pin);

    if (status_registration_measure_pin != StatusConfigMeasure::REGISTRATION_PIN_OK)
    {
        return StatusConfigMeasure::CONFIG_HW_MEASURE_PIN_FAIL;
    }

    measure_pin.measurement_config = ConfigMeasurement::HARDWARE_MEASUREMENT;

    return status_config_tim_measure;
}

void cmd_config_measure_handler(string_rx_mess &str)
{
    StringTxQueue mes("");

    size_t first_space = str.find(' ');

    size_t new_line = str.find('\n');

    uint8_t status_measure_hard = 0;
    StatusConfigMeasure status_config_software_measure;
    StatusConfigMeasure status_config_hardware_measure;

    if (first_space == string_rx_mess::npos)
    {
        add_str(mes, "Incorrect command\n", tx_uart_queue);
        push_with_timeout(tx_uart_queue, mes, 100);
        return;
    }

    if (new_line == string_rx_mess::npos)
    {
        new_line = str.size();
    }

    etl::string<10> pin_str(str, first_space + 1, new_line - first_space - 1);

    for (auto &pin : ListPins)
    {
        if (pin_str == pin.name.pin_name)
        {
            if (pin.conf == Pin_configured::NO_CONFIG)
            {
                if (pin.hardware_features.size() == 0) // Есть ли таймера, если нет то программное измерение
                {
                    status_config_software_measure = config_software_measure(pin.name);

                    if (status_config_software_measure == StatusConfigMeasure::CONFIG_SW_MEASURE_PIN_OK)
                    {
                        pin.conf = Pin_configured::MEASURE_SOFT;
                        add_str(mes, "MEASURE CONFIGURED on ", tx_uart_queue);
                        add_str(mes, pin.name.pin_name, tx_uart_queue);
                        add_str(mes, " (software)\n", tx_uart_queue);
                        push_with_timeout(tx_uart_queue, mes, 100);
                        break;
                    }
                    else if (status_config_software_measure == StatusConfigMeasure::CONFIG_SW_MEASURE_PIN_FAIL)
                    {
                        add_str(mes, "CANNOT MEASURE CONFIGURED on ", tx_uart_queue);
                        add_str(mes, pin.name.pin_name, tx_uart_queue);
                        add_str(mes, " (software)\n", tx_uart_queue);
                        push_with_timeout(tx_uart_queue, mes, 100);
                        break;
                    }
                }
                else
                {
                    for (auto &tim : pin.hardware_features)
                    {
                        if (tim_handl_table[tim.timer]->Instance == tim.timer || tim.inv_ch == Inverse_Ch::N) // Поиск свободного таймера, канал которого не инверсный
                        {
                            continue;
                        }

                        status_config_hardware_measure = config_hardware_measure(pin.name, tim);

                        if (status_config_hardware_measure == StatusConfigMeasure::CONFIG_HW_MEASURE_PIN_OK)
                        {
                            pin.conf = Pin_configured::MEASURE_HARD;
                            measure_pin.tim_measure = tim;
                            status_measure_hard = 1;
                            add_str(mes, "MEASURE CONFIGURED on ", tx_uart_queue);
                            add_str(mes, pin.name.pin_name, tx_uart_queue);
                            add_str(mes, " (hardware)\n", tx_uart_queue);
                            push_with_timeout(tx_uart_queue, mes, 100);
                            break;
                        }
                    }

                    if (!status_measure_hard)
                    {
                        status_config_software_measure = config_software_measure(pin.name);

                        if (status_config_software_measure == StatusConfigMeasure::CONFIG_SW_MEASURE_PIN_OK)
                        {
                            pin.conf = Pin_configured::MEASURE_SOFT;
                            add_str(mes, "MEASURE CONFIGURED on ", tx_uart_queue);
                            add_str(mes, pin.name.pin_name, tx_uart_queue);
                            add_str(mes, " (software)\n", tx_uart_queue);
                            push_with_timeout(tx_uart_queue, mes, 100);
                            break;
                        }
                        else if (status_config_software_measure == StatusConfigMeasure::CONFIG_SW_MEASURE_PIN_FAIL)
                        {
                            add_str(mes, "CANNOT MEASURE CONFIGURED on ", tx_uart_queue);
                            add_str(mes, pin.name.pin_name, tx_uart_queue);
                            add_str(mes, " (software)\n", tx_uart_queue);
                            push_with_timeout(tx_uart_queue, mes, 100);
                            break;
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

uint32_t none(array_measurement &arr)
{
    return *arr.end();
}

uint32_t mean(array_measurement &arr)
{
    uint64_t sum = 0;

    etl::for_each(arr.begin(), arr.end(), [&sum](uint32_t &x)
                  { sum += x; });

    uint64_t result = static_cast<uint64_t>(static_cast<double>(sum) / static_cast<double>(arr.size()));

    return result;
}

uint32_t median(array_measurement &arr)
{
    etl::sort(arr.begin(), arr.end());

    if (arr.size() == 1)
    {
        uint32_t ret_val = arr[0];
        return ret_val;
    }

    if (arr.size() % 2 == 1)
    {
        uint32_t ret_val = arr[static_cast<size_t>(static_cast<double>(arr.size()) / 2.0)];
        return ret_val;
    }
    else
    {
        uint32_t ret_val = static_cast<uint32_t>(static_cast<double>(arr[static_cast<size_t>(static_cast<double>(arr.size()) / 2.0) - 1] + arr[static_cast<size_t>(static_cast<double>(arr.size()) / 2.0)]) / 2.0);
        return ret_val;
    }
}

uint32_t min(array_measurement &arr)
{
    return *etl::min_element(arr.begin(), arr.end());
}

uint32_t max(array_measurement &arr)
{
    return *etl::max_element(arr.begin(), arr.end());
}

etl::map<etl::string<10>, FilterFunc, 5> table_filters_func = {
    {"none", none},
    {"mean", mean},
    {"median", median},
    {"min", min},
    {"max", max},
};

void cmd_measure_params_handler(string_rx_mess &str)
{
    StringTxQueue mes("");

    size_t first_space = str.find(' ');
    size_t second_space = str.find(' ', first_space + 1);
    size_t third_space = str.find(' ', second_space + 1);
    size_t new_line = str.find('\n');

    /* etl::string<10> timeout_ms_str; */

    if (first_space == string_rx_mess::npos || second_space == string_rx_mess::npos/*  || third_space == string_rx_mess::npos */)
    {
        return;
    }

    /* if (third_space == string_rx_mess::npos)
    {
        measure_pin.timeout_ms = 1001;
    }else
    {
        etl::string<10> timeout_ms_str(str, third_space + 1, new_line - third_space - 1);
        timeout_ms_str.clear();
        timeout_ms_str += etl::string<10>(str, third_space + 1, new_line - third_space - 1);
        uint32_t timeout_ms = atoi(timeout_ms_str.c_str());
        measure_pin.timeout_ms = timeout_ms;
    } */

    if (new_line == string_rx_mess::npos)
    {
        new_line = str.size();
    }

    etl::string<4> avg_str(str, first_space + 1, second_space - first_space - 1);
    etl::string<10> filter_func_str(str, second_space + 1, third_space - second_space - 1);
    etl::string<4> timeout_ms_str(str, third_space + 1, new_line - third_space - 1);

    uint32_t avg = atoi(avg_str.c_str());
    uint32_t timeout_ms = atoi(timeout_ms_str.c_str());

    measure_pin.avg = avg;
    measure_pin.filter = table_filters_func[filter_func_str];
    measure_pin.filter_func_name = filter_func_str;
    measure_pin.timeout_ms = timeout_ms;

    add_str(mes, "MEASURE PARAMS: avg=", tx_uart_queue);
    add_str(mes, avg_str, tx_uart_queue);
    add_str(mes, ", filter=", tx_uart_queue);
    add_str(mes, measure_pin.filter_func_name, tx_uart_queue);
    add_str(mes, ", timeout=", tx_uart_queue);
    add_str(mes, timeout_ms_str, tx_uart_queue);
    add_str(mes, ", ms\n", tx_uart_queue);

    push_with_timeout(tx_uart_queue, mes, 100);
}

void cmd_freq_handler(string_rx_mess &str)
{
    StringTxQueue mes;
    mes.clear();

    string_rx_mess str_temp;
    str_temp.clear();

    uint32_t measured_freq = 0;

    if (measure_pin.measurement_config == ConfigMeasurement::SOFTWARE_MEASUREMENT)
    {
        HAL_NVIC_EnableIRQ(exti_irq_table[measure_pin.pin.pin_number]);
        software_counter_ic = 0;
        HAL_TIM_Base_Start_IT(tim_handl_table[TIM_SW_MEASURE]);

        measure_pin.measurement_state = StateMeasurement::MEASURMENT_IN_PROGRESS;

        measure_pin.start_measurement = HAL_GetTick();
        measure_pin.last_pulse_time = HAL_GetTick();

        while (measure_pin.measurement_state != StateMeasurement::MEASURMENT_COMPLETED)
        {
            if ((HAL_GetTick() - measure_pin.start_measurement) >= measure_pin.timeout_ms  && measure_pin.array.size() == 0)
            {
                add_str(mes, "TIMEOUT\n", tx_uart_queue);
                measure_pin.number_measurement = 0;
                measure_pin.array.clear();
                measure_pin.measurement_state = StateMeasurement::MEASURMENT_NONE;
                push_with_timeout(tx_uart_queue, mes, 100);
                HAL_NVIC_DisableIRQ(exti_irq_table[measure_pin.pin.pin_number]);
                return;
            }

            if ((HAL_GetTick() - measure_pin.last_pulse_time) >= measure_pin.timeout_ms  && measure_pin.array.size() == 0)
            {
                add_str(mes, "TIMEOUT\n", tx_uart_queue);
                measure_pin.number_measurement = 0;
                measure_pin.array.clear();
                measure_pin.measurement_state = StateMeasurement::MEASURMENT_NONE;
                push_with_timeout(tx_uart_queue, mes, 100);
                HAL_NVIC_DisableIRQ(exti_irq_table[measure_pin.pin.pin_number]);
                return;
            }
        }

        /* for (auto &fr : measure_pin.array)
        {
            LOGI("%d", fr);
        } */

        measured_freq = measure_pin.filter(measure_pin.array);

        /* LOGI("measured_freq: %d", measured_freq); */

        add_str(mes, "FREQ ", tx_uart_queue);
        etl::to_string(measured_freq, str_temp);
        add_str(mes, str_temp, tx_uart_queue);
        str_temp.clear();
        add_str(mes, " (avg=", tx_uart_queue);
        etl::to_string(measure_pin.avg, str_temp);
        add_str(mes, str_temp, tx_uart_queue);
        add_str(mes, ", filter=", tx_uart_queue);
        add_str(mes, measure_pin.filter_func_name, tx_uart_queue);
        add_str(mes, ", raw: ", tx_uart_queue);
        for (auto &raw_freq : measure_pin.array)
        {
            str_temp.clear();
            etl::to_string(raw_freq, str_temp);
            add_str(mes, str_temp, tx_uart_queue);
            add_str(mes, " ", tx_uart_queue);
        }

        measure_pin.number_measurement = 0;
        measure_pin.array.clear();
        measure_pin.measurement_state = StateMeasurement::MEASURMENT_NONE;
        HAL_NVIC_DisableIRQ(exti_irq_table[measure_pin.pin.pin_number]);
    }
    else if (measure_pin.measurement_config == ConfigMeasurement::HARDWARE_MEASUREMENT)
    {
        HAL_NVIC_EnableIRQ(tim_cc_irq_table[measure_pin.tim_measure.timer]);
        measure_pin.tim_measure.timer->CNT = 0;
        HAL_TIM_IC_Start_IT(tim_handl_table[measure_pin.tim_measure.timer], measure_pin.tim_measure.channel);

        measure_pin.measurement_state = StateMeasurement::MEASURMENT_IN_PROGRESS;

        measure_pin.start_measurement = HAL_GetTick();
        measure_pin.last_pulse_time = HAL_GetTick();

        while (measure_pin.measurement_state != StateMeasurement::MEASURMENT_COMPLETED)
        {
            if ((HAL_GetTick() - measure_pin.start_measurement) >= measure_pin.timeout_ms && measure_pin.array.size() == 0)
            {
                add_str(mes, "TIMEOUT\n", tx_uart_queue);
                measure_pin.number_measurement = 0;
                measure_pin.array.clear();
                measure_pin.measurement_state = StateMeasurement::MEASURMENT_NONE;
                push_with_timeout(tx_uart_queue, mes, 100);
                return;
            }

            if ((HAL_GetTick() - measure_pin.last_pulse_time) >= measure_pin.timeout_ms  && measure_pin.array.size() == 0)
            {
                add_str(mes, "TIMEOUT\n", tx_uart_queue);
                measure_pin.number_measurement = 0;
                measure_pin.array.clear();
                measure_pin.measurement_state = StateMeasurement::MEASURMENT_NONE;
                push_with_timeout(tx_uart_queue, mes, 100);
                return;
            }
        }

        array_measurement array_period;
        array_period.clear();

        for (auto it = measure_pin.array.begin() + 1; it != measure_pin.array.end(); ++it)
        {
            array_period.push_back(*it - *(it - 1));
        }

        etl::for_each(array_period.begin(), array_period.end(), [](uint32_t &x)
                      { if (x != 0) x = static_cast<uint32_t>(1000000.0 / static_cast<double>(x)); });

        measured_freq = measure_pin.filter(array_period);

        /* for (auto &fr : measure_pin.array)
        {
            LOGI("%d", fr);
        }
        LOGI("measured_freq: %d", measured_freq); */

        add_str(mes, "FREQ ", tx_uart_queue);
        etl::to_string(measured_freq, str_temp);
        add_str(mes, str_temp, tx_uart_queue);
        str_temp.clear();
        add_str(mes, " (avg=", tx_uart_queue);
        etl::to_string(measure_pin.avg, str_temp);
        add_str(mes, str_temp, tx_uart_queue);
        add_str(mes, ", filter=", tx_uart_queue);
        add_str(mes, measure_pin.filter_func_name, tx_uart_queue);
        add_str(mes, ", raw: ", tx_uart_queue);
        for (auto &raw_freq : array_period)
        {
            str_temp.clear();
            etl::to_string(raw_freq, str_temp);
            add_str(mes, str_temp, tx_uart_queue);
            add_str(mes, " ", tx_uart_queue);
        }

        measure_pin.number_measurement = 0;
        measure_pin.array.clear();
        measure_pin.measurement_state = StateMeasurement::MEASURMENT_NONE;
    }

    add_str(mes, "\n", tx_uart_queue);
    push_with_timeout(tx_uart_queue, mes, 100);

    return;
}