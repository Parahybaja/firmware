/**
 * @file main.cpp
 * @authors
 *      Jefferson Lopes (jefferson.lopes@ee.ufcg.edu.br)
 *      Vitor Lucas
 *      Raynoan Émilly (raynoan.emilly@estudante.ufcg.edu.br)
 * @brief start point of the validation AV
 * @version 1.0
 * @date 2023-07-18
 *
 * @copyright Copyright (c) 2023
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Jefferson L.  25/04/2023  first version
 *  1.0.0    Raynoan E.    15/07/2023  version esp32
 *  2.0.0    Raynoan E.    18/07/2023  afv extern module 
 */

#ifndef __MODULE_AFV_H__
#define __MODULE_AFV_H__ 

#include <WiFi.h>
#include <esp_now.h>

#define DEFAULT_DIST 50 // cm 

typedef enum {
    AFV_START,  /*empty command*/
    AFV_FINISH, /*start command*/
    AFV_SPEED,  /*stop command*/
} afv_mode_t;

typedef struct setup_t {
    float distance = DEFAULT_DIST;
    bool active = false;
} setup_t;

typedef struct board_t {
    uint8_t id = BOARD_ID;
    afv_mode_t mode = afv_mode_t(BOARD_ID - 1);
    float speed;
} board_t;

uint8_t address_ECU_BOX[]   = {0x94, 0xB5, 0x55, 0x2D, 0x1E, 0x0C};
uint8_t address_module_1[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8D, 0x38};
uint8_t address_module_2[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8A, 0xD8};
uint8_t address_module_3[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8B, 0xA0};

// prototypes
void init_espnow(void); // initializing esp now protocol

void init_espnow(void) {
    // -----ESPNOW settings-----
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_now_init() != ESP_OK) // check if was initialized successfully
        log_e("initializing ESP-NOW");
    else
        log_i("Done initializing ESP-NOW");

    // register peer
    esp_now_peer_info_t peerInfo = {};
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // ECU BOX peer
    memcpy(peerInfo.peer_addr, address_ECU_BOX, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        log_e("Failed to add ECU BOX  peer");
    else
        log_i("ECU BOX  peer added");

    // module 1 peer
    memcpy(peerInfo.peer_addr, address_module_1, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        log_e("Failed to add module 1 peer");
    else
        log_i("module 1 peer added");

    // module 2 peer
    memcpy(peerInfo.peer_addr, address_module_2, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        log_e("Failed to add module 2 peer");
    else
        log_i("module 2 peer added");

    // module 3 peer
    memcpy(peerInfo.peer_addr, address_module_3, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        log_e("Failed to add module 3 peer");
    else
        log_i("module 3 peer added");
}

#endif // __MODULE_AFV_H__
