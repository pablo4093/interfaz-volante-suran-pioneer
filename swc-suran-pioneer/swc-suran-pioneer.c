// decodificacion del SWC de la volkswagen suran
// son 32 bits donde se pueden diferenciar los 1s y los 0s midiendo el ancho de pulso del high, lo mismo que del preambulo
// donde en high hay dos preambulos diferenciables, el del boton presionado por primera vez y el de la retencion de boton
// que es un pulso mas corto

#include <stdio.h>
#include "pico/stdlib.h"
#include "pulse_decoding_functions.h"

#define OUT1 0
#define OUT2 1
#define OUT3 2
#define OUT4 3
#define OUT5 4
#define OUT6 5
#define OUT7 6
#define GND 7

#define INPUT_PIN 14 // Cambia este GPIO según la conexión de tu RP2040-Zero

// Arreglo auxiliar para iterar los pines cómodamente
static const uint8_t output_pins[7] = {OUT1, OUT2, OUT3, OUT4, OUT5, OUT6, OUT7};

/**
 * @brief Inicializa los 6 pines de salida como GPIO OUTPUT y los deja en LOW.
 */
void outputs_init(void)
{
    for (int i = 0; i < 7; i++)
    {
        gpio_init(output_pins[i]);
        gpio_set_dir(output_pins[i], GPIO_IN); //flotantes a 3v3 del estereo

    }
}

/**
 * @brief Activa una sola salida (1 a 6) y apaga todas las demás.
 *        Si se pasa 0 (o cualquier valor fuera de 1-6), se apagan todas las salidas.
 *
 * @param active_out Número de salida a activar (0 = todas apagadas, 1 para OUT1, ..., 6 para OUT6)
 */
void set_active_output(uint8_t active_out)
{
    for (int i = 0; i < 7; i++)
    {
        // (i + 1) mapea el índice 0->OUT1, 1->OUT2 ... 5->OUT6
        if ((i + 1) == active_out)
        {
            gpio_set_dir(output_pins[i], GPIO_OUT); //pongo el pin como salida
            gpio_put(output_pins[i], 0); // y lo conecto a masa
        }
        else
        {
            gpio_set_dir(output_pins[i], GPIO_IN); //apago, lo pongo flotante a 3v3 del estereo
        }
    }
}

uint32_t key_pressed;
uint32_t release_sent = 0;
int main()
{
    stdio_init_all();
    gpio_init(INPUT_PIN);
    gpio_set_dir(INPUT_PIN, GPIO_IN);
    gpio_pull_up(INPUT_PIN);

    gpio_init(GND);
    gpio_set_dir(GND, GPIO_IN);

    outputs_init();

    sleep_ms(2000);
    printf("Pablo Ibaceta - Iniciando.\n");
    while (true)
    {
        key_pressed = get_pressed_button(INPUT_PIN);
        switch (key_pressed)
        {
        case VOL_MINUS_PRESSED:
            printf("VOL- PRESIONADO\n");
            release_sent = 0;
            set_active_output(5);
            break;

        case VOL_PLUS_PRESSED:
            printf("VOL+ PRESIONADO\n");
            release_sent = 0;
            set_active_output(6);
            break;

        case FF_PRESSED:
            printf("FF PRESIONADO\n");
            release_sent = 0;
            set_active_output(4);
            break;

        case REV_PRESSED:
            printf("REV PRESIONADO\n");
            release_sent = 0;
            set_active_output(3);
            break;

        case SRC_PRESSED:
            printf("SRC PRESIONADO\n");
            release_sent = 0;
            set_active_output(1);
            break;

        case OK_PRESSED:
            printf("OK PRESIONADO\n");
            release_sent = 0;
            set_active_output(2);
            break;

        case ERROR:
            printf("ERROR FRAME\n");
            release_sent = 0;
            break;

        case NO_BUTTON_PRESSED:
        default:
            set_active_output(0);
            if (release_sent == 0)
            {
                release_sent = 1;
                printf("RELEASED.\n");
            }
            break;
        }
    }
}
