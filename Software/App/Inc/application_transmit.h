#ifndef __APP_TRANS
#define __APP_TRANS

#include "main.h"
#include "application.h"

template <typename Queue, typename T>
bool push_with_timeout(Queue &q, const T &item, uint32_t timeout_ms)
{
    uint32_t start = HAL_GetTick();

    while (!q.push(item))
    {
        if ((HAL_GetTick() - start) >= timeout_ms)
            return false;
    }
    return true;
}

void add_str(StringTxQueue &to_add, const string_rx_mess &from_add);
void add_str(StringTxQueue &to_add, const char *from_add);

#endif