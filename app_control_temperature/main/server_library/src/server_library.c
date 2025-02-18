#include "server_library.h"
#include "command_library.h"
#include "uart_library.h"

static const char *TAG = "wifi station";

static uint8_t s_led_state = 0;

QueueHandle_t read_pot;

/* // Función para manejar la solicitud de encender el LED
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
} */

static esp_err_t http_server_toogle_led_handler(httpd_req_t *req)
{
	//ESP_LOGI(TAG, "/toogle_led.json requested");

	s_led_state = !s_led_state;
	gpio_set_level(LED_GPIO, s_led_state);

	// Cerrar la conexion
    httpd_resp_set_hdr(req, "Connection", "close");
    httpd_resp_send(req, NULL, 0);
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

// Función para manejar la solicitud de obtener el valor de ADC temperatura
esp_err_t adc_tem_handler(httpd_req_t *req) {
    float temp_Celsius;
    if (xQueueReceive(temp_queue, &temp_Celsius, pdMS_TO_TICKS(1000)) == pdPASS) {
        char resp_str[64];
        snprintf(resp_str, sizeof(resp_str), "{\"Temperature\": %.2f}", temp_Celsius);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    } else {
        httpd_resp_send_500(req);
    }
    return ESP_OK;
}

// Función para manejar la solicitud de obtener el valor de ADC temperatura de la ultima lectura
esp_err_t adc_tem_one_read_handler(httpd_req_t *req) {
    float temp_Celsius;
    if (xQueueReceive(temp_queue, &temp_Celsius, pdMS_TO_TICKS(1000)) == pdPASS) {
        char resp_str[64];
        snprintf(resp_str, sizeof(resp_str), "{\"Temperature\": %.2f}", temp_Celsius);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    } else {
        httpd_resp_send_500(req);
    }
    return ESP_OK;
}


esp_err_t set_rgb_handler(httpd_req_t *req) {
    char buf[100];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= ret;
    }

    buf[req->content_len] = '\0';  // Asegurarse de que la cadena esté terminada en nulo

    cJSON *json = cJSON_Parse(buf);
    if (json == NULL) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    const cJSON *color = cJSON_GetObjectItem(json, "color");
    const cJSON *min = cJSON_GetObjectItem(json, "min");
    const cJSON *max = cJSON_GetObjectItem(json, "max");

    if (cJSON_IsString(color) && cJSON_IsNumber(min) && cJSON_IsNumber(max)) {
        ESP_LOGI(TAG, "Received RGB settings - Color: %s, Min: %.2f, Max: %.2f", color->valuestring, min->valuedouble, max->valuedouble);
        
        if (strcmp(color->valuestring, "red") == 0) {
            float min_red = min->valuedouble;
            float max_red = max->valuedouble;
            ESP_LOGI(TAG,"se agrego a la cola");
            char mensaje[50];

        // Enviar el nuevo valor a la cola
        if (xQueueSend(min_red_queue, &min_red, portMAX_DELAY) == pdTRUE) {
            snprintf(mensaje, sizeof(mensaje), "MIN_red  configurado en %f°C\n", min_red);
            sendData("CMD_HANDLER", mensaje);
        } else {
            sendData("CMD_HANDLER", "Error al enviar MIN_GREEN a la cola");
        }
            xQueueSend(max_red_queue, &max_red, portMAX_DELAY);
        } else if (strcmp(color->valuestring, "green") == 0) {
            float min_green = min->valuedouble;
            float max_green = max->valuedouble;
            xQueueSend(min_green_queue, &min_green, portMAX_DELAY);
            xQueueSend(max_green_queue, &max_green, portMAX_DELAY);
        } else if (strcmp(color->valuestring, "blue") == 0) {
            float min_blue = min->valuedouble;
            float max_blue = max->valuedouble;
            xQueueSend(min_blue_queue, &min_blue, portMAX_DELAY);
            xQueueSend(max_blue_queue, &max_blue, portMAX_DELAY);
        } else {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
    } else {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    cJSON_Delete(json);
    httpd_resp_send(req, "RGB settings updated", HTTPD_RESP_USE_STRLEN);
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



// Definir la URI para la solicitud de obtener el valor de ADC
httpd_uri_t uri_get_adc = {
    .uri = "/get_adc",
    .method = HTTP_GET,
    .handler = adc_handler,
    .user_ctx = NULL
};

// Definir la URI para la solicitud de obtener el valor de Temperature
httpd_uri_t uri_get_tem_adc = {
    .uri = "/turn_on_temp",
    .method = HTTP_GET,
    .handler = adc_tem_handler,
    .user_ctx = NULL
};

// Definir la URI para la solicitud de obtener el valor de Temperature de la ultima lectura
httpd_uri_t uri_get_tem_one_read = {
    .uri = "/get_temperature",
    .method = HTTP_GET,
    .handler = adc_tem_one_read_handler,
    .user_ctx = NULL
};

// register toogle_led handler
httpd_uri_t uri_toogle_led = {
    .uri = "/toogle_led",
    .method = HTTP_POST,
    .handler = http_server_toogle_led_handler,
    .user_ctx = NULL
};


httpd_uri_t uri_set_rgb = {
    .uri = "/set_rgb",
    .method = HTTP_POST,
    .handler = set_rgb_handler,
    .user_ctx = NULL
};


// Actualizar la función de inicio del servidor web para incluir estos manejadores
void start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        //httpd_register_uri_handler(server, &uri_led_on);
        //httpd_register_uri_handler(server, &uri_led_off);
        httpd_register_uri_handler(server, &uri_get_adc);
        httpd_register_uri_handler(server, &uri_get_tem_adc);
        httpd_register_uri_handler(server, &uri_get_tem_one_read);
        httpd_register_uri_handler(server, &uri_toogle_led);
        httpd_register_uri_handler(server, &uri_set_rgb);

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
