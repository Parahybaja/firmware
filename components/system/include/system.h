#pragma once

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_timer.h"
#include "lora.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESPNOW_CHANNEL 1
#define GPIO_4X4_INPUT     GPIO_NUM_15

#define TELEMETRY_KEY 0x98
#define TASK_TELEMETRY_SEND_RATE_Hz 1

typedef enum {
    RPM, SPEEDOMETER, FUEL_LEVEL, FUEL_EMERGENCY, BATTERY,
    HOURS, MINUTES, SECONDS, AMBIENT_TEMP, ROLLOVER,
    TILT_X, TILT_Y, TILT_Z, BLIND_SPOT_L, BLIND_SPOT_R, FOUR_X_FOUR
} sensor_type_t;

typedef struct {
    sensor_type_t type;
    float value;
} sensor_t;

typedef struct {
    float rpm; float speed; float fuel_level; float fuel_em; float battery;
    int hours; int minutes; int seconds; float temp; float rollover;
    float tilt_x; float tilt_y; float tilt_z;
    float blind_spot_l; float blind_spot_r; float fourxfour;
} system_t;

// Em system.h, substitua a struct antiga por esta:
typedef struct __attribute__((packed)) {
    uint8_t key;
    uint16_t rpm;            
    uint8_t speed;           
    uint8_t fuel_level;      
    uint8_t fuel_em;
    uint8_t battery;         
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    int8_t temp;             
    uint8_t rollover;        
    int16_t tilt_x;          // MUDOU PARA INT16_T (Suporta -180 a 180)
    int16_t tilt_y;          // MUDOU PARA INT16_T 
    int16_t tilt_z;          // MUDOU PARA INT16_T 
    uint8_t blind_spot_l;    
    uint8_t blind_spot_r;    
    uint8_t fourxfour;        
} simplified_system_t;

extern system_t system_global;
extern SemaphoreHandle_t sh_global_vars;
extern bool lora_initialized_flag;

// Task Handlers
extern TaskHandle_t th_lora, th_alive, th_display_nextion, th_rollover;

// MACs
extern const uint8_t mac_address_ECU_box[ESP_NOW_ETH_ALEN];
extern const uint8_t mac_address_ECU_front[ESP_NOW_ETH_ALEN];
extern const uint8_t mac_address_ECU_rear[ESP_NOW_ETH_ALEN];
extern const uint8_t mac_address_module_1[ESP_NOW_ETH_ALEN];
extern const uint8_t mac_address_module_2[ESP_NOW_ETH_ALEN];
extern const uint8_t mac_address_module_3[ESP_NOW_ETH_ALEN];

// Funções
void print_task_remaining_space(void);
void print_mac_address(void);
simplified_system_t system_to_simplified(const system_t*);
system_t simplified_to_system(const simplified_system_t*); 
void system_queue_init(void);
void system_espnow_init(void);
void system_lora_init(int, int, int);
void task_lora_sender(void*);
void task_lora_receiver(void*);

#ifdef __cplusplus
}
#endif