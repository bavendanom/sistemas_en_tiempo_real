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

#define LEDC_OUTPUT_IO_RED        (5)   // GPIO para el LED rojo
#define LEDC_OUTPUT_IO_GREEN      (19)  // GPIO para el LED verde
#define LEDC_OUTPUT_IO_BLUE       (17)   // GPIO para el LED azul
#define LEDC_DUTY_INITAL          (0)   // Duty inicial (apagado) Set duty to 50%. (2 ** 13) * 50% = 4096

// Estructura para configurar el LED RGB
led_RGB mi_led_rgb = {
        .led_red = {
            .gpio_num    = LEDC_OUTPUT_IO_RED,
            .channel     = LEDC_CHANNEL_0,
            .duty        = LEDC_DUTY_INITAL
        },
        .led_green = {
            .gpio_num    = LEDC_OUTPUT_IO_GREEN,
            .channel     = LEDC_CHANNEL_1,
            .duty        = LEDC_DUTY_INITAL
        },
        .led_blue = {
            .gpio_num    = LEDC_OUTPUT_IO_BLUE,
            .channel     = LEDC_CHANNEL_2,
            .duty        = LEDC_DUTY_INITAL
        }
    };

// Función para inicializar un LED RGB
static void rgb_init(led_RGB rgb) {
    // Inicializar timers y canales de cada LED
    ledc_init_timer();
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

