/**
 * @file main.h
 * @authors
 *      Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief Parahybaja's embedded system: ECU Front
 * @version 5.0
 * @date 2024-01-23
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
#include "task/display.h"
#include "task/task_example.h"

#include "espnow_callback.h"

static const char* TAG = "ECU_front";

static const gpio_num_t alive_pin = GPIO_NUM_12;

static const int cr = 8;  // coding rate
static const int sbw = 1; // signal bandwidth
static const int sf = 7;  // spreading factor rate

void app_main(void) {
    ESP_LOGW(TAG, "ECU front v5");

    system_queue_init();

    // system_espnow_init();
    // register_callbacks();

    // print_mac_address();

    system_lora_init(cr, sbw, sf);

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
        task_display,  // task function
        "display",     // task name
        4096,          // stack size
        NULL,          // parameters
        10,            // priority
        &th_example,   // handler
        APP_CPU_NUM    // core number
    );

    xTaskCreatePinnedToCore(
        task_lora_sender, // task function
        "lora sender",    // task name
        4096,             // stack size
        NULL,             // parameters
        8,                // priority
        &th_lora,         // handler
        PRO_CPU_NUM       // core number
    );

    // just for test
    vTaskDelay(pdMS_TO_TICKS(5000));
    sensor_t sensor = {};

    sensor.type = SPEEDOMETER;
    sensor.value = 25;
    xQueueSend(qh_speed, &sensor, pdMS_TO_TICKS(0));
    vTaskDelay(pdMS_TO_TICKS(500));

    sensor.type = RPM;
    sensor.value = 2500;
    xQueueSend(qh_rpm, &sensor, pdMS_TO_TICKS(0));
    vTaskDelay(pdMS_TO_TICKS(500));

    sensor.type = FUEL_EMERGENCY;
    sensor.value = 1;
    xQueueSend(qh_fuel_emer, &sensor, pdMS_TO_TICKS(0));
    vTaskDelay(pdMS_TO_TICKS(500));

    sensor.type = BATTERY;
    sensor.value = 12.0;
    xQueueSend(qh_battery, &sensor, pdMS_TO_TICKS(0));
    vTaskDelay(pdMS_TO_TICKS(500));

    sensor.type = AMBIENT_TEMP;
    sensor.value = 25.0;
    xQueueSend(qh_temp, &sensor, pdMS_TO_TICKS(0));
    vTaskDelay(pdMS_TO_TICKS(500));

    sensor.type = TILT_X;
    sensor.value = 12.0;
    xQueueSend(qh_tilt_x, &sensor, pdMS_TO_TICKS(0));
    vTaskDelay(pdMS_TO_TICKS(500));

    sensor.type = TILT_Y;
    sensor.value = -5.0;
    xQueueSend(qh_tilt_y, &sensor, pdMS_TO_TICKS(0));
    vTaskDelay(pdMS_TO_TICKS(500));
}
