/**
 * @file task_write_SD.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief SD task, functions and configs
 * @version 0.1
 * @date 2023-02-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __WRITE_SD_H__
#define __WRITE_SD_H__

#include "communication.h"
#include "SD_module.h"

/**
 * @brief read from the global vars and write to SD card
 * 
 * @param arg void arg
 */
void task_write_SD(void*);

void task_write_SD(void *arg){
    (void)arg;

    // -----create local variables-----
    system_t data_received;
    system_t buffer_data[BUFFER_SIZE];
    uint32_t timer;
    uint16_t buffer_count = 0;

    init_file(); // only return when initialize correctly

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_1: %d bytes remaining of the write SD task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----update timer-----
    timer = millis();

    while (true){
        if ((millis() - timer) >= TASK_SD_RATE_ms){
            // -----increment timer-----
            timer += TASK_SD_RATE_ms;

            // add to buffer
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                buffer_data[buffer_count] = system_global;
            xSemaphoreGive(sh_global_vars);

            // -----write data to SD card-----
            if(++buffer_count >= BUFFER_SIZE){
                // reset buffer counter
                buffer_count = false;

                // convert data to string
                String data_string = data_to_string(buffer_data);
                
                // add data to the file
                appendFile(SD, file_name.c_str(), data_string.c_str());

                // -----clear buffer-----
                memset(buffer_data, 0, sizeof(buffer_data));
            }
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

#endif // __WRITE_SD_H__