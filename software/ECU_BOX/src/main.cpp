/**
 * @file ECU_BOX.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief 
 * @version 3.1
 * @date 2023-02-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>

#include "communication.h"
#include "espnow_callback.h"
#include "tasks/task_alive.h"

// ----- pinout -----
const uint8_t pin_alive_LED = 33;

void setup(){
    // init system general modules (pinMode and Serial)
    init_system();

    // -----header-----
    log_i("embedded system: ECU BOX");

    pinMode(pin_alive_LED, OUTPUT);

    // init system handlers such as queues and semaphores
    init_espnow();

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    // -----final confirmation-----
    INFO("it's all configured!");

    xTaskCreatePinnedToCore(
        task_alive_LED, // task function
        "alive LED",    // task name
        1024,           // stack size
        NULL,           // parameters
        5,              // priority
        &th_alive,      // handler
        APP_CPU_NUM     // core number
    );   
}

void loop(){
    vTaskDelete(NULL);
}