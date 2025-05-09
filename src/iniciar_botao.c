// Declaração de bibliotecas
#include <stdio.h>
#include <stdbool.h>
#include "iniciar_botao.h"
#include "pico/stdlib.h"


#define BTA 5 // O botão A está conectado ao GPIO 05
#define BTB 6 // O botão B está conectado ao GPIO 06

// Mensagem para passar o estado dos botões
char mensagem_botao_A[50] = "Nenhum evento detectado no Botão A";
char mensagem_botao_B[50] = "Nenhum evento detectado no Botão B";

// Inicializa os botões e configura como tipo de entrada
void iniciar_botoes() {
    gpio_init(BTA);
    gpio_set_dir(BTA, GPIO_IN);
    gpio_pull_up(BTA);

    gpio_init(BTB);
    gpio_set_dir(BTB, GPIO_IN);
    gpio_pull_up(BTB);
}

// Função que monitora o estado dos botões
void monitorar_estado_botao() {
    // Variáveis para armazenar os últimos estados dos botões
    static bool ultimo_estado_botao_A = false;
    static bool ultimo_estado_botao_B = false;
    

    // Lê o estado atual dos pinos dos botões
    bool estado_botao_A = !gpio_get(BTA);
    bool estado_botao_B = !gpio_get(BTB);

    
    // Verifica se houve mudança no estado do botão A
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

    // Verifica se houve mudança no estado do botão B
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

