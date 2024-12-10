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

#define LEDC_OUTPUT_IO_RED        (4)   // GPIO para el LED rojo
#define LEDC_OUTPUT_IO_GREEN      (17)  // GPIO para el LED verde
#define LEDC_OUTPUT_IO_BLUE       (5)   // GPIO para el LED azul
#define LEDC_DUTY_INITAL          (0)   // Duty inicial (apagado)Set duty to 50%. (2 ** 13) * 50% = 4096


/* Warning:
 * For ESP32, ESP32S2, ESP32S3, ESP32C3, ESP32C2, ESP32C6, ESP32H2, ESP32P4 targets,
 * when LEDC_DUTY_RES selects the maximum duty resolution (i.e. value equal to SOC_LEDC_TIMER_BIT_WIDTH),
 * 100% duty cycle is not reachable (duty cannot be set to (2 ** SOC_LEDC_TIMER_BIT_WIDTH)).
 */

// Estructura para configurar el LED RGB
led_RGB mi_led_rgb = {
        .led_red = {
            .gpio_num    = LEDC_OUTPUT_IO_RED,
            .speed_mode  = LEDC_LOW_SPEED_MODE,
            .channel     = LEDC_CHANNEL_0,
            .intr_type   = LEDC_INTR_DISABLE,
            .timer_sel   = LEDC_TIMER_0,
            .duty        = LEDC_DUTY_INITAL
        },
        .led_green = {
            .gpio_num    = LEDC_OUTPUT_IO_GREEN,
            .speed_mode  = LEDC_LOW_SPEED_MODE,
            .channel     = LEDC_CHANNEL_1,
            .intr_type   = LEDC_INTR_DISABLE,
            .timer_sel   = LEDC_TIMER_0,
            .duty        = LEDC_DUTY_INITAL
        },
        .led_blue = {
            .gpio_num    = LEDC_OUTPUT_IO_BLUE,
            .speed_mode  = LEDC_LOW_SPEED_MODE,
            .channel     = LEDC_CHANNEL_2,
            .intr_type   = LEDC_INTR_DISABLE,
            .timer_sel   = LEDC_TIMER_0,
            .duty        = LEDC_DUTY_INITAL
        }
    };

// Función para inicializar un LED RGB
static void rgb_init(led_RGB rgb) {
    // LEDs individuales (rojo, verde, azul)
    led leds[] = {rgb.led_red, rgb.led_green, rgb.led_blue};

    // Inicializar timers y canales de cada LED
    for (int i = 0; i < 3; i++) {
        ledc_init_timer(leds[i]);
        ledc_init_channel(leds[i]);
    }
}

void app_main(void)
{
    // Inicializa el RGB
    rgb_init(mi_led_rgb);
    // Establece un color (por ejemplo, full rojo, sin verde, sin azul)
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

