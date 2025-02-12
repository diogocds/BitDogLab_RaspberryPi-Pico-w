#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"

void pwm_init_buzzer(uint pin);
void play_star(); // Função para emitir um beep do buzzer

#endif // BUZZER_H