#include "main.h"
#include "application_measure.h"
#include "application_pwm.h"
#include "tim.h"

extern uint32_t software_counter_pwm;
extern uint32_t software_counter_ic;

extern USBD_HandleTypeDef USBD_Device;

extern UART_HandleTypeDef UartHandle;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    uint32_t buffsize;
    USBD_StatusTypeDef ret1;
    USBD_StatusTypeDef ret2;
    uint32_t count_err_1 = 0;
    uint32_t count_err_2 = 0;

    if (htim->Instance == TIM7)
    {
        htim->State = HAL_TIM_STATE_READY;

        measure_pin.number_measurement++;

        if (software_counter_ic > 0)
        {
            measure_pin.array.push_back(software_counter_ic);
            measure_pin.last_pulse_time = HAL_GetTick();
        } /* else if (software_counter_ic == 0)
         {} */

        software_counter_ic = 0;

        if (measure_pin.number_measurement < measure_pin.avg)
        {
            HAL_TIM_Base_Start_IT(htim);
        }
        else
        {
            measure_pin.measurement_state = StateMeasurement::MEASURMENT_COMPLETED;
        }
    }

    if (htim->Instance == TIM6)
    {
        software_counter_pwm++;

        for (auto &sw_pwm_pin : registered_sw_pins)
        {
            uint32_t phase = software_counter_pwm % sw_pwm_pin.period;

            if (phase < sw_pwm_pin.pulse_t)
            {
                sw_pwm_pin.pin.port->BSRR = sw_pwm_pin.pin.pin_number;
            }
            else
            {
                sw_pwm_pin.pin.port->BSRR = sw_pwm_pin.pin.pin_number << 16U;
            }
        }
    }

    if (htim->Instance == TIM3)
    {
        if (!tx_queue.empty())
        {
            auto &str = tx_queue.front();

            HAL_UART_Transmit_DMA(&UartHandle, reinterpret_cast<const uint8_t *>(str.c_str()), str.size());
        }

        /* ДОРАБОТАТЬ ОТПРАВКУ СООБЩЕНИЙ ПО USB */
        if (!tx_usb_queue.empty())
        {
            auto &str = tx_queue.front();

            uint8_t *data = reinterpret_cast<uint8_t *>(const_cast<char *>(str.c_str()));

            ret1 = static_cast<USBD_StatusTypeDef>(
                USBD_CDC_SetTxBuffer(&USBD_Device, data, str.size()));

            ret2 = static_cast<USBD_StatusTypeDef>(USBD_CDC_TransmitPacket(&USBD_Device));
        }
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    measure_pin.number_measurement++;

    uint32_t ccr = HAL_TIM_ReadCapturedValue(htim, measure_pin.tim_measure.channel);

    if (measure_pin.number_measurement <= measure_pin.avg + 1)
    {
        measure_pin.array.push_back(ccr);
        measure_pin.last_pulse_time = HAL_GetTick();
    }
    else
    {
        HAL_TIM_IC_Stop_IT(htim, measure_pin.tim_measure.channel);
        measure_pin.measurement_state = StateMeasurement::MEASURMENT_COMPLETED;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (htim7.State == HAL_TIM_STATE_BUSY)
    {
        software_counter_ic++;
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (!tx_queue.empty())
    {
        tx_queue.pop();
    }
}

extern "C" void cdc_transmit_cplt(void)
{
    if (!tx_usb_queue.empty())
    {
        tx_usb_queue.pop();
    }
    /* return (0); */
}