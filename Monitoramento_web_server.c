#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include "string.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"

#define WIFI_SSID "RAFAEL"
#define WIFI_PASSWORD "rafael120905!"

#define BTA 5
#define BTB 6

char mensagem_botao_A[50] = "Nenhum evento detectado no Botão A";
char mensagem_botao_B[50] = "Nenhum evento detectado no Botão B";
char mensagem_temperatura[50] = "";

char resposta_http[1024];

volatile bool monitoramento_botao_A = false;
volatile bool monitoramento_botao_B = false;

void criar_resposta_http() {

    adc_select_input(4);
    uint16_t raw_value = adc_read();
    const float conversion_factor = 3.3f / (1 << 12);
    float temperatura = 27.0f - ((raw_value * conversion_factor) - 0.706f) / 0.001721f;

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

    gpio_init(BTA);
    gpio_set_dir(BTA, GPIO_IN);
    gpio_pull_up(BTA);

    gpio_init(BTB);
    gpio_set_dir(BTB, GPIO_IN);
    gpio_pull_up(BTB);

    adc_init();
    adc_set_temp_sensor_enabled(true);

    while (true) {
        cyw43_arch_poll();
        monitorar_estado_botao();
        sleep_ms(1000);
    }

    cyw43_arch_deinit();
    return 0;
}
