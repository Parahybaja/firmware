#include "task/alive.h"

static const char *TAG = "alive";

void task_alive_LED(void *arg) {
    
    const gpio_num_t gpio_pin = (gpio_num_t)arg;

    // -----config gpio-----
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set
    io_conf.pin_bit_mask = (1ULL<<gpio_pin);
    //disable pull-down mode
    io_conf.pull_down_en = false;
    //disable pull-up mode
    io_conf.pull_up_en = false;
    //configure GPIO with the given settings
    ESP_ERROR_CHECK(gpio_config(&io_conf)); 

    task_remaining_space();

    for (;;) {
        ESP_LOGI(TAG, "CPU alive");
        gpio_set_level(gpio_pin, true);

        vTaskDelay(pdMS_TO_TICKS(100));

        gpio_set_level(gpio_pin, false);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
