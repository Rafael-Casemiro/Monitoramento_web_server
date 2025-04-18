#include <stdio.h>
#include <stdbool.h>
#include "iniciar_botao.h"
#include "pico/stdlib.h"

#define BTA 5
#define BTB 6

char mensagem_botao_A[50] = "Nenhum evento detectado no Botão A";
char mensagem_botao_B[50] = "Nenhum evento detectado no Botão B";

void iniciar_botoes() {
    gpio_init(BTA);
    gpio_set_dir(BTA, GPIO_IN);
    gpio_pull_up(BTA);

    gpio_init(BTB);
    gpio_set_dir(BTB, GPIO_IN);
    gpio_pull_up(BTB);
}


void monitorar_estado_botao() {
    static bool ultimo_estado_botao_A = false;
    static bool ultimo_estado_botao_B = false;
    
    bool estado_botao_A = !gpio_get(BTA);
    bool estado_botao_B = !gpio_get(BTB);

    

    if(estado_botao_A != ultimo_estado_botao_A) {
        ultimo_estado_botao_A = estado_botao_A;
        if(estado_botao_A) {
            snprintf(mensagem_botao_A, sizeof(mensagem_botao_A), "Botão A foi pressionado");
            printf("Botão A pressionado\n");
        } else {
            snprintf(mensagem_botao_A, sizeof(mensagem_botao_A), "Botão A foi solto");
            printf("Botão A foi solto\n");
        }
    }

    if(estado_botao_B != ultimo_estado_botao_B) {
        ultimo_estado_botao_B = estado_botao_B;
        if(estado_botao_B) {
            snprintf(mensagem_botao_B, sizeof(mensagem_botao_B), "Botão B foi pressionado");
            printf("Botão B foi pressionado\n");
        } else {
            snprintf(mensagem_botao_B, sizeof(mensagem_botao_B), "Botão B foi solto");
            printf("Botão B foi solto");
        }
    }
}

