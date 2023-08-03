/**
 * @file ECU_Front.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief Parahybaja's embedded system: Front ECU
 * @version 3.2
 * @date 2023-02-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>

// common libs
#include "communication.h"
#include "tasks/task_telemetry.h"
#include "tasks/task_rollover.h"
#include "tasks/task_display.h"
// #include "tasks/task_display_LCD.h" // backup
#include "tasks/task_alive.h"

#include "espnow_callback.h"

// ----- pinout -----
const uint8_t pin_alive_LED = 33;

void setup(){
    // init system general modules (pinMode and Serial)
    init_system();

    // -----header-----
    log_i("embedded system: ECU Front");

    pinMode(pin_alive_LED, OUTPUT);

    // -----create semaphores-----
    sh_global_vars = xSemaphoreCreateMutex();
    if(sh_global_vars == NULL)
        ERROR("global vars Semaphore init failed", false);

    // -----create queues-----
    qh_speed     = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_rpm       = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_fuel_emer = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_battery   = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_temp      = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_tilt_x    = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_tilt_y    = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_rollover  = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));

    // init system handlers such as queues and semaphores
    init_espnow();

    // register esp-now callbacks
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    // -----final confirmation-----
    INFO("it's all configured!");

    // -----fire up tasks-----
    xTaskCreatePinnedToCore(
        task_display,      // task function
        "display control", // task name
        4096,              // stack size
        NULL,              // parameters
        10,                // priority
        &th_display,       // handler 
        APP_CPU_NUM        // core number
    );
    
    // ----- backup -----
    // xTaskCreatePinnedToCore( 
    //     task_display_LCD,      // task function
    //     "display LCD control", // task name
    //     2048,                  // stack size
    //     NULL,                  // parameters
    //     10,                    // priority
    //     &th_display_LCD,       // handler 
    //     APP_CPU_NUM            // core number
    // );
    
    xTaskCreatePinnedToCore(
        task_rollover, // task function
        "rollover",    // task name
        4096,          // stack size
        NULL,          // parameters
        9,             // priority
        &th_rollover,  // handler 
        APP_CPU_NUM    // core number
    );

    xTaskCreatePinnedToCore(
        task_telemetry, // task function
        "telemetry",    // task name
        2048,           // stack size
        NULL,           // parameters
        11,             // priority
        &th_telemetry,  // handler 
        APP_CPU_NUM     // core number
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

void loop(){
    vTaskDelete(NULL);
}