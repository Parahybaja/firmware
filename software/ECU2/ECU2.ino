/**
 * @file ECU2.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief 
 * @version 0.1
 * @date 2022-10-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/* 
    command list
- start: start writing data
- stop: stop writing data
- new: create new file
- restart: restart base esp
*/

// -----FreeRTOS includes-----
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include "soc/rtc_wdt.h"

// -----espnow includes-----
#include <esp_now.h>
#include <WiFi.h>

// -----external modules includes-----
//

// -----include external files-----
#include "configs.h"      // general configs
#include "var_global.h"   // global variables must be declared before functions definitions
#include "prototypes.h"   // functions prototypes must be explicitly declared 
#include "tasks.h"        // task functions definitions
#include "system.h"       // system functions
#include "esp-now.h"      // esp-now functions

void setup(){
    // init system general modules (pinMode and Serial)
    init_system();

    // init system handlers such as queues and semaphores
    init_system_handlers();

    // init system handlers such as queues and semaphores
    init_espnow();

    // -----external modules initialization-----
    // 

    // -----final confirmation-----
    INFO("INFO_2: setup successfully");
    delay(50); // give time to send the espnow message

    // -----fire up tasks-----
    xTaskCreate(task_sensor_1,         // task function
                "sensor 1",            // task name
                2048,                  // stack size
                NULL,                  // parameters
                10,                    // priority
                &TaskHandle_sensor_1); // handler  

    xTaskCreate(task_sensor_2,         // task function
                "sensor 2",            // task name
                2048,                  // stack size
                NULL,                  // parameters
                10,                    // priority
                &TaskHandle_sensor_2); // handler  
}

void loop(){
    // alive signal
    digitalWrite(PIN_LED_ALIVE, HIGH);
    delay(100);
    digitalWrite(PIN_LED_ALIVE, LOW);
    delay(1000);
}