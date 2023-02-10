/**
 * @file task_telemetry.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief telemetry task, functions and configs
 * @version 0.1
 * @date 2023-02-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__

#include "communication.h"

/**
 * @brief read from the global vars and send pack to BOXECU
 * 
 * @param arg void arg
 */
void task_send_pack(void*);

void task_send_pack(void *arg){
    (void)arg;

    // -----create local variables-----
    system_t system_copy;
    uint32_t timer;

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_1: %d bytes remaining of the send pack task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----update timer-----
    timer = millis();

    while (true){
        if ((millis() - timer) >= TASK_SENDPACK_RATE_ms){
            // -----increment timer-----
            timer += TASK_SENDPACK_RATE_ms;

            // -----add to buffer-----
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_copy = system_global;
            xSemaphoreGive(sh_global_vars);

            // -----send system data through esp-now-----
            esp_now_send(address_ECU_BOX, (uint8_t *) &system_copy, sizeof(system_copy));
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

#endif // __TELEMETRY_H__