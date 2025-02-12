/* LEDC (LED Controller) basic example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "led_RGB_LIBRARY/include/led_RGB_library.h"

#define LEDC_OUTPUT_IO_RED        (21)   // GPIO para el LED rojo
#define LEDC_OUTPUT_IO_GREEN      (19)  // GPIO para el LED verde
#define LEDC_OUTPUT_IO_BLUE       (18)   // GPIO para el LED azul
#define LEDC_DUTY_INITAL          (0)   // Duty inicial (apagado) Set duty to 50%. (2 ** 13) * 50% = 4096

// Estructura para configurar el LED RGB
led_RGB mi_led_rgb = {
        .led_red = {
            .gpio_num    = LEDC_OUTPUT_IO_RED,
            .timmer      = LEDC_TIMER_0,
            .channel     = LEDC_CHANNEL_0,
            .duty        = LEDC_DUTY_INITAL,
            .flag_output_invert = 1
        },
        .led_green = {
            .gpio_num    = LEDC_OUTPUT_IO_GREEN,
            .timmer      = LEDC_TIMER_0,
            .channel     = LEDC_CHANNEL_1,
            .duty        = LEDC_DUTY_INITAL,
            .flag_output_invert = 1
        },
        .led_blue = {
            .gpio_num    = LEDC_OUTPUT_IO_BLUE,
            .timmer      = LEDC_TIMER_0,
            .channel     = LEDC_CHANNEL_2,
            .duty        = LEDC_DUTY_INITAL,
            .flag_output_invert = 1
        }
    };

// Función para inicializar un LED RGB
static void rgb_init(led_RGB rgb) {
    // Inicializar timers y canales de cada LED
    // Inicializa el temporizador usando solo uno de los LEDs (se asume que los tres usan el mismo timer)
    ledc_init_timer(rgb.led_red);
    ledc_initialize_rgb(rgb);
}

void app_main(void)
{
    // Inicializa el RGB
    rgb_init(mi_led_rgb);
    // Establece un color (por ejemplo, sin rojo, sin verde, azul variando)
    while (1)
    {
        int max_value = 100;
        for (int i = 0; i < max_value; i++)
        {
            rgb_set_color(mi_led_rgb, 0,0,i);
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
    }
}

