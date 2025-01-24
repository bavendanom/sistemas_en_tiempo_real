

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

typedef struct {
    adc_unit_t unit;
    adc_channel_t channel;
    adc_atten_t attenuation;
    adc_oneshot_unit_handle_t handle;
    adc_cali_handle_t cali_handle;
    bool do_calibration;
} adc_config_t; 

esp_err_t init_adc(adc_config_t *adc_config);
esp_err_t read_adc_raw(adc_config_t *adc_config, int *adc_raw_value);
esp_err_t read_voltage(adc_config_t *adc_config, int adc_raw_value, int *adc_voltage);
esp_err_t deinit_adc(adc_config_t *adc_config);
bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
void example_adc_calibration_deinit(adc_cali_handle_t handle);

