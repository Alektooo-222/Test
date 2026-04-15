#ifndef __APPLICATION_H
#define __APPLICATION_H

#include "main.h"
#include "etl/queue_spsc_atomic.h"
#include "etl/string.h"

void CommandManager(uint8_t *);

inline constexpr size_t SIZE_TX_QUEUE = 16;
inline constexpr size_t SIZE_RX_MESS = 64;
inline constexpr size_t SIZE_ELEMENT_TX_QUEUE = 64;

using string_rx_mess = etl::string<SIZE_RX_MESS>;
using CommandHandler = void (*)(string_rx_mess &str);
using StringTxQueue = etl::string<SIZE_ELEMENT_TX_QUEUE>;
using TypeTxQueue = etl::queue_spsc_atomic<StringTxQueue, SIZE_TX_QUEUE>;

extern TypeTxQueue tx_uart_queue;
extern TypeTxQueue tx_usb_queue;

#endif