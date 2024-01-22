/**
 * @file main.h
 * @authors
 *      Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief Parahybaja's embedded system: ECU Front
 * @version 5.0
 * @date 2024-01-15
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
#include "task/task_example.h"

#include "espnow_callback.h"

static const char* TAG = "ECU_front";

void app_main(void) {
    ESP_LOGD(TAG, "ECU front v5");

    print_mac_address();

    init_espnow();
    register_callbacks();

    // -----fire up tasks-----
    xTaskCreatePinnedToCore(task_example,  // task function
                            "example",     // task name
                            2048,          // stack size
                            NULL,          // parameters
                            10,            // priority
                            &th_example,   // handler
                            APP_CPU_NUM    // core number
    );

    gpio_num_t gpio_pin = GPIO_NUM_12;
    xTaskCreatePinnedToCore(task_alive_LED,   // task function
                            "alive LED",      // task name
                            2048,             // stack size
                            (void*)gpio_pin,  // parameters
                            8,                // priority
                            &th_alive,        // handler
                            APP_CPU_NUM       // core number
    );
}
