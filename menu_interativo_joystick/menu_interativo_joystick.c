#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "inc/joystick_led.h"
#include "inc/buzzer.h"
#include "inc/led_rgb.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
const uint JOY_UP_PIN = 26;     // Pino do joystick para movimento
const uint JOY_SELECT_PIN = 22; // Pino do joystick para selecionar

int selected_item = 0; // Item atualmente selecionado

// Atualização no número de itens do menu, agora calculado com base no array
char *text[] = {
    "1- Joystick_Led",
    "2- Buzzer",
    "3- Led_RGB"};

// Calculando o número de itens do menu automaticamente
int text_count = sizeof(text) / sizeof(text[0]);

// Função para desenhar bordas de um retângulo na tela
void draw_rect(uint8_t *ssd, int x, int y, int width, int height, uint8_t color)
{
    if (x < 0 || y < 0 || x + width > ssd1306_width || y + height > ssd1306_height)
    {
        return;
    }

    // Desenha a borda superior
    for (int i = x; i < x + width; i++)
    {
        int byte_pos = (i + (y / 8) * ssd1306_width);
        int bit_pos = y % 8;
        if (color == 0xFF)
        {
            ssd[byte_pos] |= (1 << bit_pos);
        }
        else
        {
            ssd[byte_pos] &= ~(1 << bit_pos);
        }
    }

    // Desenha a borda inferior
    for (int i = x; i < x + width; i++)
    {
        int byte_pos = (i + ((y + height - 1) / 8) * ssd1306_width);
        int bit_pos = (y + height - 1) % 8;
        if (color == 0xFF)
        {
            ssd[byte_pos] |= (1 << bit_pos);
        }
        else
        {
            ssd[byte_pos] &= ~(1 << bit_pos);
        }
    }

    // Desenha a borda esquerda
    for (int j = y; j < y + height; j++)
    {
        int byte_pos = (x + (j / 8) * ssd1306_width);
        int bit_pos = j % 8;
        if (color == 0xFF)
        {
            ssd[byte_pos] |= (1 << bit_pos);
        }
        else
        {
            ssd[byte_pos] &= ~(1 << bit_pos);
        }
    }

    // Desenha a borda direita
    for (int j = y; j < y + height; j++)
    {
        int byte_pos = ((x + width - 1) + (j / 8) * ssd1306_width);
        int bit_pos = j % 8;
        if (color == 0xFF)
        {
            ssd[byte_pos] |= (1 << bit_pos);
        }
        else
        {
            ssd[byte_pos] &= ~(1 << bit_pos);
        }
    }
}

void draw_menu(uint8_t *ssd, int selected)
{
    int y = 0;
    for (int i = 0; i < text_count; i++)
    {
        ssd1306_draw_string(ssd, 5, y, text[i]);

        if (i == selected)
        {
            // Desenha o contorno mais destacado ao redor do item selecionado
            draw_rect(ssd, 0, y, ssd1306_width, 16, 0xFF); // Borda mais grossa
        }

        y += 16; // Aumentar a altura da linha para melhor espaçamento visual
    }
}

int main()
{
    stdio_init_all();

    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Configuração dos pinos do joystick
    gpio_init(JOY_UP_PIN);
    gpio_set_dir(JOY_UP_PIN, GPIO_IN);
    gpio_pull_up(JOY_UP_PIN);

    gpio_init(JOY_SELECT_PIN);
    gpio_set_dir(JOY_SELECT_PIN, GPIO_IN);
    gpio_pull_up(JOY_SELECT_PIN);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init(i2c1, I2C_SDA, I2C_SCL);

    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    while (1)
    {
        // Limpa o display
        memset(ssd, 0, ssd1306_buffer_length);

        // Lê o joystick e altera a seleção
        if (!gpio_get(JOY_UP_PIN))
        {
            selected_item = (selected_item + 1) % text_count; // Aumenta para o próximo item, e volta ao início ao passar do último
            sleep_ms(100);                                    // Debounce
        }

        // Desenha o menu com o item selecionado
        draw_menu(ssd, selected_item);

        if (!gpio_get(JOY_SELECT_PIN))
        {
            switch (selected_item)
            {
            case 0:
                joystick_led_function(); // Chama a função do LED do joystick
                break;

            case 1:
                play_star(); // Executa a música do Star Wars
                break;

            case 2:
                rgb_led(); // Chama a função do LED RGB
                break;
            }

            sleep_ms(100); // Debounce
        }

        // Renderiza o conteúdo no display
        render_on_display(ssd, &frame_area);
        sleep_ms(100); // Atraso para evitar loop muito rápido
    }
}