/**
 * @file ECU_Front.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief Parahybaja's embedded system: Front ECU
 * @version 3.1
 * @date 2023-02-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>

// common libs
#include "communication.h"
#include "tasks/task_telemetry.h"
#include "tasks/task_battery.h"
#include "tasks/task_display_LCD.h"
#include "tasks/task_alive.h"

#include "espnow_callback.h"

void setup(){
    // init system general modules (pinMode and Serial)
    init_system();

    // -----header-----
    log_i("embedded system: ECU Front");

    // -----create semaphores-----
    sh_global_vars = xSemaphoreCreateMutex();
    if(sh_global_vars == NULL)
        ERROR("global vars Semaphore init failed", false);

    // -----create queues-----
    qh_speed     = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_rpm       = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_fuel_emer = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_battery   = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));

    // init system handlers such as queues and semaphores
    init_espnow();

    // register esp-now callbacks
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    // -----final confirmation-----
    INFO("it's all configured!");
    delay(50); // give time to send the espnow message

    // -----fire up tasks-----
    xTaskCreatePinnedToCore(
        task_display_LCD,      // task function
        "display LCD control", // task name
        2048,                  // stack size
        NULL,                  // parameters
        10,                    // priority
        &th_display_LCD,       // handler 
        APP_CPU_NUM            // core number
    );

    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
    
    xTaskCreatePinnedToCore(
        task_send_pack, // task function
        "send pack",    // task name
        2048,           // stack size
        NULL,           // parameters
        10,             // priority
        &th_send_pack,  // handler 
        APP_CPU_NUM     // core number
    );

    xTaskCreatePinnedToCore(
        task_alive_LED, // task function
        "alive LED",    // task name
        1024,           // stack size
        NULL,           // parameters
        5,              // priority
        &th_alive,      // handler
        APP_CPU_NUM);   // core number
}

void loop(){
    vTaskDelete(NULL);
}