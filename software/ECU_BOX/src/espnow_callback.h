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
    // -----check if the data was delivered-----
    if (status == ESP_NOW_SEND_SUCCESS)
        log_i("Send status:\tDelivery Success");
    else
        log_e("Send status:\tDelivery Fail");
}

/**
 * @brief ESP-NOW on data receive callback
 * 
 * @param mac device MAC address
 * @param incomingData data received
 * @param len array size 
 */
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len){
    #if DEBUG_MODE
        Serial.print("DEBUG: packet received size: ");
        Serial.println(len);
    #endif

    if (len == sizeof(debug_t)){
        debug_t debug;
        memcpy(&debug, incomingData, sizeof(debug));

        Serial.print("DEBUG:");
        Serial.println(debug.msg);
    }
    else if (len == sizeof(system_t)){
        system_t system;
        memcpy(&system, incomingData, sizeof(system));

        String payload = 
            "DATA:" +
            String(system.rpm, 2) + "," +
            String(system.speed, 2) + "," +
            String(system.fuel_emer, 2) + "," +
            String(system.battery, 2) + "," +
            String(system.temp, 2) + "," +
            String(system.rollover, 2) + "," +
            String(system.tilt_x, 2) + "," +
            String(system.tilt_y, 2) + "," +
            String(system.tilt_z, 2);

        Serial.println(payload);
    }
}

#endif // __ESPNOW_CALLBACK_H__