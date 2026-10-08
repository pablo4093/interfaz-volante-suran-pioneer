#include <stdio.h>
#include "pico/stdlib.h"
#include "pulse_decoding_functions.h"

uint64_t usec_high(int pin)
{
    uint64_t start_wait = time_us_64();

    // 1. Espera bloqueante hasta que el pin pase a HIGH (Timeout de 5000 us)
    while (!gpio_get(pin))
    {
        if ((time_us_64() - start_wait) >= TIMEOUT_USEC)
        {
            return (uint64_t)(time_us_64() - start_wait); // Timeout esperando HIGH
        }
       
    }

    // 2. Inicio de la medición en HIGH
    uint64_t high_start = time_us_64();

    // 3. Mide la duración en HIGH (con proteccion si se queda pegado en HIGH)
    while (gpio_get(pin))
    {
        uint64_t high_duration = (uint64_t)(time_us_64() - high_start);

        // Si el pulso en HIGH supera los 5000 us, retoma el control y devuelve el límite
        if (high_duration >= TIMEOUT_USEC)
        {
            return high_duration;
        }

        tight_loop_contents();
    }

    // 4. Retorna la duración real del pulso HIGH
    return (uint64_t)(time_us_64() - high_start);
}

uint32_t get_pressed_button(int pin)
{
    me_canse:
    uint64_t usec_qty;
    static uint64_t sustain_start_time, now;
    static uint32_t preamble_detected = 0;
    static uint32_t _32_bits_key_detected = 0;
    static uint32_t received_code; // con esto voy a llevar el sostenimiento de la tecla tambien
    now = time_us_64();
    usec_qty = usec_high(pin);
    if (usec_qty > MIN_BUTTON_PREAMBLE_HIGH_EXPECTED && usec_qty < MAX_BUTTON_PREAMBLE_HIGH_EXPECTED)
    {
        preamble_detected = 1;printf("preamble'\n");
    }


    else if (usec_qty >= TIMEOUT_USEC && _32_bits_key_detected == 1 && now - sustain_start_time <= MAX_SUSTAIN_TIME)
    {
        return received_code; // sigo mandando el mismo codigo pero no reseteo el sustain time ya que estamos en timeout
    }
    else if (usec_qty > MIN_SUSTAIN_PREAMBLE_EXPECTED && usec_qty < MAX_SUSTAIN_PREAMBLE_EXPECTED && now - sustain_start_time <= MAX_SUSTAIN_TIME)
    { printf("sustaining");
        sustain_start_time = now;
        return received_code; // sigo mandando el mismo codigo y reseteo el sustain time
    }
    else if ( now - sustain_start_time >= MAX_SUSTAIN_TIME )
    {
        preamble_detected = 0;
        _32_bits_key_detected = 0;
        return NO_BUTTON_PRESSED;
    }
    else {
        goto me_canse;
    }
    if (preamble_detected)
    {
        received_code = 0;
        for (int i = 0; i < 32; i++)
        {
            uint32_t duracion = usec_high(pin);

            // Posición del bit (de 31 bajando a 0)
            uint8_t pos_bit = 31 - i;
            if (duracion > MIN_HIGH_BIT_EXPECTED && duracion < MAX_HIGH_BIT_EXPECTED)
            {
                bit_set(received_code, pos_bit);
            }
            else if (duracion > MIN_LOW_BIT_EXPECTED && duracion < MAX_LOW_BIT_EXPECTED)
            {
                bit_clear(received_code, pos_bit);
            }
            else
            {
                // Ocurrió timeout, la trama no está completa
                preamble_detected = 0;
                _32_bits_key_detected = 0;
                received_code = 0;
                return ERROR;
                break;
            }
            if (i == 31)
            {
                _32_bits_key_detected = 1;
                preamble_detected = 0;
                sustain_start_time = now;
                return received_code;
            }
        }
    }


}