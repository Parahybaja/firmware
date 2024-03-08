/**
 * @file main.h
 * @authors 
 *      Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief Parahybaja's embedded system: ECU Rear
 * @version 5.0
 * @date 2024-01-24
 *
 * @copyright Copyright (c) 2024
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  5.0.0    Jefferson L.  15/01/2024  convertion to esp-idf
 * 
 */

#include <stdio.h>
#include "system.h"
#include "task/alive.h"
#include "task/battery.h"
// #include "task/fuel_em.h"

#include "espnow_callback.h"

#include "driver/pulse_cnt.h"
#include "driver/gpio.h"

#define TASK_SPEED_SEND_RATE_Hz  2 // speedometer task send rate in hertz

#define PCNT_HIGH_LIMIT 100
#define PCNT_LOW_LIMIT  -100
#define PCNT_PIN 7

// ----- constants -----
#define PI          3.1415f
#define ms2kmh      3.6f
#define WHEEL_DIA   0.45f            // diameter in meters
#define WHEEL_CIRC  (PI * WHEEL_DIA) // wheel circumference
#define WHEEL_EDGES 6.0f             // 6 edges per revolution

static const char *TAG = "ECU_rear";

static const gpio_num_t speed_pin = GPIO_NUM_39;
static const gpio_num_t alive_pin = GPIO_NUM_2;
// static const gpio_num_t fuel_em_pin = GPIO_NUM_34;
static const battery_config_t battery_config = {
    .adc_channel = ADC_CHANNEL_7, // GPIO 35
    .R1 = 10e3,
    .R2 = 2.1e3 // calibrated
};

void task_speed(void *arg);

void app_main(void) {
    
    ESP_LOGD(TAG, "ECU rear v5");

    system_espnow_init();
    register_callbacks();

    print_mac_address();

    // -----fire up tasks-----
    xTaskCreatePinnedToCore(
        task_alive_LED,   // task function
        "alive LED",      // task name
        2048,             // stack size
        (void*)alive_pin, // parameters
        8,                // priority
        &th_alive,        // handler 
        APP_CPU_NUM       // core number
    );

    xTaskCreatePinnedToCore(
        task_battery,           // task function
        "battery",              // task name
        2048,                   // stack size
        (void*)&battery_config, // parameters
        8,                      // priority
        &th_battery,            // handler 
        APP_CPU_NUM             // core number
    );

    // xTaskCreatePinnedToCore(
    //     task_fuel_em,       // task function
    //     "fuel emergency",   // task name
    //     4096,               // stack size
    //     (void*)fuel_em_pin, // parameters
    //     8,                  // priority
    //     &th_fuel_em,        // handler 
    //     APP_CPU_NUM         // core number
    // );

    xTaskCreatePinnedToCore(
        task_speed,       // task function
        "speed",          // task name
        2048,             // stack size
        (void*)speed_pin, // parameters
        10,               // priority
        &th_speed,        // handler 
        APP_CPU_NUM       // core number
    );
}

void task_speed(void *arg){
    (void)arg;

    const gpio_num_t gpio_pin = (gpio_num_t)arg;

    // create task variables
    const int send_rate_ms = (int)(1000.0 / (float)(TASK_SPEED_SEND_RATE_Hz));
    const int send_rate_s = (int)(1.0f / (float)(TASK_SPEED_SEND_RATE_Hz));
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
            pcnt_unit_get_count(pcnt_unit, &pulse_count);
            pcnt_unit_clear_count(pcnt_unit);

            // -----calculate-----
            float meters = (pulse_count / WHEEL_EDGES) * WHEEL_CIRC;
            spdmt.value = meters / send_rate_s; // in meter/second
            spdmt.value *= ms2kmh;          
            
            // -----send spped data through esp-now to receiver-----
            esp_now_send(mac_address_ECU_front, (uint8_t *) &spdmt, sizeof(spdmt));
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // free up the processor
    }
}