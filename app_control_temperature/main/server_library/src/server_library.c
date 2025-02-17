#include "server_library.h"
#include "command_library.h"

static const char *TAG = "wifi station";


QueueHandle_t read_pot;

// Función para manejar la solicitud de encender el LED
esp_err_t led_on_handler(httpd_req_t *req) {
    gpio_set_level(LED_GPIO, 1);  // Enciende el LED
    httpd_resp_send(req, "LED On", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Función para manejar la solicitud de apagar el LED
esp_err_t led_off_handler(httpd_req_t *req) {
    gpio_set_level(LED_GPIO, 0);  // Apaga el LED
    httpd_resp_send(req, "LED OFF", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


// Función para manejar la solicitud de obtener el valor de ADC
esp_err_t adc_handler(httpd_req_t *req) {
    int32_t adc_value;
    if (xQueueReceive(read_pot, &adc_value, pdMS_TO_TICKS(1000)) == pdPASS) {
        char resp_str[64];
        snprintf(resp_str, sizeof(resp_str), "{\"adc_value\": %li}", adc_value);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    } else {
        httpd_resp_send_500(req);
    }
    return ESP_OK;
}


// Añadir estos manejadores para servir los archivos
esp_err_t index_handler(httpd_req_t *req) {
    extern const unsigned char index_html_start[] asm("_binary_index_html_start");
    extern const unsigned char index_html_end[] asm("_binary_index_html_end");
    const size_t index_html_size = (index_html_end - index_html_start);
    httpd_resp_send(req, (const char *)index_html_start, index_html_size);
    return ESP_OK;
}

esp_err_t styles_handler(httpd_req_t *req) {
    extern const unsigned char styles_css_start[] asm("_binary_styles_css_start");
    extern const unsigned char styles_css_end[] asm("_binary_styles_css_end");
    const size_t styles_css_size = (styles_css_end - styles_css_start);
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)styles_css_start, styles_css_size);
    return ESP_OK;
}

esp_err_t script_handler(httpd_req_t *req) {
    extern const unsigned char script_js_start[] asm("_binary_script_js_start");
    extern const unsigned char script_js_end[] asm("_binary_script_js_end");
    const size_t script_js_size = (script_js_end - script_js_start);
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)script_js_start, script_js_size);
    return ESP_OK;
}



// Definir las URIs para las solicitudes de encender y apagar el LED
httpd_uri_t uri_led_on = {
    .uri = "/led_on",
    .method = HTTP_GET,
    .handler = led_on_handler,
    .user_ctx = NULL
};



httpd_uri_t uri_led_off = {
    .uri = "/led_off",
    .method = HTTP_GET,
    .handler = led_off_handler,
    .user_ctx = NULL
};

// Definir la URI para la solicitud de obtener el valor de ADC
httpd_uri_t uri_get_adc = {
    .uri = "/get_adc",
    .method = HTTP_GET,
    .handler = adc_handler,
    .user_ctx = NULL
};

// Actualizar la función de inicio del servidor web para incluir estos manejadores
void start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_led_on);
        httpd_register_uri_handler(server, &uri_led_off);
        httpd_register_uri_handler(server, &uri_get_adc);

        // Registrar los nuevos manejadores
        httpd_uri_t uri_index = {
            .uri      = "/",
            .method   = HTTP_GET,
            .handler  = index_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_index);

        httpd_uri_t uri_styles = {
            .uri      = "/styles.css",
            .method   = HTTP_GET,
            .handler  = styles_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_styles);

        httpd_uri_t uri_script = {
            .uri      = "/script.js",
            .method   = HTTP_GET,
            .handler  = script_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_script);

        ESP_LOGI(TAG, "Web server started");
    } else {
        ESP_LOGE(TAG, "Failed to start web server");
    }
}


void comandos_init_server(void) {
    read_pot  =  xQueueCreate(5, sizeof(float));
}
