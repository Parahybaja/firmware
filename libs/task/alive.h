/**
 * @file task_alive.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief controller alive signal, shows that it's not lockup
 * @version 0.1
 * @date 2023-02-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __ALIVE_H__
#define __ALIVE_H__

#include "system.h"

// ----- pinout -----
extern const uint8_t pin_alive_LED;

/**
 * @brief keep blinking a LED to ensure the CPU is working properly
 * 
 */
void task_alive_LED(void*);

void task_alive_LED(void *arg) {
    (void)arg;

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "%d bytes remaining of alive LED task space", 
        uxHighWaterMark); 
    INFO(info, false);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    while(true){
        digitalWrite(pin_alive_LED, HIGH);

        vTaskDelay(100 / portTICK_PERIOD_MS);

        digitalWrite(pin_alive_LED, LOW);
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

#endif // __ALIVE_H__