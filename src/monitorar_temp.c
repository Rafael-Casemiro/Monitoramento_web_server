#include <stdio.h>
#include "monitorar_temp.h"
#include "hardware/adc.h"


char mensagem_temperatura[50];

void iniciar_temp() {
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
}


float monitorar_temp() {
    uint16_t raw_value = adc_read();
    const float conversion_factor = 3.3f / (1 << 12);
    float temperatura = 27.0f - ((raw_value * conversion_factor) - 0.706f) / 0.001721f;
    return temperatura;
}