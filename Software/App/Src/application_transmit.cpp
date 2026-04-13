#include "main.h"
/* #include "application.h" */
#include "application_transmit.h"

void add_str(StringTxQueue &to_add, const string_rx_mess &from_add)
{
    if (to_add.size() + from_add.size() > SIZE_ELEMENT_TX_QUEUE)
    {
        push_with_timeout(tx_queue, to_add, 100);
        to_add.clear();
        to_add += from_add;
    }
    else
    {
        to_add += from_add;
    }
}

void add_str(StringTxQueue &to_add, const char *from_add)
{
    if (to_add.size() + strlen(from_add) > SIZE_ELEMENT_TX_QUEUE)
    {
        push_with_timeout(tx_queue, to_add, 100);
        to_add.clear();
        to_add += from_add;
    }
    else
    {
        to_add += from_add;
    }
}