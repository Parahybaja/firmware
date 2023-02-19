/**
 * @file ECU_Rear.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief Parahybaja's embedded system: Rear ECU
 * @version 3.1
 * @date 2023-02-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>

#include "communication.h"
#include "espnow_callback.h"
#include "tasks/task_speedometer.h"
#include "tasks/task_battery.h"
#include "tasks/task_fuel.h"
#include "tasks/task_RPM.h"
#include "tasks/task_alive.h"

// ----- pinout -----
const uint8_t pin_rpm = 4;
const uint8_t pin_fuel = 5;
const uint8_t pin_battery = 35;
const uint8_t pin_alive_LED = 2;
const uint8_t pin_speedometer = 17;

void setup() {
    // init system general modules (pinMode and Serial)
    init_system();

    // -----header-----
    log_i("embedded system: ECU Rear");

    pinMode(pin_rpm, INPUT);
    pinMode(pin_fuel, INPUT);
    pinMode(pin_battery, INPUT);
    pinMode(pin_alive_LED, OUTPUT);
    pinMode(pin_speedometer, INPUT);

    attachInterrupt(pin_speedometer, isr_spdmt, CHANGE);
    attachInterrupt(pin_rpm, isr_rpm, HIGH);

    // init system handlers such as queues and semaphores
    init_espnow();

    // register esp-now callbacks
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    // -----final confirmation-----
    INFO("it's all configured!");

    // -----fire up tasks-----
    xTaskCreatePinnedToCore( // 
        task_battery,   // task function
        "task_battery", // task name
        2048,           // stack size
        NULL,           // parameters
        9,              // priority
        &th_battery,    // handler
        APP_CPU_NUM     // core number
    ); 
    
    xTaskCreatePinnedToCore(
        task_RPM,   // task function
        "task RPM", // task name
        2048,       // stack size
        NULL,       // parameters
        10,         // priority
        &th_RPM,    // handler
        APP_CPU_NUM // core number
    ); 

    xTaskCreatePinnedToCore(
        task_fuel,   // task function
        "task fuel", // task name
        2048,        // stack size
        NULL,        // parameters
        11,          // priority
        &th_fuel,    // handler
        APP_CPU_NUM  // core number
    ); 

    xTaskCreatePinnedToCore(
        task_speedometer,   // task function
        "task speedometer", // task name
        2048,               // stack size
        NULL,               // parameters
        12,                 // priority
        &th_speedometer,    // handler
        APP_CPU_NUM         // core number
    ); 
    
    xTaskCreatePinnedToCore(
        task_alive_LED, // task function
        "alive LED",    // task name
        2048,           // stack size
        NULL,           // parameters
        5,              // priority
        &th_alive,      // handler
        APP_CPU_NUM     // core number
    );   
}

void loop() {
    vTaskDelete(NULL);
}