// Declaração de bibliotecas
#include <stdio.h>
#include "monitorar_temp.h"
#include "hardware/adc.h"


// Configuração do sensor de temperatura
void iniciar_temp() {
    adc_init(); // Inicializa o ADC (Conversor Analógico-Digital)
    adc_set_temp_sensor_enabled(true); // Habilita o sensor de temperatura
    adc_select_input(4); // Seleciona o canal 4 do ADC - Sensor de temperatura interno
}

// Monitora e retorna a temperatura
float monitorar_temp() {
    uint16_t raw_value = adc_read();
    const float conversion_factor = 3.3f / (1 << 12);
    float temperatura = 27.0f - ((raw_value * conversion_factor) - 0.706f) / 0.001721f;
    return temperatura;
}