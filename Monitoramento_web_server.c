#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include "string.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "iniciar_botao.h"
#include "monitorar_temp.h"

#define WIFI_SSID "RAFAEL"
#define WIFI_PASSWORD "rafael120905!"


char resposta_http[1024];



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




static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if(!p) {
        // Cliente fechou a conexão
        tcp_close(tpcb);
        return ERR_OK;
    }

    monitorar_estado_botao();

    criar_resposta_http();

    tcp_write(tpcb, resposta_http, strlen(resposta_http), TCP_WRITE_FLAG_COPY);

    pbuf_free(p);

    return ERR_OK;


}

static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);
    return ERR_OK;
}





int main()
{
    stdio_init_all();
    sleep_ms(10000);
    printf("Iniciando servidor HTTP\n");

    if(cyw43_arch_init()) {
        printf("Erro ao inicializar o wifi");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }

    if(netif_default) {
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }

    struct tcp_pcb *server = tcp_new();

    if(!server) {
        printf("Falha ao criar servidor TCP\n");
        return -1;
    }

    if(tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Falha ao associar servidor a porta 80\n");
        return -1;
    }

    server = tcp_listen(server);
    tcp_accept(server, connection_callback);

    printf("Servidor ouvindo na porta 80\n");

    iniciar_botoes();
    iniciar_temp();

    while (true) {
        cyw43_arch_poll();
        monitorar_estado_botao();
        sleep_ms(1000);
    }

    cyw43_arch_deinit();
    return 0;
}
