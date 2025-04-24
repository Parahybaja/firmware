#include "task/speedometer.h"

static const char *TAG = "speedometer";

void task_speed(void *arg){
    (void)arg;

    const gpio_num_t gpio_pin = (gpio_num_t)arg;

    // create task variables
    const int send_rate_ms = (int)(1000.0 / (float)(TASK_SPEED_SEND_RATE_Hz));
    const float send_rate_s = 1.0f / (float)TASK_SPEED_SEND_RATE_Hz;
    uint32_t timer_send_ms;
    int pulse_count = 0;
    sensor_t spdmt = {
        .type = SPEEDOMETER, 
        .value = 0.0
    };

    /*-----config pulse counter-----*/
    ESP_LOGI(TAG, "install pcnt unit");
    pcnt_unit_config_t unit_config = {
        .high_limit = PCNT_HIGH_LIMIT,
        .low_limit = PCNT_LOW_LIMIT,
    };

    pcnt_unit_handle_t pcnt_unit = NULL;
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    ESP_LOGI(TAG, "set glitch filter");
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 100,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = gpio_pin,
        .level_gpio_num = -1,
        .flags.virt_level_io_level = false,
    };
    pcnt_channel_handle_t pcnt_chan = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));

    ESP_LOGI(TAG, "enable pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_LOGI(TAG, "clear pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_LOGI(TAG, "start pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    // show remaining task space
    print_task_remaining_space();

    // -----update timer-----
    timer_send_ms = esp_log_timestamp();

    for (;;) {
        if ((esp_log_timestamp() - timer_send_ms) >= send_rate_ms){
            // -----add to timer-----
            timer_send_ms += send_rate_ms;

            // -----get counts-----
            esp_err_t err = pcnt_unit_get_count(pcnt_unit, &pulse_count);
            pcnt_unit_clear_count(pcnt_unit);

            if (err == ESP_OK) {
                // -----calculate-----
                float meters = (pulse_count / WHEEL_EDGES) * WHEEL_CIRC;
                spdmt.value = meters / send_rate_s; // in meter/second
                spdmt.value *= ms2kmh;

                ESP_LOGW(TAG, "speed: %f, counts: %i", spdmt.value, pulse_count);

                // -----send spped data through esp-now to receiver-----
                esp_now_send(mac_address_ECU_front, (uint8_t *) &spdmt, sizeof(spdmt));
            }
            else {
                ESP_LOGE(TAG, "error getting PCNT value");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // free up the processor
    }
}
