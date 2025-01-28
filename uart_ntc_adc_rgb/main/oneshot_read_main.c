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
#include "freertos/semphr.h"

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

// Definir una variable global para la temperatura y un mutex
float T_Celsius = 0.0;
SemaphoreHandle_t temp_mutex; 

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

static void config_adc_unit(adc_config_t *acd_ch){
    config_unit init_adc_unit = init_adc(ADC_UNIT_1);
    init_adc_ch(acd_ch, init_adc_unit);
}

void NTC_task(void *pvParameter){
    float fixed_resistor = 100.0; // Resistencia fija en ohmios 
    float reference_voltage = 3.3; // Voltaje de referencia

    
    
    config_adc_unit(&adc1_config_ch4);

    rgb_init(rgb_ntc);
    rgb_init(rgb_pot);
    
    while(1) {
        // Leer ADC
        read_adc_raw(&adc1_config_ch4, &adc_raw[0][0]);
        read_voltage(&adc1_config_ch4, adc_raw[0][0], &voltage[0][0]);
        ESP_LOGI("adc_task", "Valor voltage del ADC: %d", voltage[0][0]);

        // Convertir el voltaje medido de milivoltios a voltios
        float voltage_out = voltage[0][0] / 1000.0; // Convertir de mV a V
        
        // Calcular resistencia NTC y temperatura
        
        float R_NTC = res_ntc(voltage_out, fixed_resistor, reference_voltage); 
        float temp_Celsius = tem_cel(R_NTC);  

        ESP_LOGI("adc_task", "Resistencia NTC: %.2f ohmios, Temperatura: %.2f °C", R_NTC, temp_Celsius);
        // Controlar el LED según la temperatura
        if (temp_Celsius >= 20 && temp_Celsius <= 30) {
            rgb_set_color(rgb_ntc, 0, 0, 50); // LED rojo
        } else if (temp_Celsius >= 30 && temp_Celsius <= 40) {
            rgb_set_color(rgb_ntc, 0, 50, 0); // LED verde
        } else if (temp_Celsius >= 40 && temp_Celsius <= 50){
            rgb_set_color(rgb_ntc, 50, 0, 0); // LED azul
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        // Esperar 1 segundo antes de la siguiente lectura
    }

    deinit_adc(&adc1_config_ch5);
    deinit_adc(&adc1_config_ch4);
    vTaskDelete(NULL);
}

void app_main(void)
{
    // Crear la tarea para leer ADC y calculos para la NTC
    xTaskCreate(&NTC_task, "NTC_task", 4096, NULL, 5, NULL);
    

    // Otras inicializaciones o tareas pueden ir aquí
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

