#ifndef BOTAO_INIT_H
#define BOTAO_INIT_H

#include "pico/stdlib.h"
#include "stdbool.h"

extern char mensagem_botao_A[50];
extern char mensagem_botao_B[50];

void iniciar_botoes();
void monitorar_estado_botao();

#endif