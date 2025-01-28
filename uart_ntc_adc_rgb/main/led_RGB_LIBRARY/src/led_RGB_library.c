#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include <math.h>
#include "led_RGB_library.h"

#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz


// Prepare and then apply the LEDC PWM timer configuration
esp_err_t ledc_init_timer(void) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,// Modo de velocidad (LEDC_HIGH_SPEED_MODE o LEDC_LOW_SPEED_MODE)
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    return ledc_timer_config(&ledc_timer);
}

esp_err_t ledc_init_channel(led l) {
    ledc_channel_config_t ledc_channel = {  
        .channel        = l.channel,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = l.gpio_num,
        .duty           = l.duty,
        .hpoint         = 0,
        .flags.output_invert = 1 /*!< Enable (1) or disable (0) gpio output invert */
    };
    return ledc_channel_config(&ledc_channel);
}


esp_err_t ledc_initialize_rgb(led_RGB rgb){
    led leds[] = {rgb.led_red, rgb.led_green, rgb.led_blue};
    for (int i = 0; i < 3; i++) {
        ledc_init_channel(leds[i]);
    }
    return ESP_OK;
}


uint32_t set_duty_porcent(uint32_t duty_porcent) {
    return ((1 << LEDC_TIMER_13_BIT) * duty_porcent) / 100;
}

esp_err_t set_led_color(led l, uint32_t duty) {
    if (duty > 100) {
        return ESP_ERR_INVALID_ARG; // Retorna error si el duty es inválido
    }
    // Calcular y configurar el duty
    uint32_t scaled_duty = set_duty_porcent(duty);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, l.channel, scaled_duty);
    return ledc_update_duty(LEDC_LOW_SPEED_MODE, l.channel);
}

esp_err_t rgb_set_color(led_RGB rgb, uint32_t duty_red, uint32_t duty_green, uint32_t duty_blue) {
    set_led_color(rgb.led_red, duty_red);
    set_led_color(rgb.led_green, duty_green);
    set_led_color(rgb.led_blue, duty_blue);
    return ESP_OK;
}