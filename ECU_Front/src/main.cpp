/**
 * @file ECU_Front.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief Parahybaja's embedded system: Front ECU
 * @version 4.1
 * @date 2023-10-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>

// common libs
#include "system.h"
#include "task/telemetry.h"
#include "task/rollover.h"
#include "task/display.h"
// #include "tasks/task_display_LCD.h" // backup
#include "task/battery.h"
#include "task/alive.h"

#include "espnow_callback.h"

// ----- pinout -----
const uint8_t pin_alive_LED = 12;
const uint8_t pin_battery = 35;

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

    battery_config_t battery_config;
    battery_config.R1 = 160000.0;
    battery_config.R2 = 21700.0;
    uint8_t empty_mac[6] = {0, 0, 0, 0, 0, 0};
    memcpy(battery_config.mac, empty_mac, sizeof(empty_mac));

    xTaskCreatePinnedToCore(
        task_battery,      // task function
        "battery voltage", // task name
        4096,              // stack size
        &battery_config,   // parameters
        10,                // priority
        &th_battery,       // handler 
        APP_CPU_NUM        // core number
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