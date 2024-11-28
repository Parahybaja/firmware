#include "task/rpm.h"
          
static const char *TAG = "RPM";
QueueHandle_t pulse_queue;
typedef struct {
    uint32_t time;
} pulse_message;

static bool IRAM_ATTR pcnt_isr_handler(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx) {
    pulse_message pcntisr_message = {
        .time = esp_log_timestamp(),
    }; 

    ESP_DRAM_LOGW(TAG, "Interrupt called");
    xQueueSendFromISR(pulse_queue, &pcntisr_message, NULL);
    pcnt_unit_clear_count(unit);
    return 0;
}

void task_rpm(void *arg){
    (void)arg;

    const gpio_num_t gpio_pin = (gpio_num_t)arg;

    // create task variables
    const int send_rate_ms = (int)(1000.0 / (float)(TASK_RPM_SEND_RATE_Hz));
    const double send_rate_min = 1 / ((float)TASK_RPM_SEND_RATE_Hz * 60.0f);
    uint32_t timer_send_ms;
    int pulse_count = 0;
    sensor_t rpm = {
        .type = RPM, 
        .value = 0.0
    };
    pulse_queue = xQueueCreate(1, sizeof(pulse_message));
    pulse_message pulse = {
        .time = 0
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
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = gpio_pin,
        .level_gpio_num = -1,
        .flags.virt_level_io_level = false,
    };
    pcnt_channel_handle_t pcnt_chan = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, 1));

    pcnt_event_callbacks_t cbs = {
        .on_reach = pcnt_isr_handler
    };

    ESP_ERROR_CHECK(pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, NULL));

    ESP_LOGI(TAG, "enable pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_LOGI(TAG, "clear pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_LOGI(TAG, "start pcnt unit");
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    // show remaining task space
    print_task_remaining_space();

    uint32_t last_pulse = 0;
    uint32_t current_pulse = 0;
    for (;;) {
        if(uxQueueMessagesWaiting(pulse_queue) > 0) {
            last_pulse = current_pulse;
            xQueueReceive(pulse_queue, &pulse, 0);
            current_pulse = pulse.time;
        } 

        uint32_t current_time = esp_log_timestamp();
        uint32_t pulse_diff = current_pulse - last_pulse;
        uint32_t task_diff = current_time - current_pulse;
        
        if(pulse_diff > task_diff) {
            rpm.value = (int)((1 / ((float)(pulse_diff)/1000)) * 60);
        } else {
            rpm.value = (int)(1 / (float)(task_diff)/1000) * 60;
        }
        //ESP_LOGW(TAG, "%f", rpm.value);      
        
        system_global.rpm = rpm.value;
        esp_now_send(mac_address_ECU_front, (uint8_t *) &rpm, sizeof(rpm));

        vTaskDelay(pdMS_TO_TICKS(10)); // free up the processor
    }
}
