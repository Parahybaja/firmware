/**
 * @file espnow_callback.h
 * @brief espnow callbacks definitions
 * @version 1.0.1
 * @date 2023-08-02
 *
 * @authors
 *      Jefferson Lopes (jefferson.lopes@ee.ufcg.edu.br)
 *
 * @copyright Copyright (c) 2023
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Jefferson L.  19/02/2023  last version
 *  1.0.0    Jefferson L.  02/08/2023  add AFV task to callbacks
 *  1.0.1    Jefferson L.  02/08/2023  update to use reset_time_counter function
 */

#ifndef __ESPNOW_CALLBACK_H__
#define __ESPNOW_CALLBACK_H__

#include "module/AV.h"
#include "system.h"

extern String msg;

/**
 * @brief ESP-NOW on data sent callback
 *
 * @param mac_addr device MAC address
 * @param status esp-now status
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    // -----check if the data was delivered-----
    if (status == ESP_NOW_SEND_SUCCESS) {
        snprintf(str_buffer, sizeof(str_buffer), "AV:Send status: Delivery SuccessZ", msg); // format string
        Serial.println(str_buffer);                                                          // send string to the server
        memset(str_buffer, 0, sizeof(str_buffer));                                           // reset buffer
    } else {
        snprintf(str_buffer, sizeof(str_buffer), "AV:Send status: Delivery FailZ", msg); // format string
        Serial.println(str_buffer);                                                       // send string to the server
        memset(str_buffer, 0, sizeof(str_buffer));                                        // reset buffer
    }
}

/**
 * @brief ESP-NOW on data receive callback
 *
 * @param mac device MAC address
 * @param incomingData data received
 * @param len array size
 */

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    if (len == sizeof(debug_t)) {
        debug_t debug;
        memcpy(&debug, incomingData, sizeof(debug));

        Serial.print("DEBUG:");
        Serial.println(debug.msg);
    } else if (len == sizeof(system_t)) {
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
    } else if (len == sizeof(board_t)) {
        board_t board_received;
        memcpy(&board_received, incomingData, sizeof(board_t));

        if (board_received.id == BOARD_01 && !initial_time) {
            initial_time = millis();
            Serial.println("AFV:started - ");
        } else if (board_received.id == BOARD_02 && initial_time && !final_time) {
            final_time = millis();

            float acc_time = (final_time - initial_time) / 1000.0; // calculate accelaration time in seconds

            snprintf(str_buffer, sizeof(str_buffer), "AFV:finished - %.2fs - ", acc_time); // format string
            Serial.println(str_buffer);                                                    // send string to the server
            memset(str_buffer, 0, sizeof(str_buffer));                                     // reset buffer

            reset_time_counter();
        } else if (board_received.id == BOARD_03) {
            snprintf(str_buffer, sizeof(str_buffer), "AFV:speed: %.2f km/h%c", board_received.speed, CMD_NEWLINE); // format string
            Serial.println(str_buffer);                                                                            // send string to the server
            memset(str_buffer, 0, sizeof(str_buffer));                                                             // reset buffer
        } else {
            Serial.println("ERROR:unknown board");
        }
    } else if (len == sizeof(sensor_t)) {
        sensor_t sensor_received;
        memcpy(&sensor_received, incomingData, sizeof(sensor_t));

        if (comp_array(mac, address_module_1)) {
            snprintf(str_buffer, sizeof(str_buffer), "AV:BAT1:%.2f", sensor_received.value); // format string
            Serial.println(str_buffer);                                                                            // send string to the server
            memset(str_buffer, 0, sizeof(str_buffer));
        } else if (comp_array(mac, address_module_2)) {
            snprintf(str_buffer, sizeof(str_buffer), "AV:BAT2:%.2f", sensor_received.value); // format string
            Serial.println(str_buffer);                                                                            // send string to the server
            memset(str_buffer, 0, sizeof(str_buffer));
        } else if (comp_array(mac, address_module_3)) {
            snprintf(str_buffer, sizeof(str_buffer), "AV:BAT3:%.2f", sensor_received.value); // format string
            Serial.println(str_buffer);                                                                            // send string to the server
            memset(str_buffer, 0, sizeof(str_buffer));
        } else {
            Serial.println("ERROR:Don´t have mac address");
        }
    } else {
        Serial.println("ERROR:incomming data does not match");
    }
}

#endif // __ESPNOW_CALLBACK_H__