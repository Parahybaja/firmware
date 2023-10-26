/**
 * @file espnow_callback.h
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief local espnow callback functions 
 * @version 0.2
 * @date 2023-10-22
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
    log_i("data received");
}

#endif // __ESPNOW_CALLBACK_H__