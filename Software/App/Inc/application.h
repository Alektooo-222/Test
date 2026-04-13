#ifndef __APPLICATION_H
#define __APPLICATION_H

#include "main.h"
#include "etl/queue_spsc_atomic.h"
#include "etl/string.h"

void CommandManager();

inline constexpr size_t SIZE_TX_QUEUE = 16;
inline constexpr size_t SIZE_RX_MESS = 64;
inline constexpr size_t SIZE_ELEMENT_TX_QUEUE = 64;

/* using FilterFunc = uint32_t(*)(etl::vector<uint32_t, 100> &vect);

struct Registered_measure_pin
{
    Pin_name &pin;
    etl::vector<uint32_t, 100> array;
    uint8_t avg;
    FilterFunc filter;
    uint32_t timeout_ms;
}; */
using string_rx_mess = etl::string<SIZE_RX_MESS>;
using CommandHandler = void (*)(string_rx_mess &str);
using StringTxQueue = etl::string<SIZE_ELEMENT_TX_QUEUE>;

extern etl::queue_spsc_atomic<StringTxQueue, SIZE_TX_QUEUE> tx_queue;
extern etl::queue_spsc_atomic<StringTxQueue, SIZE_TX_QUEUE> tx_usb_queue;

#endif