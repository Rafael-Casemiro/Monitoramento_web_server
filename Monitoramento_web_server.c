// Declaração de Bibliotecas
#include <stdio.h> // Biblioteca de entrada e saída padrão
#include "pico/stdlib.h" // Biblioteca padrão do Raspberry Pi Pico
#include "hardware/adc.h" // Biblioteca para manipulação do ADC
#include "pico/cyw43_arch.h" // Controla o chip Wi-Fi CYW43 do Pico W
#include "string.h" // Biblioteca para manipulação de strings
#include "lwip/pbuf.h" // Estrutura de buffers usada para armazenar pacotes de rede
#include "lwip/tcp.h" // Biblioteca que gerencia conexões TCP/IP
#include "lwip/netif.h" // Para acessar netif_default e IP
#include "iniciar_botao.h" // Arquivo para configuração dos botões
#include "monitorar_temp.h" // Arquivo para configuração de temperatura

// Configuração da rede Wi-Fi
#define WIFI_SSID "RAFAEL"
#define WIFI_PASSWORD "rafael120905!"

// Buffer para resposta HTTP
char resposta_http[1024];


// Função para criar a resposta HTTP
void criar_resposta_http() {

   float temperatura =  monitorar_temp();

    snprintf(resposta_http, sizeof(resposta_http),
        "HTTP/1.1 200 OK\r\n"
        "Cache-Control: no-cache, no-store, must-revalidate, max-age=0\r\n"
        "Pragma: no-cache\r\n"
        "Expires: 0\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n\r\n"
        "<!DOCTYPE html>"
        "<html>\n"
        "<head>\n"
        "<title>Servidor</title>\n"
        "<meta http-equiv='refresh' content='1'/>\n"
        "<style>\n"
        "body { font-family: Arial, Helvetica, sans-serif; text-align: center }\n"
        "p { font-size: x-large; }\n"
        "</style>\n"
        "</head>\n"
        "<body>\n"
        "<h1>Estados dos Botões</h1>\n"
        "<p>Botão A:  %s</p>\n"
        "<p>Botão B: %s</p>\n"
        "<p>Temperatura Interna: %.2f &deg;C</p>\n"
        "</body>\n"
        "<script>\n"
        "setTimeout(() => { window.location.href = \"/\"; }, 1000);\n"
        "</script>\n"
        "</html>\r\n",
        mensagem_botao_A, mensagem_botao_B, temperatura
    );
}



// Função de callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if(!p) {
        // Cliente fechou a conexão
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Monitora o estado dos botões
    monitorar_estado_botao();

    // Atualiza o conteúdo da página
    criar_resposta_http();

    // Envia a resposta HTTP
    tcp_write(tpcb, resposta_http, strlen(resposta_http), TCP_WRITE_FLAG_COPY);

    // Libera o buffe recebido
    pbuf_free(p);

    // Retorna que a operação foi bem-sucedida
    return ERR_OK;


}

// Callback de conexão: associa o http_callback à conexão
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback); // Associa o callback HTTP
    // Retorna  que a operação foi bem-sucedida
    return ERR_OK;
}



// Função principal
int main()
{
    stdio_init_all(); // Inicializa o sistema de saída padrão
    iniciar_botoes(); // Inicializa as configurações dos botões
    iniciar_temp(); // Inicializa as congigurações para monitoramento de temperatura
    sleep_ms(10000);
    printf("Iniciando servidor HTTP\n");

    // Inicializa o Wi-Fi
    if(cyw43_arch_init()) {
        printf("Erro ao inicializar o wifi");
        return 1;
    }

    // Ativa o modo cliente para conectar a uma rede Wi-Fi existente
    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    // Tenta conectar ao Wi-Fi com SSID e senha fornecidos
    if(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }

    // Se a conexão foi feita com sucesso, exibe o IP
    if(netif_default) {
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }

    // Cria uma nova estrutura para o servidor
    struct tcp_pcb *server = tcp_new();

    // Verifica se houve erro na criação da estrutura do servidor
    if(!server) {
        printf("Falha ao criar servidor TCP\n");
        return -1;
    }

    // Associa o servidor à porta 80
    if(tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Falha ao associar servidor a porta 80\n");
        return -1;
    }

    server = tcp_listen(server); // Coloca o servidor em modo "escuta"
    tcp_accept(server, connection_callback); // Define a função de callback para quando a conexão for aceita

    printf("Servidor ouvindo na porta 80\n");

    
    // Loop principal
    while (true) {
        cyw43_arch_poll(); // Necessário para manter o Wi-Fi ativo
        monitorar_estado_botao(); // Monitoramento do estado dos botões
        sleep_ms(500); // Reduz o uso da CPU
    }

    cyw43_arch_deinit(); // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
    return 0;
}
