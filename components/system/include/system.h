/**
 * @file system.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief system's general types definitions and basic esp-now communication functions
 * @version 2.0
 * @date 2024-01-08
 *
 * @copyright Copyright (c) 2023
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Jefferson L.  10/02/2023  first version
 *  1.0.0    Jefferson L.  18/07/2023  move common_libs and rename a few modules
 *  1.1.0    Jefferson L.  26/10/2023  remove Wire begin
 *  2.0.0    Jefferson L.  08/01/2024  esp-idf convertion
 * 
 */

#pragma once

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_now.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESPNOW_CHANNEL 1
#define MAC_SIZE 6

// -----type definitions-----
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
    float fuel_em;
    float battery;
    float temp;
    float rollover;
    float tilt_x;
    float tilt_y;
    float tilt_z;
} system_t;

// -----sensor data type definition-----
typedef struct {
    sensor_type_t type;
    float value;
} sensor_t;

// -----system run time data-----
extern system_t system_global;

// -----FreeRTOS objects-----
extern TaskHandle_t th_example;
extern TaskHandle_t th_alive;

// -----esp-now mac addresses-----
extern const uint8_t mac_address_ECU_box[MAC_SIZE];
extern const uint8_t mac_address_ECU_front[MAC_SIZE];
extern const uint8_t mac_address_ECU_rear[MAC_SIZE];
extern const uint8_t mac_address_module_1[MAC_SIZE];
extern const uint8_t mac_address_module_2[MAC_SIZE];
extern const uint8_t mac_address_module_3[MAC_SIZE];

void task_remaining_space(void);

void print_mac_address(void);

void init_espnow(void);

#ifdef __cplusplus
}
#endif
