/**
 * @file ECU_Front.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief 
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

void setup(){
    // LED alive config
    pinMode(PIN_LED_ALIVE, OUTPUT); 

    // multiple SPI config
    // pre_config_SPI();

    // init system general modules (pinMode and Serial)
    init_system();

    // -----header-----
    Serial.println("|-------------------------|");
    Serial.println("|                         |");
    Serial.println("|        ECU_Front        |");
    Serial.println("|                         |");
    Serial.println("|-------------------------|");

    // init system handlers such as queues and semaphores
    // -----create semaphores-----
    // sh_SD = xSemaphoreCreateMutex();
    // if(sh_SD == NULL)
    //     ERROR("ERROR_1: SD Semaphore init failed", false);

    sh_global_vars = xSemaphoreCreateMutex();
    if(sh_global_vars == NULL)
        ERROR("ERROR_1: global vars Semaphore init failed", false);

    // -----create queues-----
    // qh_rpm        = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    // qh_speed      = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    // qh_fuel_level = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    // qh_fuel_emer  = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_battery    = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    // qh_rollover   = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    // qh_tilt_x     = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    // qh_tilt_y     = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    // qh_tilt_z     = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));

    // init system handlers such as queues and semaphores
    init_espnow();

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    // -----final confirmation-----
    INFO("INFO_1: it's all configured!");
    delay(50); // give time to send the espnow message

    // -----fire up tasks-----
    xTaskCreate(task_display,      // task function
                "display control", // task name
                2048,              // stack size
                NULL,              // parameters
                10,                // priority
                &th_display);      // handler 

    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message

    xTaskCreate(task_battery,   // task function
                "task_battery", // task name
                2048,           // stack size
                NULL,           // parameters
                10,             // priority
                &th_battery);   // handler 

    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
    
    xTaskCreate(task_send_pack, // task function
                "send pack",    // task name
                2048,           // stack size
                NULL,           // parameters
                10,             // priority
                &th_send_pack); // handler 
}

void loop(){
    digitalWrite(PIN_LED_ALIVE, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    digitalWrite(PIN_LED_ALIVE, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}