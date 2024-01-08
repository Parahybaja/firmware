/**
 * @file definitions.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief system's general types definitions and basic esp-now communication functions
 * @version 1.1
 * @date 2023-10-26
 *
 * @copyright Copyright (c) 2023
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Jefferson L.  10/02/2023  first version
 *  1.0.0    Jefferson L.  18/07/2023  move common_libs and rename a few modules
 *  1.1.0    Jefferson L.  26/10/2023  remove Wire begin
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <Arduino.h>

#include <WiFi.h>
#include <Wire.h>
#include <esp_now.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <soc/rtc_wdt.h>

#include "configs.h"

// -----command lookup table-----
typedef enum {
    CMD_NULL,     /*empty command*/
    CMD_START,    /*start command*/
    CMD_STOP,     /*stop command*/
    CMD_NEW_FILE, /*new file command*/
    CMD_RESTART   /*restart command*/
} command_t;

typedef enum {
    RPM,
    SPEEDOMETER,
    FUEL_LEVEL,
    FUEL_EMERGENCY,
    BATTERY,
    AMBIENT_TEMP,
    ROLLOVER,
    TILT_X,
    TILT_Y,
    TILT_Z
} sensor_type_t;

typedef struct {
    float rpm;
    float speed;
    float fuel_level;
    float fuel_emer;
    float battery;
    float temp;
    float rollover;
    float tilt_x;
    float tilt_y;
    float tilt_z;
} system_t;

// -----debug data type definition-----
typedef struct {
    char msg[ESPNOW_BUFFER_SIZE];
} debug_t;

// -----sensor data type definition-----
typedef struct {
    sensor_type_t type;
    float value; // sensor data must be float due to a bug in writing to the sd card
} sensor_t;

// -----system run time data-----
system_t system_global = {
    .rpm = 0.0,
    .speed = 0.0,
    .fuel_emer = 0.0,
    .battery = 0.0,
    .temp = 0.0,
    .rollover = 0.0,
    .tilt_x = 0.0,
    .tilt_y = 0.0,
    .tilt_z = 0.0};

// -----FreeRTOS objects-----
TaskHandle_t th_alive; // LED alive task handler
TaskHandle_t th_rollover;
TaskHandle_t th_battery;          // sensor 3 task handler
TaskHandle_t th_display;          // display control task handler
TaskHandle_t th_display_LCD;      // display control task handler
TaskHandle_t th_blink_LCD;        //
TaskHandle_t th_SD;               //
TaskHandle_t th_telemetry;        //
TaskHandle_t th_RPM;              //
TaskHandle_t th_fuel;             //
TaskHandle_t th_speedometer;      //
SemaphoreHandle_t sh_SD;          //
SemaphoreHandle_t sh_global_vars; //
SemaphoreHandle_t sh_i2c;         //
QueueHandle_t qh_rpm;             //
QueueHandle_t qh_speed;           //
QueueHandle_t qh_fuel_level;      //
QueueHandle_t qh_fuel_emer;       //
QueueHandle_t qh_battery;         //
QueueHandle_t qh_temp;            //
QueueHandle_t qh_rollover;        //
QueueHandle_t qh_tilt_x;          //
QueueHandle_t qh_tilt_y;          //
QueueHandle_t qh_tilt_z;          //

/**
 * @brief init system general modules (pinMode, Serial and SPI)
 */
void init_system(void);

/**
 * @brief init esp-now
 */
void init_espnow(void);

/**
 * @brief print ERROR messages on serial and through esp-now:
 *
 * @param returnMsg const *char: message to be sent
 */
void ERROR(const char *);

/**
 * @brief print ERROR messages on serial and through esp-now:
 *
 * @param returnMsg const *char: message to be sent
 * @param espnow_active bool: esp-now active flag
 */
void ERROR(const char *, bool);

/**
 * @brief print INFO messages on serial and through esp-now:
 *
 * @param returnMsg const *char: message to be sent
 */
void INFO(const char *);

/**
 * @brief print INFO messages on serial and through esp-now:
 *
 * @param returnMsg const *char: message to be sent
 * @param espnow_active bool: esp-now active flag
 */
void INFO(const char *, bool);

/**
 * @brief send debug messages through esp-now
 *
 * @param returnMsg const *char: message to be sent
 */
void send_debug(const char *);

/**
 * @brief send error messages through esp-now with option
 *
 * @param returnMsg const *char: message to be sent
 * @param espnow_active bool: esp-now active flag
 */
void on_error(const char *, bool);

void init_system(void) {
    // -----Serial initiallization-----
    Serial.begin(SERIAL_FREQ);
    while (!Serial) {
    }; // wait until is initialized
}

void init_espnow(void) {
    // -----ESPNOW settings-----
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_now_init() != ESP_OK)             // check if was initialized successfully
        ERROR("initializing ESP-NOW", false); // DO NOT send through esp-now
    INFO("Done initializing ESP-NOW", false); // DO NOT send through esp-now
    delay(50);                                // give time to send the espnow message

    // register peer
    esp_now_peer_info_t peerInfo = {};
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // ECU BOX peer
    memcpy(peerInfo.peer_addr, address_ECU_BOX, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("Failed to add ECU BOX  peer", false); // DO NOT send through esp-now
    INFO("ECU BOX  peer added", false);
    delay(50); // give time to send the espnow message

    // ECU Front peer
    memcpy(peerInfo.peer_addr, address_ECU_Front, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("Failed to add ECU Front peer", false); // DO NOT send through esp-now
    INFO("ECU Front peer added", false);
    delay(50); // give time to send the espnow message

    // ECU Rear peer
    memcpy(peerInfo.peer_addr, address_ECU_Rear, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("Failed to add ECU Rear peer", false); // DO NOT send through esp-now
    INFO("ECU Rear peer added", false);
    delay(50); // give time to send the espnow message

    // module 1 peer
    memcpy(peerInfo.peer_addr, address_module_1, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("Failed to add module 1 peer", false); // DO NOT send through esp-now
    INFO("module 1 peer added", false);
    delay(50); // give time to send the espnow message

    // module 2 peer
    memcpy(peerInfo.peer_addr, address_module_2, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("Failed to add module 2 peer", false); // DO NOT send through esp-now
    INFO("module 2 peer added", false);
    delay(50); // give time to send the espnow message

    // module 3 peer
    memcpy(peerInfo.peer_addr, address_module_3, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("Failed to add module 3 peer", false); // DO NOT send through esp-now
    INFO("module 3 peer added", false);
    delay(50); // give time to send the espnow message
}

void ERROR(const char returnMsg[]) {
    log_e("%s", returnMsg);
    send_debug(returnMsg);

    on_error(returnMsg, true);
}

void ERROR(const char returnMsg[], bool espnow_active) {
    log_e("%s", returnMsg);
    if (espnow_active)
        send_debug(returnMsg);

    on_error(returnMsg, espnow_active);
}

void INFO(const char returnMsg[]) {
    log_i("%s", returnMsg);
    send_debug(returnMsg);
}

void INFO(const char returnMsg[], bool espnow_active) {
    log_i("%s", returnMsg);
    if (espnow_active)
        send_debug(returnMsg);
}

void send_debug(const char returnMsg[]) {
    if (strlen(returnMsg) <= ESPNOW_BUFFER_SIZE) {
        // mudar para char*
        debug_t msg_data;
        strcpy(msg_data.msg, returnMsg);
        esp_now_send(address_ECU_BOX, (uint8_t *)&msg_data, sizeof(msg_data));
    } else
        log_e("espnow buffer overflow");
}

void on_error(const char returnMsg[], bool espnow_active) {
    if (REBOOT_ON_ERROR) {
        ESP.restart();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    } else {
        while (true) {
            log_e("%s", returnMsg);

            if (espnow_active)
                send_debug(returnMsg);

            vTaskDelay(DEBUG_DELAY / portTICK_PERIOD_MS);
        }
    }
}

bool comp_array(const uint8_t arr1[], const uint8_t arr2[]) { // Function for compare vectors

    for (uint8_t i = 0; i < 6; i++) {
        if (arr1[i] != arr2[i]) {
            return false;
        }
    }
    
    return true;
}

#endif // __SYSTEM_H__