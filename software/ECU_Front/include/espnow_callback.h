#ifndef __ESPNOW_CALLBACK_H__
#define __ESPNOW_CALLBACK_H__

#include "communication.h"

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
    // if (len ==  sizeof(cmd_t)){
    //     cmd_t config = {};
    //     memcpy(&config, incomingData, sizeof(config));

    //     if (config.command == CMD_START){
    //         INFO("INFO_1: start writing data");
    //         delay(50); // give time to send the espnow message
    //     }
    //     else if (config.command == CMD_STOP){
    //         INFO("INFO_1: stop writing data");
    //         delay(50); // give time to send the espnow message
    //     }
    //     else if (config.command == CMD_NEW_FILE){ 
    //         INFO("INFO_1: new file created");
    //         delay(50); // give time to send the espnow message
    //     }
    //     else if (config.command == CMD_RESTART){
    //         INFO("INFO_1: restarting . . .\n----------------------");
    //         delay(50); // give time to send the espnow message

    //         ESP.restart();
    //     }
    //     else{
    //         INFO("INFO_1: command does not exist");
    //         delay(50); // give time to send the espnow message
    //     }
    // }
    // else if (len ==  sizeof(sensor_t)){
    //     sensor_t sensor = {};
    //     memcpy(&sensor, incomingData, sizeof(sensor));

    //     if (sensor.type == RPM){
    //         // -----send RPM data through queue-----
    //         Serial.println("RPM in");
    //         xQueueSend(qh_rpm, &sensor, pdMS_TO_TICKS(0));
    //     }
    //     else if (sensor.type == SPEEDOMETER){
    //         // -----send speed data through queue-----
    //         xQueueSend(qh_speed, &sensor, pdMS_TO_TICKS(0));
    //     }
    //     else if (sensor.type == FUEL_EMERGENCY){
    //         // -----send speed data through queue-----
    //         xQueueSend(qh_fuel_emer, &sensor, pdMS_TO_TICKS(0));
    //     }
    //     else {
    //         INFO("INFO_1: unknown sensor type");
    //     }
    // }
    // else {
    //     INFO("INFO_1: unknown incoming data");
    // }
}

#endif // __ESPNOW_CALLBACK_H__