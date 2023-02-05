/**
 * @file ECU_BOX.ino
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

void setup(){
    // multiple SPI config
    // pre_config_SPI();

    // init system general modules (pinMode and Serial)
    init_system();

    // -----header-----
    Serial.println("|-----------------------|");
    Serial.println("|                       |");
    Serial.println("|        ECU_BOX        |");
    Serial.println("|                       |");
    Serial.println("|-----------------------|");

    // init system handlers such as queues and semaphores
    init_espnow();

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    // -----final confirmation-----
    INFO("INFO_1: it's all configured!");
    delay(50); // give time to send the espnow message
}

void loop(){
    digitalWrite(PIN_LED_ALIVE, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    digitalWrite(PIN_LED_ALIVE, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}