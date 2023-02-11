/**
 * @file ECU_Front.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief Parahybaja's embedded system: Front ECU
 * @version 3.0
 * @date 2023-02-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>

#include "communication.h"
#include "espnow_callback.h"
#include "task_telemetry.h"
#include "task_battery.h"
#include "task_display.h"
#include "task_alive.h"

void setup(){
    // init system general modules (pinMode and Serial)
    init_system();

    // -----header-----
    Serial.println("|-------------------------|");
    Serial.println("|                         |");
    Serial.println("|        ECU_Front        |");
    Serial.println("|                         |");
    Serial.println("|-------------------------|");

    // -----create semaphores-----
    sh_global_vars = xSemaphoreCreateMutex();
    if(sh_global_vars == NULL)
        ERROR("ERROR_1: global vars Semaphore init failed", false);

    // -----create queues-----
    qh_battery = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));

    // init system handlers such as queues and semaphores
    init_espnow();

    // register esp-now callbacks
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    // -----final confirmation-----
    INFO("INFO_1: it's all configured!");
    delay(50); // give time to send the espnow message

    // -----fire up tasks-----
    xTaskCreatePinnedToCore(
        task_display,      // task function
        "display control", // task name
        2048,              // stack size
        NULL,              // parameters
        10,                // priority
        &th_display,       // handler 
        APP_CPU_NUM        // core number
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