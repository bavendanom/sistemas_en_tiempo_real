/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h> 
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h"

#include "NTC_library/include/NTC_library.h"
#include "ADC_library/include/ADC_library.h"


static int adc_raw[2][10];
static int voltage[2][10];
    

adc_config_t adc1_config_ch4 = {
    .unit = ADC_UNIT_1,
    .channel = ADC_CHANNEL_4,
    .attenuation = ADC_ATTEN_DB_12,
    .handle = NULL,
    .cali_handle = NULL,
    .do_calibration = false
};

void app_main(void)
{
    init_adc(&adc1_config_ch4);

    while (1) {
        // Leer ADC
        read_adc_raw(&adc1_config_ch4, &adc_raw[0][0]);
        //float raw = adc_raw[0][0];
        ESP_LOGI("main", "Valor raw del ADC: %d", adc_raw[0][0]);
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
}
