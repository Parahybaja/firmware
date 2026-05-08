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
#include "task/rollover.h"
// #include "task/task_example.h"
// #include "task/timer.h"

#include "espnow_callback.h"

static const char* TAG = "ECU_front";

static const gpio_num_t alive_pin = GPIO_NUM_12;

static const uint8_t mpu_calibrate = false;

/* LoRa preamble */
static const int cr = 8;  // coding rate
static const int sbw = 7; // signal bandwidth
static const int sf = 7;  // spreading factor rate

void app_main(void) {
    ESP_LOGW(TAG, "ECU front v6");

    system_queue_init();

    system_espnow_init();
    register_callbacks();

    print_mac_address();

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
        task_display,          // task function
        "display",             // task name
        4096,                  // stack size
        NULL,                  // parameters
        10,                    // priority
        &th_display_nextion,   // <--- Mudar de th_example para th_display_nextion
        APP_CPU_NUM            // core number
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

    xTaskCreatePinnedToCore(
        task_rollover,        
        "rollover",           
        4096,                 
        (void *)(uintptr_t)mpu_calibrate, // Cast seguro para evitar o erro de tamanho
        8,                    
        &th_rollover,         
        APP_CPU_NUM           
    );

    /*xTaskCreatePinnedToCore(
        task_timer,   // task function
        "timer",      // task name
        2048,             // stack size
        NULL, // parameters
        8,                // priority
        &th_timer,        // handler
        APP_CPU_NUM       // core number
    );*/
}
