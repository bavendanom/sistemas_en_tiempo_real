/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/ledc.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

#include "NTC_library/include/NTC_library.h"
#include "ADC_library/include/ADC_library.h"
#include "led_RGB_LIBRARY/include/led_RGB_library.h"
#include "uart_library/include/uart_library.h"
#include "command_library/include/command_library.h"
#include "wifi_library/include/wifi_library.h"
#include "server_library/include/server_library.h"


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


#define GPIO_INPUT_IO_0     0
#define GPIO_INPUT_IO_1     5
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))

static int adc_raw[2][10];
static int voltage[2][10];

#define ESP_INTR_FLAG_DEFAULT 0
#define NTC_TASK_ 0



// Definición de colores usando enum
typedef enum {
    RED = 0,
    GREEN,
    BLUE
} color_t;

static uint8_t current_color = RED; // Color actual del LED

//MARK: Estructuras ADC
// Estructura para configurar el ADC 1 en el canal 4 
adc_config_t adc1_config_ch4 = {
    .channel = ADC_CHANNEL_4,
    .attenuation = ADC_ATTEN_DB_12,
    .cali_handle = NULL,
    .do_calibration = false
}; 

// Estructura para configurar el ADC 2 en el canal 0
adc_config_t adc2_config_ch0 = {
    .channel = ADC_CHANNEL_5,
    .attenuation = ADC_ATTEN_DB_12,
    .cali_handle = NULL,
    .do_calibration = false
};

//MARK: Estructuras LED's RGB 


// Estructura para configurar el LED RGB que funciona deacuerdo a la temperatura leida por la NTC
led_RGB rgb_ntc = {
        .led_red = {
            .gpio_num    = LEDC_OUTPUT_IO_RED_NTC ,
            .timmer      = LEDC_TIMER_0,
            .channel     = LEDC_CHANNEL_0,
            .duty        = LEDC_DUTY_INITAL,
            .flag_output_invert = 1
        },
        .led_green = {
            .gpio_num    = LEDC_OUTPUT_IO_GREEN_NTC,
            .timmer      = LEDC_TIMER_0,
            .channel     = LEDC_CHANNEL_1,
            .duty        = LEDC_DUTY_INITAL,
            .flag_output_invert = 1
        },
        .led_blue = {
            .gpio_num    = LEDC_OUTPUT_IO_BLUE_NTC,
            .timmer      = LEDC_TIMER_0,
            .channel     = LEDC_CHANNEL_2,
            .duty        = LEDC_DUTY_INITAL,
            .flag_output_invert = 1
        }
};

// Estructura para configurar el LED RGB que funciona con el potenciometro 
led_RGB rgb_pot = {
        .led_red = {
            .gpio_num    = LEDC_OUTPUT_IO_RED_POT,
            .timmer      = LEDC_TIMER_1,
            .channel     = LEDC_CHANNEL_3,
            .duty        = LEDC_DUTY_INITAL,
            .flag_output_invert = 0
        },
        .led_green = {
            .gpio_num    = LEDC_OUTPUT_IO_GREEN_POT,
            .timmer      = LEDC_TIMER_1,
            .channel     = LEDC_CHANNEL_4,
            .duty        = LEDC_DUTY_INITAL,
            .flag_output_invert = 0
        },
        .led_blue = {
            .gpio_num    = LEDC_OUTPUT_IO_BLUE_POT,
            .timmer      = LEDC_TIMER_1,
            .channel     = LEDC_CHANNEL_5,
            .duty        = LEDC_DUTY_INITAL,
            .flag_output_invert = 0
        }
};


//MARK: rgb_init
// Función para inicializar un LED RGB
static void rgb_init(led_RGB rgb) {
    // Inicializar timers de cada LED
    // Inicializa el temporizador usando solo uno de los LEDs (se asume que los tres usan el mismo timer)
    ledc_init_timer(rgb.led_red);
    ledc_initialize_rgb(rgb);
}

//MARK: config_adc_unit
static void config_adc_unit(adc_config_t *acd_ch, adc_unit_t adc_unit ){
    config_unit init_adc_unit = init_adc(adc_unit);
    init_adc_ch(acd_ch, init_adc_unit);
}

//MARK: task_process_uart
void task_process_uart(void *arg) {
    char received_cmd[RX_BUF_SIZE];
    while (1) {
        if (xQueueReceive(uart_rx_queue, received_cmd, portMAX_DELAY) == pdTRUE) {
            process_command(received_cmd);
        }
    }
}


#if NTC_TASK_
//MARK: NTC_task
void NTC_task(void *pvParameter){
    float fixed_resistor = 100.0; // Resistencia fija en ohmios 
    float reference_voltage = 3.3; // Voltaje de referencia 
    
    config_adc_unit(&adc2_config_ch0, ADC_UNIT_1);
    rgb_init(rgb_ntc);


    float MIN_RED = 30, MAX_RED = 50;
    float MIN_GREEN = 20, MAX_GREEN = 40;
    float MIN_BLUE = 10, MAX_BLUE = 30;
    
    while(1) {


        //read_adc_raw(&adc1_config_ch4, &adc_raw[0][2]);
        // Leer ADC
        read_adc_raw(&adc2_config_ch0, &adc_raw[0][2]);
        read_voltage(&adc2_config_ch0, adc_raw[0][2], &voltage[0][2]);

        /* // Log para verificar el valor del ADC
        ESP_LOGI("NTC_task", "ADC Raw Value: %d", adc_raw[0][2]);
        ESP_LOGI("NTC_task", "Voltage: %d mV", voltage[0][2]); */

        // Convertir el voltaje medido de milivoltios a voltios
        float voltage_out = voltage[0][2] / 1000.0; // Convertir de mV a V
        
        // Calcular resistencia NTC y temperatura
        float R_NTC = res_ntc(voltage_out, fixed_resistor, reference_voltage); 
        float temp_Celsius = tem_cel(R_NTC);

        
        //xQueueReset(temp_queue);  
        xQueueSend(temp_queue, &temp_Celsius , portMAX_DELAY);

        if (xQueueReceive(min_red_queue , &MIN_RED, pdMS_TO_TICKS(100)) == pdTRUE) {
            char mensaje[50];
            snprintf(mensaje, sizeof(mensaje), "MIN_RED RECEIVED: %.2f°C\n", MIN_RED);
            sendData("NTC TASK", mensaje);
        }

        if (xQueueReceive(max_red_queue , &MAX_RED, pdMS_TO_TICKS(100)) == pdTRUE) {
            char mensaje[50];
            snprintf(mensaje, sizeof(mensaje), "MAX_RED RECEIVED: %.2f°C\n", MAX_RED);
            sendData("NTC TASK", mensaje);
        }

        else if (xQueueReceive(min_green_queue , &MIN_GREEN, pdMS_TO_TICKS(100)) == pdTRUE) {
            char mensaje[50];
            snprintf(mensaje, sizeof(mensaje), "MIN_GREEN RECEIVED: %.2f°C\n", MIN_RED);
            sendData("NTC TASK", mensaje);
        }

        else if (xQueueReceive(max_green_queue , &MAX_GREEN, pdMS_TO_TICKS(100)== pdTRUE)) {
            char mensaje[50];
            snprintf(mensaje, sizeof(mensaje), "MIN_BLUE RECEIVED: %.2f°C\n", MAX_RED);
            sendData("NTC TASK", mensaje);
        }

        else if (xQueueReceive(min_blue_queue , &MIN_BLUE, pdMS_TO_TICKS(100)) == pdTRUE) {
            char mensaje[50];
            snprintf(mensaje, sizeof(mensaje), "MIN_BLUE RECEIVED: %.2f°C\n", MIN_RED);
            sendData("NTC TASK", mensaje);
        }

        else if (xQueueReceive(max_blue_queue , &MAX_BLUE, pdMS_TO_TICKS(100)) == pdTRUE) {
            char mensaje[50];
            snprintf(mensaje, sizeof(mensaje), "MIN_GREEN RECEIVED: %.2f°C\n", MAX_RED);
            sendData("NTC TASK", mensaje);
        }
        

    // **Acumular los colores antes de actualizar el LED**
        int red_value = 0, green_value = 0, blue_value = 0;

        if (temp_Celsius >= MIN_RED && temp_Celsius <= MAX_RED) {
            red_value = temp_Celsius;  // Asigna intensidad proporcional a la temperatura
        }
        if (temp_Celsius >= MIN_GREEN && temp_Celsius <= MAX_GREEN) {
            green_value = temp_Celsius;
        }
        if (temp_Celsius >= MIN_BLUE && temp_Celsius <= MAX_BLUE) {
            blue_value = temp_Celsius;
        }

        // **Aplicar todos los colores al mismo tiempo**
        rgb_set_color(rgb_ntc, red_value, green_value, blue_value);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    deinit_adc(&adc2_config_ch0);
    vTaskDelete(NULL);
}

#else 
//MARK: RGB CRHOMATIC CIRCLE 
void rgb_crhomatic_circle_task(void *pvParameter){
    rgb_init(rgb_ntc);
    int RED_CC=0, GREEN_CC=0, BLUE_CC=0, brightness=100;
    
    while (1) {
        if (xQueueReceive(slider_crhomatic_circle_queue, &brightness, pdMS_TO_TICKS(100)) == pdTRUE) {
            char mensaje[50];
            snprintf(mensaje, sizeof(mensaje), "brightness RECEIVED: %d\n", brightness);
            sendData("CRHOMATIC CIRCLE TASK", mensaje);
        }


        if (xQueueReceive(rgb_crhomatic_circle_red_queue , &RED_CC, pdMS_TO_TICKS(100)) == pdTRUE) {
            char mensaje[50];
            snprintf(mensaje, sizeof(mensaje), "RED_CC RECEIVED: %d\n", RED_CC);
            sendData("CRHOMATIC CIRCLE TASK", mensaje);
        }

        if (xQueueReceive(rgb_crhomatic_circle_green_queue , &GREEN_CC, pdMS_TO_TICKS(100)) == pdTRUE) {
            char mensaje[50];
            snprintf(mensaje, sizeof(mensaje), "GREEN_CC RECEIVED: %d\n", GREEN_CC);
            sendData("CRHOMATIC CIRCLE TASK", mensaje);
        }

        if (xQueueReceive(rgb_crhomatic_circle_blue_queue , &BLUE_CC, pdMS_TO_TICKS(100)) == pdTRUE) {
            char mensaje[50];
            snprintf(mensaje, sizeof(mensaje), "BLUE_CC RECEIVED: %d\n", BLUE_CC);
            sendData("CRHOMATIC CIRCLE TASK", mensaje);
        }

        int red_cc = (100*RED_CC)/255;
        int green_cc = (100*GREEN_CC)/255;
        int blue_cc = (100*BLUE_CC)/255;

        // Evitar división por cero
        if (red_cc == 0) red_cc = 1; // Asignar un valor mínimo para evitar división por cero
        if (green_cc == 0) green_cc = 1;
        if (blue_cc == 0) blue_cc = 1;

        int red_brg = (red_cc*brightness)/100;
        int green_brg = (green_cc*brightness)/100;
        int blue_brg = (blue_cc*brightness)/100;

        rgb_set_color(rgb_ntc, red_brg, green_brg, blue_brg);
        vTaskDelay(pdMS_TO_TICKS(100));
    }  
}
#endif



// Manejador de interrupciones para GPIO
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    // Cambiar el color actual (ciclo entre 0, 1 y 2)
    current_color = (current_color + 1) % 3;
}

//MARK: config_button
static void config_button(){
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_down_en = 1;
    gpio_config(&io_conf);

    //change gpio interrupt type for one pin
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_POSEDGE);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);

    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);

    printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());
}


//MARK: button_task
static void button_task(void *pvParameter){
    rgb_init(rgb_pot);
    config_adc_unit(&adc1_config_ch4, ADC_UNIT_1);

    
    while (1)
    {
        read_adc_raw(&adc1_config_ch4, &adc_raw[0][1]);
        int32_t duty = (100 * adc_raw[0][1])/4095; 
        int32_t adc_value = adc_raw[0][1];
        xQueueReset(read_pot);  
        xQueueSend(read_pot, &duty, portMAX_DELAY);
        // Cambiar el color del LED basado en el valor actual
        int color_current = 1;

        if (xQueueReceive(change_current_color , &color_current, pdMS_TO_TICKS(100)) == pdTRUE) {

            current_color = (current_color + 1) % 3;

            char mensaje[50];
            snprintf(mensaje, sizeof(mensaje), "COLOR CHANGED: %i\n", color_current);
            sendData("TASK BUTTON: WEB SERVER:", mensaje);
        }

        switch (current_color){
            case RED:
                rgb_set_color(rgb_pot, duty, 0, 0); // Rojo
                break;
            case GREEN:
                rgb_set_color(rgb_pot, 0, duty, 0); // Verde
                break;
            case BLUE:
                rgb_set_color(rgb_pot, 0, 0, duty); // Azul
                break;
            default:
                break;
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
    }      
}


static const char *TAG = "wifi station";
static void init_server(void){
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Configurar el pin GPIO para el LED
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_app_start();

    // Iniciar el servidor web
    start_webserver();
}
//MARK: app_main
void app_main(void)
{
    config_button();
    
    uart_init();
    comandos_init();
    comandos_init_server();

    init_server();

    #if NTC_TASK_
    xTaskCreate(NTC_task, "NTC_task", 4096*2, NULL, 4, NULL); // Crear la tarea para leer ADC y calculos para la NTC
    #else
    xTaskCreate(rgb_crhomatic_circle_task, "rgb_crhomatic_circle_task", 4096*2, NULL, 6, NULL);
    #endif

    xTaskCreate(button_task, "button_task", 4096, NULL, configMAX_PRIORITIES -  3, NULL);
    xTaskCreate(rx_task, "rx_task", 4096, NULL, 5, NULL);
    xTaskCreate(task_process_uart, "task_process_uart", 4096, NULL, 5, NULL);
    
    
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
} 