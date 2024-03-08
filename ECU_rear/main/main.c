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
#include "task/rpm.h"
#include "task/alive.h"
#include "task/battery.h"
#include "task/speedometer.h"
// #include "task/fuel_em.h"

#include "espnow_callback.h"

static const char *TAG = "ECU_rear";

static const gpio_num_t rpm_pin = GPIO_NUM_36;
static const gpio_num_t speed_pin = GPIO_NUM_32;
static const gpio_num_t alive_pin = GPIO_NUM_2;
// static const gpio_num_t fuel_em_pin = GPIO_NUM_34;
static const battery_config_t battery_config = {
    .adc_channel = ADC_CHANNEL_7, // GPIO 35
    .R1 = 10e3,
    .R2 = 2.1e3 // calibrated
};

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

    xTaskCreatePinnedToCore(
        task_rpm,       // task function
        "speed",          // task name
        2048,             // stack size
        (void*)rpm_pin, // parameters
        10,               // priority
        &th_rpm,        // handler 
        APP_CPU_NUM       // core number
    );
}
