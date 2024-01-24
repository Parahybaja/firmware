#include "task/fuel_em.h"

static const char *TAG = "task_fuel_em";

void task_fuel_em(void *arg){
    
    const gpio_num_t gpio_pin = (gpio_num_t)arg;

    // -----config gpio-----
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set
    io_conf.pin_bit_mask = (1ULL<<gpio_pin);
    //disable pull-down mode
    io_conf.pull_down_en = false;
    //disable pull-up mode
    io_conf.pull_up_en = false;
    //configure GPIO with the given settings
    ESP_ERROR_CHECK(gpio_config(&io_conf)); 

    // create task variables
    const int send_rate = (int)(1000.0 / (float)(TASK_FUEL_SEND_RATE_Hz));
    uint32_t timer_send;
    float sum;
    bool last_value = false;
    sensor_t fuel = {FUEL_EMERGENCY, 0.0};

    // show remaining task space
    print_task_remaining_space();

    // -----update timer-----
    timer_send = esp_log_timestamp();

    for (;;) {
        if ((esp_log_timestamp() - timer_send) >= send_rate){
            // -----add to timer-----
            timer_send += send_rate;

            // -----calculate-----
            sum = 0; // clean sum buffer
            for (int i=0; i < FUEL_AVERAGE_POINTS; i++) { 
                if (ACTIVE_LOW)
                    sum += !gpio_get_level(gpio_pin);
                else
                    sum += gpio_get_level(gpio_pin);
            }
            fuel.value = sum / (float)(FUEL_AVERAGE_POINTS);

            // ----- define high or low level -----
            // comment out to send raw data fuel average
            if (fuel.value <= THRESHOLD) // low fuel level
                fuel.value = true; // active low fuel emergency flag
            else 
                fuel.value = false; 

            // ----- send data just when is changed -----
            if (fuel.value != last_value) {
                // -----send fuel data through esp-now to receiver-----
                ESP_LOGD(TAG, "send fuel_em");
                // esp_now_send(address_ECU_Front, (uint8_t *) &fuel, sizeof(fuel));
            }

            // ----- update last value -----
            last_value = fuel.value;
        }

        vTaskDelay(pdMS_TO_TICKS(1)); // free up the processor
    }
}
