#include "task/fuel_level.h"

static const char *TAG = "fuel_level";
QueueHandle_t fuel_queue;
typedef struct {
    uint64_t diff;
} pulse_message;
static volatile uint64_t last_isr_pulse = 0;

static bool IRAM_ATTR pcnt_isr_handler(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx) {
    uint64_t current_isr_pulse = esp_timer_get_time();
    uint64_t diff = current_isr_pulse - last_isr_pulse;

    pulse_message pcntisr_message = {
        .diff = diff,
    };
    xQueueSendFromISR(fuel_queue, &pcntisr_message, NULL);

    last_isr_pulse = current_isr_pulse;
    pcnt_unit_clear_count(unit);
    return 0;
}

void task_fuel_level(void *arg){
    (void)arg;

    const gpio_num_t gpio_pin = (gpio_num_t)arg;
    sensor_t fuel_level = {
        .type = FUEL_LEVEL, 
        .value = 0.0
    };
    fuel_queue = xQueueCreate(1, sizeof(pulse_message));
    pulse_message pulse = {
        .diff = 0,
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

    uint64_t last_pulse = 0;
    uint64_t current_pulse = 0;
    for (;;) {
        xQueueReceive(fuel_queue, &pulse, 0);
        fuel_level.value = pulse.diff;
        


        ESP_LOGW(TAG, "%f", fuel_level.value);      
        system_global.fuel_level = fuel_level.value;
        esp_now_send(mac_address_ECU_front, (uint8_t *) &fuel_level, sizeof(fuel_level));

        vTaskDelay(pdMS_TO_TICKS(10)); // free up the processor
    }
}
