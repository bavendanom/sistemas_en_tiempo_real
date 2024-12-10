#include "esp_err.h"
#include "driver/ledc.h"

// Definiciones globales para PWM
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Resolución de duty de 13 bits
#define LEDC_FREQUENCY          (4000)           // Frecuencia PWM de 4 kHz

// Estructura para representar un LED
typedef struct {
    int gpio_num;                   // Número de pin GPIO conectado al LED
    ledc_mode_t speed_mode;         // Modo de velocidad (LEDC_HIGH_SPEED_MODE o LEDC_LOW_SPEED_MODE)
    ledc_channel_t channel;         // Canal del controlador LEDC
    ledc_intr_type_t intr_type;     // Tipo de interrupción
    ledc_timer_t timer_sel;         // Selección del temporizador
    uint32_t duty;                  // Ciclo de trabajo inicial (duty)
} led;

// Estructura para representar un LED RGB
typedef struct {
    led led_red;                    // LED rojo
    led led_green;                  // LED verde
    led led_blue;                   // LED azul
} led_RGB;

// Declaraciones de funciones públicas

/**
 * @brief Inicializa el temporizador para un LED.
 * 
 * @param l Estructura `led` que contiene la configuración del LED.
 * @return esp_err_t ESP_OK si se inicializó correctamente, otro valor en caso de error.
 */
esp_err_t ledc_init_timer(led l);

/**
 * @brief Configura el canal del controlador LEDC para un LED.
 * 
 * @param l Estructura 'led' que contiene la configuración del LED.
 * @return esp_err_t ESP_OK si se configuró correctamente, otro valor en caso de error.
 */
esp_err_t ledc_init_channel(led l);

/**
 * @brief Establece el color del LED RGB mediante los ciclos de trabajo (duty).
 * 
 * @param rgb Estructura `led_RGB` que representa los LEDs RGB.
 * @param duty_red Ciclo de trabajo para el LED rojo (0-100%).
 * @param duty_green Ciclo de trabajo para el LED verde (0-100%).
 * @param duty_blue Ciclo de trabajo para el LED azul (0-100%).
 * @return esp_err_t ESP_OK si los valores se configuraron correctamente, otro valor en caso de error.
 */
esp_err_t rgb_set_color(led_RGB rgb, uint32_t duty_red, uint32_t duty_green, uint32_t duty_blue);


