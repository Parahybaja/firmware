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
void ERROR(char*);
void ERROR(char*, bool);
void INFO(const char*);
void INFO(const char*, bool);
void send_debug(const char*);
void on_error(char*, bool);

/**
 * @brief init system general modules (pinMode, Serial and SPI)
 * 
 */
void init_system(void){
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
    esp_now_register_send_cb(OnDataSent);

    // register peer
    esp_now_peer_info_t peerInfo = {};
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    // control peer
    memcpy(peerInfo.peer_addr, address_ECUBOX, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("ERROR_1: Failed to add control peer", false); // DO NOT send through esp-now
    INFO("INFO_1: control peer added");
    delay(50); // give time to send the espnow message

    // sender peer
    memcpy(peerInfo.peer_addr, address_ECU1, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("ERROR_1: Failed to add sender peer", false); // DO NOT send through esp-now
    INFO("INFO_1: sender peer added");
    delay(50); // give time to send the espnow message

    esp_now_register_recv_cb(OnDataRecv);
}

/**
 * @brief ESP-NOW on data sent callback
 * 
 * @param mac_addr device MAC address
 * @param status esp-now status
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
#if DEBUG_MODE
    // -----check if the data was delivered-----
    Serial.print("Send status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
#endif
}

/**
 * @brief ESP-NOW on data receive callback
 * 
 * @param mac device MAC address
 * @param incomingData data received
 * @param len array size 
 */
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len){
    if (len ==  sizeof(cmd_t)){
        cmd_t config = {};
        memcpy(&config, incomingData, sizeof(config));

        if (config.command == CMD_START){
            INFO("INFO_1: start writing data");
            delay(50); // give time to send the espnow message
        }
        else if (config.command == CMD_STOP){
            INFO("INFO_1: stop writing data");
            delay(50); // give time to send the espnow message
        }
        else if (config.command == CMD_NEW_FILE){ 
            INFO("INFO_1: new file created");
            delay(50); // give time to send the espnow message
        }
        else if (config.command == CMD_RESTART){
            INFO("INFO_1: restarting . . .\n----------------------");
            delay(50); // give time to send the espnow message

            ESP.restart();
        }
        else{
            INFO("INFO_1: command does not exist");
            delay(50); // give time to send the espnow message
        }
    }
    else if (len ==  sizeof(sensor_t)){
        sensor_t sensor = {};
        memcpy(&sensor, incomingData, sizeof(sensor));

        if (sensor.type == RPM){
            // -----send RPM data through queue-----
            Serial.println("RPM in");
            xQueueSend(qh_rpm, &sensor, pdMS_TO_TICKS(0));
        }
        else if (sensor.type == SPEEDOMETER){
            // -----send speed data through queue-----
            xQueueSend(qh_speed, &sensor, pdMS_TO_TICKS(0));
        }
        else if (sensor.type == FUEL_EMERGENCY){
            // -----send speed data through queue-----
            xQueueSend(qh_fuel_emer, &sensor, pdMS_TO_TICKS(0));
        }
        else {
            INFO("INFO_1: unknown sensor type");
        }
    }
    else {
        INFO("INFO_1: unknown incoming data");
    }
}

/**
 * @brief print ERROR messages on serial and through esp-now:
 * 
 * @param returnMsg const *char: message to be sent
 */
void ERROR(char returnMsg[]){
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
void ERROR(char returnMsg[], bool espnow_active){
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
    if (sizeof(returnMsg) <= ESPNOW_BUFFER_SIZE){
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
void on_error(char returnMsg[], bool espnow_active){
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