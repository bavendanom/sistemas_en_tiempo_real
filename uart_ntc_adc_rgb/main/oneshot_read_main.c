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

#define TXD_PIN (GPIO_NUM_1)
#define RXD_PIN (GPIO_NUM_3)


#define GPIO_INPUT_IO_0     0
#define GPIO_INPUT_IO_1     5
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))

static const int RX_BUF_SIZE = 1024;

static QueueHandle_t gpio_evt_queue = NULL;

static int adc_raw[2][10];
static int voltage[2][10];

#define ESP_INTR_FLAG_DEFAULT 0

// Definición de colores usando enum
typedef enum {
    RED = 0,
    GREEN,
    BLUE
} color_t;

static uint8_t current_color = RED; // Color actual del LED


// Estructura para configurar el ADC 1 en el canal 4 
adc_config_t adc1_config_ch4 = {
    .channel = ADC_CHANNEL_4,
    .attenuation = ADC_ATTEN_DB_12,
    .cali_handle = NULL,
    .do_calibration = false
}; 

// Estructura para configurar el ADC 2 en el canal 0
adc_config_t adc2_config_ch0 = {
    .channel = ADC_CHANNEL_0,
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

static void config_adc_unit(adc_config_t *acd_ch, adc_unit_t adc_unit ){
    config_unit init_adc_unit = init_adc(adc_unit);
    init_adc_ch(acd_ch, init_adc_unit);
}

void NTC_task(void *pvParameter){
    float fixed_resistor = 100.0; // Resistencia fija en ohmios 
    float reference_voltage = 3.3; // Voltaje de referencia 
    
    config_adc_unit(&adc2_config_ch0, ADC_UNIT_2);

    rgb_init(rgb_ntc);
    
    while(1) {
        // Leer ADC
        read_adc_raw(&adc2_config_ch0, &adc_raw[1][0]);
        read_voltage(&adc2_config_ch0, adc_raw[1][0], &voltage[1][0]);

        ESP_LOGI("adc_task", "Valor voltage del ADC: %d", voltage[1][0]);

        // Convertir el voltaje medido de milivoltios a voltios
        float voltage_out = voltage[1][0] / 1000.0; // Convertir de mV a V
        
        // Calcular resistencia NTC y temperatura
        float R_NTC = res_ntc(voltage_out, fixed_resistor, reference_voltage); 
        float temp_Celsius = tem_cel(R_NTC);  

        ESP_LOGI("adc_task", "Resistencia NTC: %.2f ohmios, Temperatura: %.2f °C", R_NTC, temp_Celsius);
        // Controlar el LED según la temperatura
        if (temp_Celsius >= 10 && temp_Celsius <= 30) {
            rgb_set_color(rgb_ntc, 0, 0, temp_Celsius); // LED azul
        } else if (temp_Celsius >= 30 && temp_Celsius <= 40) {
            rgb_set_color(rgb_ntc, 0, temp_Celsius,0); // LED verde
        } else if (temp_Celsius >= 40 && temp_Celsius <= 50){
            rgb_set_color(rgb_ntc, temp_Celsius, 0 , 0); // LED rojo
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    deinit_adc(&adc2_config_ch0);
    vTaskDelete(NULL);
}
void init_uart(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

int sendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_0, data, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}

static void tx_task(void *arg)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    while (1) {
        sendData(TX_TASK_TAG, "Hello world");
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}

static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE + 1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_0, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
        }
    }
    free(data);
}
// Manejador de interrupciones para GPIO
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    // Cambiar el color actual (ciclo entre 0, 1 y 2)
    current_color = (current_color + 1) % 3;
}

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

static void button_task(){
    rgb_init(rgb_pot);
    config_adc_unit(&adc1_config_ch4, ADC_UNIT_1);
    // Leer ADC
    
    while (1)
    {
        read_adc_raw(&adc1_config_ch4, &adc_raw[0][1]);
        int32_t duty = (100 * adc_raw[0][1])/4095; 
        // Cambiar el color del LED basado en el valor actual
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

void app_main(void)
{
    init_uart();
    config_button();
    
    // Crear la tarea para leer ADC y calculos para la NTC
    xTaskCreate(NTC_task, "NTC_task", 4096, NULL, 5, NULL);
    xTaskCreate(button_task, "uart_tx_task", 1024 * 2, NULL, configMAX_PRIORITIES -  3, NULL);
    xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(tx_task, "uart_tx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 2, NULL);
    // Otras inicializaciones o tareas pueden ir aquí
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}