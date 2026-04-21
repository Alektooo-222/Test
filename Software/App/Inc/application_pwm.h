#ifndef __APP_PWM
#define __APP_PWM

#include "etl/vector.h"
#include "pins.h"

struct pin_software_pwm
{
    const Pin_name &pin;
    uint32_t period;
    uint32_t pulse_t;
};

extern etl::vector<pin_software_pwm, 20> registered_sw_pins;
extern etl::vector<Peripheral_capabilities, 20> registered_pwm_tim;

void cmd_config_gen_handler(string_rx_mess &str);
void cmd_gen_handler(string_rx_mess &str);

#endif