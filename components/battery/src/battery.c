// #include "task/battery.h"

// static const char *TAG = "task_battery";

// void task_battery(void *arg){
    
//     const battery_config_t battery_config = (battery_config_t)arg;

//     // -----config gpio-----
//     //zero-initialize the config structure.
//     gpio_config_t io_conf = {};
//     //disable interrupt
//     io_conf.intr_type = GPIO_INTR_DISABLE;
//     //set as output mode
//     io_conf.mode = GPIO_MODE_INPUT;
//     //bit mask of the pins that you want to set
//     io_conf.pin_bit_mask = (1ULL<<battery_config.gpio_pin);
//     //disable pull-down mode
//     io_conf.pull_down_en = false;
//     //disable pull-up mode
//     io_conf.pull_up_en = false;
//     //configure GPIO with the given settings
//     ESP_ERROR_CHECK(gpio_config(&io_conf)); 

//     // create task variables
//     const int send_rate_ms = (int)(1000.0 / (float)(TASK_BATTERY_RATE_Hz));
//     uint32_t timer_send_ms;
//     float sum;
//     bool last_value = false;
//     float R1 = battery_config->R1;
//     float R2 = battery_config->R2;
//     uint8_t mac[6];
//     memcpy(mac, battery_config->mac, sizeof(mac));
//     bool all_zeros = check_empty_mac(mac);
//     sensor_t bat = {
//         .type = BATTERY, 
//         .value = 0.0
//     };

//     // show remaining task space
//     print_task_remaining_space();

//     // -----update timer-----
//     timer_send_ms = esp_log_timestamp();

//     for (;;) {
//         if ((esp_log_timestamp() - timer_send_ms) >= send_rate_ms){
//             // -----add to timer-----
//             timer_send_ms += send_rate_ms;

//             // -----calculate-----
//             sum = 0; // clean sum buffer
//             for (int i=0; i < FUEL_AVERAGE_POINTS; i++) { 
//                 if (ACTIVE_LOW)
//                     sum += !gpio_get_level(battery_config.gpio_pin);
//                 else
//                     sum += gpio_get_level(battery_config.gpio_pin);
//             }
//             fuel.value = sum / (float)(FUEL_AVERAGE_POINTS);

//             // ----- define high or low level -----
//             // comment out to send raw data fuel average
//             if (fuel.value <= THRESHOLD) // low fuel level
//                 fuel.value = true; // active low fuel emergency flag
//             else 
//                 fuel.value = false; 

//             // ----- send data just when is changed -----
//             if (fuel.value != last_value) {
//                 // -----send fuel data through esp-now to receiver-----
//                 ESP_LOGD(TAG, "send fuel_em");
//                 esp_now_send(mac_address_ECU_front, (uint8_t *) &fuel, sizeof(fuel));
//             }
            
//             // ----- update last value -----
//             last_value = fuel.value;
//         }

//         vTaskDelay(pdMS_TO_TICKS(10)); // free up the processor
//     }
// }

// bool check_empty_mac(uint8_t mac[ESP_NOW_ETH_ALEN]) {
//     uint8_t count;
    
//     // Loop through the array
//     for (int i = 0; i < 6; i++) {
//         if (mac[i] == 0) {
//             count++;
//         }
//     }

//     if (count == 6) {
//         return true;
//     }
    
//     return false;
// }