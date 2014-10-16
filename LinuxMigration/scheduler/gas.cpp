#include "gas.h"

int init_gas_sensor (void){
    int status = 0;

    status = set_gas_pwm();
    return status;
}

unsigned int read_gas_sensor (void){
    unsigned int gas_value = 0;

    gas_value = read_gas_gpio();
    return gas_value;
}
