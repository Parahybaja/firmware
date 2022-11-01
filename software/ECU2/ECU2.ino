/**
 * @file ECU2.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief 
 * @version 0.2
 * @date 2022-10-31
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// -----FreeRTOS includes-----
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include "soc/rtc_wdt.h"

// -----espnow includes-----
#include <esp_now.h>
#include <WiFi.h>

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

    // -----final confirmation-----
    INFO("INFO_2: setup successfully");
    delay(50); // give time to send the espnow message

    // -----fire up tasks-----
    init_tasks();
}

void loop(){
    // -----delete loop-----
    vTaskDelete(NULL);
}