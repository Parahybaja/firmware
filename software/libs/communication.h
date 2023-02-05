#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include "soc/rtc_wdt.h"
#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>

#include "configs.h"
#include "definitions.h"

void init_system(void);
void init_espnow();
void OnDataSent(const uint8_t*, esp_now_send_status_t);
void OnDataRecv(const uint8_t*, const uint8_t*, int);
void ERROR(const char*);
void ERROR(const char*, bool);
void INFO(const char*);
void INFO(const char*, bool);
void send_debug(const char*);
void on_error(const char*, bool);

/**
 * @brief init system general modules (pinMode, Serial and SPI)
 * 
 */
void init_system(void){
    // LED alive config
    pinMode(PIN_LED_ALIVE, OUTPUT); 

    #if DEBUG_MODE
        // time to platformIO switch tabs
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    #endif

    // -----Serial initiallization-----
    Serial.begin(SERIAL_FREQ);
    while (!Serial){}; // wait until is initialized
    Wire.begin();

    // -----header-----
    Serial.println("|-------------------------|");
    Serial.println("|                         |");
    Serial.println("|        ECU_Front        |");
    Serial.println("|                         |");
    Serial.println("|-------------------------|");
}


/**
 * @brief init esp-now
 */
void init_espnow(){    
    // -----ESPNOW settings-----
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_now_init() != ESP_OK)    // check if was initialized successfully
        ERROR("ERROR_1: initializing ESP-NOW", false); // DO NOT send through esp-now
    INFO("INFO_1: Done initializing ESP-NOW", false); // DO NOT send through esp-now
    delay(50); // give time to send the espnow message

    // register peer
    esp_now_peer_info_t peerInfo = {};
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    // control peer
    memcpy(peerInfo.peer_addr, address_ECU_BOX, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("ERROR_1: Failed to add control peer", false); // DO NOT send through esp-now
    INFO("INFO_1: control peer added");
    delay(50); // give time to send the espnow message

    // sender peer
    memcpy(peerInfo.peer_addr, address_ECU_Front, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("ERROR_1: Failed to add sender peer", false); // DO NOT send through esp-now
    INFO("INFO_1: sender peer added");
    delay(50); // give time to send the espnow message
}


/**
 * @brief print ERROR messages on serial and through esp-now:
 * 
 * @param returnMsg const *char: message to be sent
 */
void ERROR(const char returnMsg[]){
    Serial.println();
    Serial.println(returnMsg);
    send_debug(returnMsg);

    on_error(returnMsg, true);
}

/**
 * @brief print ERROR messages on serial and through esp-now:
 * 
 * @param returnMsg const *char: message to be sent
 * @param espnow_active bool: esp-now active flag
 */
void ERROR(const char returnMsg[], bool espnow_active){
    Serial.println();
    Serial.println(returnMsg);
    if (espnow_active)
        send_debug(returnMsg);
    
    on_error(returnMsg, espnow_active);
}

/**
 * @brief print INFO messages on serial and through esp-now:
 * 
 * @param returnMsg const *char: message to be sent
 */
void INFO(const char returnMsg[]){
    Serial.println();
    Serial.println(returnMsg);
    send_debug(returnMsg);
}

/**
 * @brief print INFO messages on serial and through esp-now:
 * 
 * @param returnMsg const *char: message to be sent
 * @param espnow_active bool: esp-now active flag
 */
void INFO(const char returnMsg[], bool espnow_active){
    Serial.println();
    Serial.println(returnMsg);
    if (espnow_active)
        send_debug(returnMsg);
}

/**
 * @brief send debug messages through esp-now
 * 
 * @param returnMsg const *char: message to be sent
 */
void send_debug(const char returnMsg[]){
    if (strlen(returnMsg) <= ESPNOW_BUFFER_SIZE){
        // mudar para char*
        // debug_t msg_data;
        // msg_data.id = BOARDID;
        // strcpy(msg_data.msg, returnMsg);
        // esp_now_send(address_ECUBOX, (uint8_t *) &msg_data, sizeof(msg_data));
    }
    else
        Serial.println("ERROR_1: espnow buffer overflow");
}

/**
 * @brief send error messages through esp-now with option
 * 
 * @param returnMsg const *char: message to be sent
 * @param espnow_active bool: esp-now active flag
 */
void on_error(const char returnMsg[], bool espnow_active){
    if (REBOOT_ON_ERROR){
        ESP.restart();
        delay(1);
    }
    else{
        while(true){
            Serial.println(returnMsg);

            if (espnow_active)
                send_debug(returnMsg);
            
            delay(DEBUG_DELAY);
        }
    }
}

#endif // COMMUNICATION_H