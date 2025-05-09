#ifndef BOTAO_INIT_H // Guardas de inclusão
#define BOTAO_INIT_H

// Declaração de bibliotecas
#include "pico/stdlib.h"
#include "stdbool.h"

// Mensagem para o monitoramento dos botões
extern char mensagem_botao_A[50];
extern char mensagem_botao_B[50];

// Função para configurar os botões
void iniciar_botoes();

// Função de monitoramento
void monitorar_estado_botao();

#endif