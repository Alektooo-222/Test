#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "etl/string.h"
#include "etl/algorithm.h"
#include "etl/vector.h"
#include "etl/utility.h"
#include "etl/map.h"
#include "etl/array.h"
#include "etl/numeric.h"
#include <etl/to_string.h>   
#include <etl/string.h>      
#include <etl/format_spec.h> 

#include "main.h"
#include "pins.h"
#include "tim.h"
#include "application.h"

#include "application_measure.h"
#include "application_pwm.h"
#include "application_transmit.h"

#include "dwt.h"

#define SIZE_TX_MESS 32

extern uint8_t UserRxBufferUSB[APP_RX_DATA_SIZE];
extern uint8_t UserTxBuffer[APP_TX_DATA_SIZE];
extern uint8_t UserRxBufferUART[APP_RX_DATA_SIZE];
extern uint32_t UserTxBufPtrIn;
extern uint32_t UserTxBufPtrOut;
extern const uint8_t MAX_SIZE_USB_Tx_MESS;

extern UART_HandleTypeDef UartHandle;

uint32_t UserTxBufPtrIn_local = 0;
uint32_t UserTxBufPtrOut_local = 0;

const double resolution = 0.1; // tim6 interupt every 0.1 mk sec

uint32_t count_tic = 0;

extern size_t LengthRxMess_USB;
extern size_t LengthRxMess_UART;

extern uint8_t TxCmtFlag;

extern USBD_HandleTypeDef USBD_Device;

extern etl::map<TIM_TypeDef *, TIM_HandleTypeDef *, 12> tim_handl_table;
extern etl::map<uint16_t, IRQn_Type, 16> exti_irq_table;
extern etl::map<TIM_TypeDef *, IRQn_Type, 16> tim_cc_irq_table;

TypeTxQueue tx_uart_queue;
TypeTxQueue tx_usb_queue;

void cmd_echo_handler(string_rx_mess &str);
void cmd_help_handler(string_rx_mess &str);
void cmd_pinout_handler(string_rx_mess &str);

extern Pin ListPins[50];
extern etl::map<TIM_TypeDef *, const char *, 12> tim_table;
extern etl::map<uint32_t, const char *, 4> ch_table;
extern etl::map<Pin_configured, const char *, 14> staus_table;

struct CommandEntry
{
    const etl::string<SIZE_RX_MESS> &name;
    CommandHandler handler;
};

const CommandEntry commands[] = {
    {"ECHO", cmd_echo_handler},
    {"HELP", cmd_help_handler},
    {"PINOUT", cmd_pinout_handler},
    {"CONFIG_GEN", cmd_config_gen_handler},
    {"GEN", cmd_gen_handler},
    {"CONFIG_MEASURE", cmd_config_measure_handler},
    {"MEASURE_PARAMS", cmd_measure_params_handler},
    {"FREQ", cmd_freq_handler},

};

etl::map<uint8_t *, size_t *, 2> table_len = {
    {UserRxBufferUSB, &LengthRxMess_USB},
    {UserRxBufferUART, &LengthRxMess_UART}
};

void CommandManager(uint8_t * BufPtr)
{
    etl::string<SIZE_RX_MESS> s(reinterpret_cast<char *>(BufPtr), reinterpret_cast<char *>(BufPtr) + static_cast<size_t>(*table_len[BufPtr]));
    etl::string<SIZE_RX_MESS> str_cmd_name;

    size_t pos = s.find(' ');
    if (pos != etl::string<SIZE_RX_MESS>::npos)
    {
        etl::transform(s.begin(),
                       s.begin() + pos,
                       s.begin(),
                       [](unsigned char c)
                       { return std::toupper(c); });
        str_cmd_name = {s, 0, s.find(' ')};
    }
    else
    {
        etl::transform(s.begin(),
                       s.end(),
                       s.begin(),
                       [](unsigned char c)
                       { return std::toupper(c); });
        str_cmd_name = s;
    }

    for (auto &cmd : commands)
    {
        if (cmd.name == str_cmd_name)
        {
            cmd.handler(s);
            return;
        }
    }
}

void cmd_echo_handler(string_rx_mess &str)
{
    /* while (!tx_queue.push(StringTxQueue(str)))
        ; */
    /* while (!tx_usb_queue.push(StringTxQueue(str)))
        ; */
    push_with_timeout(tx_uart_queue, str, 100);
}

void cmd_help_handler(string_rx_mess &str)
{

    StringTxQueue mes;
    /* StringTxQueue str_mes(str); */

    mes.clear();

    add_str(mes, str, tx_uart_queue);
    add_str(mes, ":", tx_uart_queue);

    uint16_t size_array_commands = CalcSizeArr(commands);
    for (auto &cmd : commands)
    {
        if (cmd.name == "HELP")
        {
            continue;
        }

        add_str(mes, " ", tx_uart_queue);
        add_str(mes, cmd.name, tx_uart_queue);

        if (size_array_commands == 2)
        {
            add_str(mes, ".\n", tx_uart_queue);
        }
        else
        {
            add_str(mes, ",", tx_uart_queue);
        }

        size_array_commands--;
        
    }

    push_with_timeout(tx_uart_queue, mes, 100);
}

void cmd_pinout_handler(string_rx_mess &str)
{
    StringTxQueue string_periph;
    string_periph.clear();

    add_str(string_periph, str, tx_usb_queue);
    add_str(string_periph, ":\n", tx_usb_queue);

    for (auto &pin : ListPins)
    {
        add_str(string_periph, pin.name.pin_name, tx_usb_queue);
        add_str(string_periph, "\n", tx_usb_queue);
        add_str(string_periph, staus_table[pin.conf], tx_usb_queue);
        add_str(string_periph, "\nPeriph: ", tx_usb_queue);

        if (pin.hardware_features.empty())
        {
            add_str(string_periph, "No TIM\n", tx_usb_queue);
        }
        else
        {
            uint32_t i = 0;
            for (auto &tim : pin.hardware_features)
            {
                if (i != 0)
                    add_str(string_periph, "        ", tx_usb_queue);
                add_str(string_periph, tim_table[tim.timer], tx_usb_queue);
                add_str(string_periph, "_", tx_usb_queue);
                add_str(string_periph, ch_table[tim.channel], tx_usb_queue);
                add_str(string_periph, "\n", tx_usb_queue);
                i++;
            }
        }

        // Отправляем одно сообщение для текущего пина
        if (!push_with_timeout(tx_usb_queue, string_periph, 100))
        {
        }
        string_periph.clear(); // готовим для следующего пина

        HAL_Delay(100);
    }
}