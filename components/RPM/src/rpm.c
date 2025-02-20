#include "task/rpm.h"
          
static const char *TAG = "RPM";
QueueHandle_t pulse_queue;
typedef struct {
    uint64_t last_pulse;
    uint64_t current_pulse;
} pulse_message;
static volatile uint64_t last_isr_pulse = 0;
static volatile uint64_t beforelast_isr_pulse = 0;

static bool IRAM_ATTR pcnt_isr_handler(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx) {
    uint64_t current_isr_pulse = esp_timer_get_time();
    uint64_t last_diff = last_isr_pulse - beforelast_isr_pulse;
    uint64_t diff = current_isr_pulse - last_isr_pulse;
    bool outside_inductive_period = (diff > (INDUCTIVE_PERIOD_US + 100)) || (diff < (INDUCTIVE_PERIOD_US - 100));
    bool resonating_pulse = (last_diff > diff - 110) && (last_diff < diff + 110);   

    if(outside_inductive_period || resonating_pulse) {
        pulse_message pcntisr_message = {
        .last_pulse = last_isr_pulse,
        .current_pulse = current_isr_pulse,
        };
        xQueueSendFromISR(pulse_queue, &pcntisr_message, NULL);
    }
    beforelast_isr_pulse = last_isr_pulse;
    last_isr_pulse = current_isr_pulse;
    pcnt_unit_clear_count(unit);
    return 0;
}

void task_rpm(void *arg){
    (void)arg;

    const gpio_num_t gpio_pin = (gpio_num_t)arg;
    sensor_t rpm = {
        .type = RPM, 
        .value = 0.0
    };
    pulse_queue = xQueueCreate(1, sizeof(pulse_message));
    pulse_message pulse = {
        .last_pulse = 0,
        .current_pulse = 0,
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
        uint64_t current_time = esp_timer_get_time();
        xQueueReceive(pulse_queue, &pulse, 0);

        uint64_t pulse_diff = pulse.current_pulse - pulse.last_pulse;
        uint64_t task_diff = current_time - pulse.current_pulse;

        pulse_diff += INDUCTIVE_PERIOD_US;

        //ESP_LOGW(TAG, "pulse_diff: %lld", pulse_diff);
        //ESP_LOGW(TAG, "task_diff: %lld", task_diff);
        
        if(pulse_diff > task_diff) {
            rpm.value = ((1 / ((double)(pulse_diff)/1000000)) * 60);
        } else if(task_diff > pulse_diff*2) {
            rpm.value = ((1 / ((double)(task_diff)/1000000)) * 60);
        }

        //ESP_LOGW(TAG, "%f", rpm.value);      
        printf("%f\n", rpm.value);

        system_global.rpm = rpm.value;
        esp_now_send(mac_address_ECU_front, (uint8_t *) &rpm, sizeof(rpm));

        vTaskDelay(pdMS_TO_TICKS(10)); // free up the processor
    }
}
