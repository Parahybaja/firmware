
// /**
//  * @file ECU1.ino
//  * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
//  * @brief 
//  * @version 1.0
//  * @date 2023-01-30
//  * 
//  * @copyright Copyright (c) 2022
//  * 
//  */

// // -----FreeRTOS includes-----
// #include <freertos/FreeRTOS.h>
// #include <freertos/semphr.h>
// #include <freertos/task.h>
// #include "soc/rtc_wdt.h"

// // -----espnow includes-----
// #include <esp_now.h>
// #include <WiFi.h>

// // -----sd card includes-----
// #include <Wire.h>
// #include <FS.h>
// #include <SPI.h>
// #include <SD.h>

// // -----external modules-----
// #include <TwoMPU6050.h>
// #include "displaylib.h"

// // -----include external files-----
// #include "configs.h"    // general configs
// #include "var_global.h" // global variables must be declared before functions definitions
// #include "prototypes.h" // functions prototypes must be explicitly declared 
// #include "tasks.h"      // task functions definitions
// #include "SD_module.h"  // SD card module functions definitions
// #include "system.h"     // system functions
// #include "esp-now.h"    // esp-now functions

// void setup(){
//     // init system general modules (pinMode and Serial)
//     init_system();

//     // init system handlers such as queues and semaphores
//     init_system_handlers();

//     // init system handlers such as queues and semaphores
//     init_espnow();

//     // -----final confirmation-----
//     INFO("INFO_1: it's all configured!");
//     delay(50); // give time to send the espnow message

//     // -----fire up tasks-----
//     init_tasks();
// }

// void loop(){
//     // delete loop
//     vTaskDelete(NULL);
// }