#include "task/battery.h"
#include "system.h" // Necessário para acessar system_global e sh_global_vars
#include "esp_log.h"

static const char *TAG = "task_battery";

void task_battery(void *arg){
    
    const battery_config_t *battery_config = (const battery_config_t*)arg;

    // create task variables
    const int send_rate_ms = (int)(1000.0 / (float)(TASK_BATTERY_RATE_Hz));
    uint32_t timer_send_ms;
    float voltage_read;
    float voltage_bat;
    int adc_raw;

    const float R1 = battery_config->R1;
    const float R2 = battery_config->R2;

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12, // <-- CORRIGIDO AQUI (era DB_11)
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, battery_config->adc_channel, &config));

    // show remaining task space
    print_task_remaining_space();

    // -----update timer-----
    timer_send_ms = esp_log_timestamp();

    for (;;) {
        if ((esp_log_timestamp() - timer_send_ms) >= send_rate_ms){
            // -----add to timer-----
            timer_send_ms += send_rate_ms;

            adc_oneshot_read(adc1_handle, battery_config->adc_channel, &adc_raw);

            // -----calculate voltage-----
            voltage_read = (ADC_VOLTAGE * adc_raw) / ADC_RESOLUTION;
            voltage_bat = voltage_read / (R2 / (R1 + R2));

            ESP_LOGI(TAG, "ADC: Raw=%d, voltage_read=%f, voltage_bat=%f", adc_raw, voltage_read, voltage_bat);

            // ==========================================
            // ATUALIZA O QUADRO DE AVISOS GLOBAL
            // ==========================================
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
            system_global.battery = voltage_bat;
            xSemaphoreGive(sh_global_vars);
            // ==========================================
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // free up the processor
    }
}