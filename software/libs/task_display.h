#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "communication.h"

void task_display(void*);

/**
 * @brief display control task
 * 
 * @param arg void arg
 */
void task_display(void *arg){
    (void)arg;

    // -----create local variables-----
    sensor_t recv_bat = {BATTERY, 0.0};


#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_1: %d bytes remaining of the display control task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    while (true){
        if (xQueueReceive(qh_battery, &recv_bat, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.rpm = recv_bat.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            Serial.print("bat:"); // debug
            Serial.println(recv_bat.value);
        }
        vTaskDelay(1 / portTICK_PERIOD_MS); // give time to send the espnow message
    }
}

#endif // __DISPLAY_H__