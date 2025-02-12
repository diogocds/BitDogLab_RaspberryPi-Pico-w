#ifndef LED_RGB_H
#define LED_RGB_H

#include "pico/stdlib.h"

void rgb_led();
void set_led_color(uint red_pin, uint green_pin, uint blue_pin, bool R, bool G, bool B); // Função para configurar a cor do LED RGB

#endif // LED_RGB_H
