#include "task/blind_spot.h"

static const char *TAG = "blind_spot";

void task_blind_spot(void *arg){
    (void)arg;
    
    const gpio_num_t gpio_pin = (gpio_num_t)arg;

    // -----config gpio-----
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL<<gpio_pin);
    io_conf.pull_down_en = false;
    io_conf.pull_up_en = false;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    const int send_rate_ms = (int)(1000.0 / (float)(TASK_BLIND_SPOT_SEND_RATE_Hz));
    uint32_t timer_send_ms;
    float sum;
    int last_value = 0;
    sensor_t blind_sr = {TILT_Z, 0.0};
    // sensor_t blind_sl = {TILT_X, 0.0};

    task_remaining_space();

    timer_send_ms = esp_log_timestamp();

    for (;;) 
    {
        if ((esp_log_timestamp() - timer_send_ms) >= send_rate_ms)
        {
            // -----add to timer-----
            timer_send_ms += send_rate_ms;

            // -----calculate-----
            sum = 0; // clean sum buffer
            for (int i=0; i < BLIND_SPOT_AVERAGE_POINTS; i++) { 
                if (ACTIVE_LOW)
                    sum += !gpio_get_level(gpio_pin);
                else
                    sum += gpio_get_level(gpio_pin);
            }
            blind_sr.value = sum / (float)(BLIND_SPOT_AVERAGE_POINTS);

            // ----- define high or low level -----
            // comment out to send raw data fuel average
            if (blind_sr.value <= THRESHOLD){ // low fuel level
                blind_sr.value = 0; // active low fuel emergency flag
            }
            else {
                blind_sr.value = 1;
            } 

            // ----- send data just when is changed -----
            if (blind_sr.value != last_value) {
                // -----send fuel data through esp-now to receiver-----
                ESP_LOGD(TAG, "Blind Spot, detected");
                printf("Pin Level: %f\n",sum);
                // esp_now_send(mac_address_ECU_rear, (uint8_t *) &blind_spot_right, sizeof(blind_spot_right));
                // esp_now_send(mac_address_ECU_rear, (uint8_t *) &blind_spot_left, sizeof(blind_spot_left));
            };

            // ----- update last value -----
            last_value = blind_sr.value;
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // free up the processor
    }
}