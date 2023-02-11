/**
 * @file ECU_Rear.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief Parahybaja's embedded system: Rear ECU
 * @version 3.0
 * @date 2023-02-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>

#include "communication.h"
#include "espnow_callback.h"
#include "task_battery.h"
#include "task_alive.h"

void setup(){
    // init system general modules (pinMode and Serial)
    init_system();

    // -----header-----
    Serial.println("|------------------------|");
    Serial.println("|                        |");
    Serial.println("|        ECU_Rear        |");
    Serial.println("|                        |");
    Serial.println("|------------------------|");

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
        task_battery,   // task function
        "task_battery", // task name
        2048,           // stack size
        NULL,           // parameters
        10,             // priority
        &th_battery,    // handler
        APP_CPU_NUM     // core number
    ); 

    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
    
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