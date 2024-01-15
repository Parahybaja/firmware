/**
 * @file main.h
 * @authors 
 *      Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief Parahybaja's embedded system: ECU Rear
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

static const char *TAG = "ECU_rear";

void app_main(void) {
    
    ESP_LOGD(TAG, "ECU rear v5");

    // -----fire up tasks-----
    gpio_num_t gpio_pin = GPIO_NUM_12;
    xTaskCreatePinnedToCore(
        task_alive_LED,  // task function
        "alive LED",     // task name
        2048,            // stack size
        (void*)gpio_pin, // parameters
        8,               // priority
        &th_alive,       // handler 
        APP_CPU_NUM      // core number
    );
}
