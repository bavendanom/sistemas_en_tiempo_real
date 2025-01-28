/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h"
#include "driver/ledc.h"

#include "NTC_library/include/NTC_library.h"
#include "ADC_library/include/ADC_library.h"
#include "led_RGB_LIBRARY/include/led_RGB_library.h"

// Definicion de led 1 
#define LEDC_OUTPUT_IO_RED_NTC       (21)   // GPIO para el LED rojo
#define LEDC_OUTPUT_IO_GREEN_NTC     (19)  // GPIO para el LED verde
#define LEDC_OUTPUT_IO_BLUE_NTC      (18)   // GPIO para el LED azul

// Definicion de led 2  
#define LEDC_OUTPUT_IO_RED_POT       (13)   // GPIO para el LED rojo
#define LEDC_OUTPUT_IO_GREEN_POT     (12)  // GPIO para el LED verde
#define LEDC_OUTPUT_IO_BLUE_POT      (14)   // GPIO para el LED azul


// Duty inicial para leds
#define LEDC_DUTY_INITAL          (0)   // Duty inicial (apagado)Set duty to 50%. (2 ** 13) * 50% = 4096


static int adc_raw[2][10];
static int voltage[2][10];

    
// Estructura para configurar el ADC 1 en el canal 4 
adc_config_t adc1_config_ch4 = {
    .channel = ADC_CHANNEL_4,
    .attenuation = ADC_ATTEN_DB_12,
    .cali_handle = NULL,
    .do_calibration = false
};

// Estructura para configurar el ADC 1 en el canal 4 
adc_config_t adc1_config_ch5 = {
    .channel = ADC_CHANNEL_5,
    .attenuation = ADC_ATTEN_DB_12,
    .cali_handle = NULL,
    .do_calibration = false
};

// Estructura para configurar el LED RGB que funciona deacuerdo a la temperatura leida por la NTC
led_RGB rgb_ntc = {
        .led_red = {
            .gpio_num    = LEDC_OUTPUT_IO_RED_NTC ,
            .channel     = LEDC_CHANNEL_0,
            .duty        = LEDC_DUTY_INITAL
        },
        .led_green = {
            .gpio_num    = LEDC_OUTPUT_IO_GREEN_NTC ,
            .channel     = LEDC_CHANNEL_1,
            .duty        = LEDC_DUTY_INITAL
        },
        .led_blue = {
            .gpio_num    = LEDC_OUTPUT_IO_BLUE_NTC,
            .channel     = LEDC_CHANNEL_2,
            .duty        = LEDC_DUTY_INITAL
        }
};

// Estructura para configurar el LED RGB que funciona con el potenciometro 
led_RGB rgb_pot = {
        .led_red = {
            .gpio_num    = LEDC_OUTPUT_IO_RED_POT,
            .channel     = LEDC_CHANNEL_3,
            .duty        = LEDC_DUTY_INITAL
        },
        .led_green = {
            .gpio_num    = LEDC_OUTPUT_IO_GREEN_POT,
            .channel     = LEDC_CHANNEL_4,
            .duty        = LEDC_DUTY_INITAL
        },
        .led_blue = {
            .gpio_num    = LEDC_OUTPUT_IO_BLUE_POT,
            .channel     = LEDC_CHANNEL_5,
            .duty        = LEDC_DUTY_INITAL
        }
};

// Función para inicializar un LED RGB
static void rgb_init(led_RGB rgb) {
    // Inicializar timers de cada LED
    ledc_init_timer();
    ledc_initialize_rgb(rgb);
}

void app_main(void)
{
    config_unit init_adc_unit = init_adc(ADC_UNIT_1);
    init_adc_ch(&adc1_config_ch4, init_adc_unit);
    init_adc_ch(&adc1_config_ch5, init_adc_unit);
    rgb_init(rgb_ntc);
    rgb_init(rgb_pot);
    rgb_set_color(rgb_ntc, 50, 0, 0); // Rojo
    rgb_set_color(rgb_pot, 0, 50, 0); // green
    while (1) {
        
        // Leer ADC
        read_adc_raw(&adc1_config_ch4, &adc_raw[0][0]);
        ESP_LOGI("main", "Valor raw del ADC_4: %d", adc_raw[0][0]);
        // adc 2 
        read_adc_raw(&adc1_config_ch5, &adc_raw[0][1]);
        ESP_LOGI("main", "Valor raw del ADC_ch5: %d", adc_raw[0][1]);


        
        
        read_voltage(&adc1_config_ch4, adc_raw[0][0], &voltage[0][0]);
        ESP_LOGI("main", "Valor voltage del ADC: %d", voltage[0][0]);
        // Convertir el voltaje medido de milivoltios a voltios
        float voltage_out = voltage[0][0] / 1000.0; // Convertir de mV a V
        
        // Convertir el voltaje medido de milivoltios a voltios 
        //float voltage_out = vol_out / 1000.0; // Convertir de mV a V 
        float R_fija = 100.0; // Resistencia fija en ohmios 
        float V_cc = 3.3; // Voltaje de referencia

         // Usar funciones de la biblioteca 
        float R_NTC = res_ntc(voltage_out, R_fija, V_cc); 
        float T_Celsius = tem_cel(R_NTC); 

        ESP_LOGI("main","Resistencia NTC: %.2f ohmios, Temperatura: %.2f °C", R_NTC, T_Celsius); 
        vTaskDelay(pdMS_TO_TICKS(1000));  
    }
    deinit_adc(&adc1_config_ch5);
    deinit_adc(&adc1_config_ch4);  
}
