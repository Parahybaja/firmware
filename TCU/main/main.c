/**
 * @file main.h
 * @authors
 *      Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief Parahybaja's embedded system: TCU - Telemetry Control Unit
 * @version 5.0
 * @date 2024-01-24
 *
 * @copyright Copyright (c) 2024
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  5.0.0    Jefferson L.  24/01/2024  convertion to esp-idf
 *
 */

#include <stdio.h>

#include "system.h"
#include "task/alive.h"
#include "task/task_example.h"

static const char* TAG = "TCU";

static const gpio_num_t alive_pin = GPIO_NUM_12;

/* LoRa preamble */
static const int cr = 8;  // coding rate
static const int sbw = 3; // signal bandwidth
static const int sf = 7;  // spreading factor rate

void app_main(void) {
    ESP_LOGW(TAG, "TCU v5");

    system_lora_init(cr, sbw, sf);

    esp_log_level_set("*", ESP_LOG_WARN);

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
        task_lora_receiver, // task function
        "lora receiver",    // task name
        4096,               // stack size
        NULL,               // parameters
        9,                  // priority
        &th_lora,           // handler
        PRO_CPU_NUM         // core number
    );
}
