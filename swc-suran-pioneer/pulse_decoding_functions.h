#ifndef PULSE_DECODING_FUNCTIONS_H
#define PULSE_DECODING_FUNCTIONS_H

#include <stdio.h>
#include "pico/stdlib.h"

#define MAX_SUSTAIN_TIME 125000

//preambulo de boton tiempo central 5181us
#define MIN_BUTTON_PREAMBLE_HIGH_EXPECTED 5100
#define MAX_BUTTON_PREAMBLE_HIGH_EXPECTED 5260

//preambulo de sustain tiempo central 2624us
#define MIN_SUSTAIN_PREAMBLE_EXPECTED 2500
#define MAX_SUSTAIN_PREAMBLE_EXPECTED 2750

//low bit tiempo central 1981us
#define MIN_LOW_BIT_EXPECTED 1900
#define MAX_LOW_BIT_EXPECTED 2060

//high bit tiempo central 701us
#define MIN_HIGH_BIT_EXPECTED 600
#define MAX_HIGH_BIT_EXPECTED 800

#define TIMEOUT_USEC 11000   // Límite de espera de 11 ms (11000 us)

#define NO_BUTTON_PRESSED 0

#define VOL_PLUS_PRESSED 0xBE177F80
#define VOL_MINUS_PRESSED 0xBE17FF00
#define REV_PRESSED 0xBE17AF50
#define FF_PRESSED 0xBE172FD0
#define OK_PRESSED 0xBE177788
#define SRC_PRESSED 0xBE17F708

#define ERROR 0xDEADDEAD

// Pone en '1' el bit en la posición indicada (0 a 31)
#define bit_set(var, bit)    ((var) |= (1UL << (bit)))

// Pone en '0' el bit en la posición indicada (0 a 31)
#define bit_clear(var, bit)  ((var) &= ~(1UL << (bit)))

// (Opcional) Lee el valor del bit (devuelve 1 o 0)
#define bit_read(var, bit)   (((var) >> (bit)) & 0x01)

/**
 * @brief Espera a que el pin pase a HIGH y mide su duración en microsegundos.
 * 
 * @return uint32_t Duración del pulso en HIGH (us), o >= 11000 si ocurre un timeout.
 */
/**
 * @brief Espera a que el pin pase a HIGH y mide su duración en microsegundos.
 * 
 * @return uint64_t Duración del pulso en HIGH (us), o >= 11000 si ocurre timeout
 *                  tanto esperando el HIGH como si se mantiene en HIGH.
 */
uint64_t usec_high(int pin);

uint32_t get_pressed_button(int pin);

#endif //PULSE_DECODING_FUNCTIONS_H