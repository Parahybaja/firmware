/**
 * @file espnow_callback.h
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief local espnow callback functions 
 * @version 0.1
 * @date 2023-10-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __ESPNOW_CALLBACK_H__
#define __ESPNOW_CALLBACK_H__

#include "system.h"

/**
 * @brief ESP-NOW on data sent callback
 * 
 * @param mac_addr device MAC address
 * @param status esp-now status
 */
void OnDataSent(const uint8_t*, esp_now_send_status_t);

/**
 * @brief ESP-NOW on data receive callback
 * 
 * @param mac device MAC address
 * @param incomingData data received
 * @param len array size 
 */
void OnDataRecv(const uint8_t*, const uint8_t*, int);

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
    // -----check if the data was delivered-----
    if (status == ESP_NOW_SEND_SUCCESS) {
        log_i("Send status:\tDelivery Success");
    }
    else {
        log_e("Send status:\tDelivery Fail");
    } 
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len){
    if (len ==  sizeof(command_t)){
        command_t config;
        memcpy(&config, incomingData, sizeof(config));

        if (config == CMD_START){
            INFO("start writing data");
            delay(50); // give time to send the espnow message
        }
        else if (config == CMD_STOP){
            INFO("stop writing data");
            delay(50); // give time to send the espnow message
        }
        else if (config == CMD_NEW_FILE){ 
            INFO("new file created");
            delay(50); // give time to send the espnow message
        }
        else if (config == CMD_RESTART){
            INFO("restarting . . .\n----------------------");
            delay(50); // give time to send the espnow message

            ESP.restart();
        }
        else{
            INFO("command does not exist");
            delay(50); // give time to send the espnow message
        }
    }
    else if (len ==  sizeof(sensor_t)){
        sensor_t sensor = {};
        memcpy(&sensor, incomingData, sizeof(sensor));

        if (sensor.type == BATTERY){
            // -----send RPM data through queue-----
            xQueueSend(qh_battery, &sensor, pdMS_TO_TICKS(0));
        }
        else if (sensor.type == RPM){
            // -----send RPM data through queue-----
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
            INFO("unknown sensor type");
        }
    }
    else {
        INFO("unknown incoming data");
    }
}

#endif // __ESPNOW_CALLBACK_H__