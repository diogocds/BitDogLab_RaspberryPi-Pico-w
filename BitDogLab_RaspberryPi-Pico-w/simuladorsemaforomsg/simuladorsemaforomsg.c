#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

#define BTN_A_PIN 5

int A_state = 0; // Botão A está pressionado?

void ExibirMensagem(char *mensagem)
{
    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // Zera o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    // Limpa o buffer antes de exibir a nova mensagem
    memset(ssd, 0, ssd1306_buffer_length);
    ssd1306_draw_string(ssd, 0, 0, mensagem);

    render_on_display(ssd, &frame_area);
}

void SinalFechado()
{
    gpio_put(LED_R_PIN, 1); // LED vermelho aceso
    gpio_put(LED_G_PIN, 0); // LED verde apagado
    gpio_put(LED_B_PIN, 0); // LED azul apagado

    ExibirMensagem("SINAL FECHADO\nAGUARDE");
}

void SinalAtencao()
{
    gpio_put(LED_R_PIN, 1); // LED vermelho aceso
    gpio_put(LED_G_PIN, 1); // LED verde aceso
    gpio_put(LED_B_PIN, 0); // LED azul apagado

    ExibirMensagem("SINAL DE ATENCAO\nPREPARE-SE");
}

void SinalAberto()
{
    gpio_put(LED_R_PIN, 0); // LED vermelho apagado
    gpio_put(LED_G_PIN, 1); // LED verde aceso
    gpio_put(LED_B_PIN, 0); // LED azul apagado

    ExibirMensagem("SINAL ABERTO\nATRAVESSAR\nCOM CUIDADO");
}

int WaitWithRead(int timeMS)
{
    for (int i = 0; i < timeMS; i = i + 100)
    {
        A_state = !gpio_get(BTN_A_PIN); // Lê o botão
        if (A_state == 1)               // Botão pressionado
        {
            return 1;
        }
        sleep_ms(100); // Timeout
    }
    return 0;
}

int main()
{
    stdio_init_all(); // Inicializa os tipos stdio padrão

    // Inicializando LEDs
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    // Inicializando Botão
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);

    // Lógica do semáforo
    while (true)
    {
        SinalFechado();
        A_state = WaitWithRead(8000); // Espera com leitura do botão

        if (A_state)
        { // Botão pressionado, sai do semáforo normal
            SinalAtencao();
            sleep_ms(5000);

            SinalAberto();
            sleep_ms(10000);
        }
        else
        { // Ninguém apertou o botão- CONTINUA NO SEMAFORO NORMAL
            SinalAtencao();
            sleep_ms(2000);

            SinalAberto();
            sleep_ms(8000);
        }
    }

    return 0;
}
