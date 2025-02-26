#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include <math.h>
#include "led_RGB_library.h"

#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz


/**
 * @brief Configura el temporizador del controlador LEDC para generar PWM.
 * 
 * @param l Estructura 'led' que contiene la configuración del LED (incluyendo el temporizador).
 * @return esp_err_t Devuelve ESP_OK si la configuración es exitosa, otro código de error en caso contrario.
 */
esp_err_t ledc_init_timer(led l) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,       // Modo de velocidad (LEDC_HIGH_SPEED_MODE o LEDC_LOW_SPEED_MODE)
        .duty_resolution  = LEDC_DUTY_RES,             // Resolución de PWM en bits
        .timer_num        = l.timmer,                  // Especifica el número del temporizador a usar
        .freq_hz          = LEDC_FREQUENCY,            // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK              // Configura automáticamente la fuente de reloj
    };
    return ledc_timer_config(&ledc_timer);
}
/**
 * @brief Configura un canal del controlador LEDC para generar PWM en un LED específico.
 * 
 * @param l Estructura 'led' que contiene la configuración del LED (GPIO, canal, temporizador, duty cycle, etc.).
 * @return esp_err_t Devuelve ESP_OK si la configuración es exitosa, otro código de error en caso contrario.
 */
esp_err_t ledc_init_channel(led l) {
    ledc_channel_config_t ledc_channel = {  
        .channel        = l.channel,                  // Canal del controlador LEDC asignado al LED
        .speed_mode     = LEDC_LOW_SPEED_MODE,        // Selección del modo de velocidad (baja en este caso)
        .timer_sel      = l.timmer,                   // Temporizador asignado al canal
        .intr_type      = LEDC_INTR_DISABLE,          // Desactiva interrupciones en este canal
        .gpio_num       = l.gpio_num,                 // Número de GPIO donde está conectado el LED
        .duty           = l.duty,                     // Duty cycle inicial del LED
        .hpoint         = 0,                          // Punto de inicio del pulso PWM (0 = inicio inmediato)
        .flags.output_invert = l.flag_output_invert   // Invierte la señal PWM si está habilitado
    };
    return ledc_channel_config(&ledc_channel);        // Aplica la configuración del canal LEDC
}


esp_err_t ledc_initialize_rgb(led_RGB rgb){
    led leds[] = {rgb.led_red, rgb.led_green, rgb.led_blue};
    for (int i = 0; i < 3; i++) {
        ledc_init_channel(leds[i]); // Configura cada canal del LED RGB
    }
    return ESP_OK;
}


static uint32_t set_duty_porcent(uint32_t duty_porcent) {
    return ((1 << LEDC_TIMER_13_BIT) * duty_porcent) / 100; // Escala el porcentaje a 13 bits
}

static esp_err_t set_led_color(led l, uint32_t duty) {
    if (duty > 100) {
        return ESP_ERR_INVALID_ARG; // Retorna error si el duty es inválido  
    }
    // Calcular y configurar el duty
    uint32_t scaled_duty = set_duty_porcent(duty);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, l.channel, scaled_duty);

   
    return ledc_update_duty(LEDC_LOW_SPEED_MODE, l.channel);
}

esp_err_t rgb_set_color(led_RGB rgb, uint32_t duty_red, uint32_t duty_green, uint32_t duty_blue) {
    set_led_color(rgb.led_red, duty_red);      // Configura el duty cycle del LED rojo
    set_led_color(rgb.led_green, duty_green);  // Configura el duty cycle del LED verde
    set_led_color(rgb.led_blue, duty_blue);    // Configura el duty cycle del LED azul
    return ESP_OK;
}