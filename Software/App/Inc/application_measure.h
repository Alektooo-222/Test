#ifndef __APP_MEAS
#define __APP_MEAS

#include "status_conf.h"
#include "application.h"
#include "etl/string.h"
#include "pins.h"



using array_measurement = etl::vector<uint32_t, 200>;
using FilterFunc = uint32_t (*)(array_measurement &vect);

typedef struct
{
    Pin_name pin;
    Peripheral_capabilities tim_measure;
    array_measurement array;
    uint8_t avg;
    uint32_t start_measurement = 0;
    uint32_t last_pulse_time = 0;
    uint8_t number_measurement = 0;
    FilterFunc filter;
    etl::string<10> filter_func_name;
    uint32_t timeout_ms = 1001;
    volatile  StateMeasurement measurement_state = StateMeasurement::MEASURMENT_NONE;
    volatile  ConfigMeasurement measurement_config = ConfigMeasurement::SOFTWARE_MEASUREMENT;
} Registered_measure_pin;

void cmd_config_measure_handler(string_rx_mess &str);
void cmd_measure_params_handler(string_rx_mess &str);
void cmd_freq_handler(string_rx_mess &str);

extern Registered_measure_pin measure_pin;
#endif