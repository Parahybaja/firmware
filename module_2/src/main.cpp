/**
 * @file main.cpp
 * @authors
 *      Jefferson Lopes (jefferson.lopes@ee.ufcg.edu.br)
 *      Vitor Lucas
 *      Raynoan Émilly (raynoan.emilly@estudante.ufcg.edu.br)
 * @brief finish point of the validation AV
 * @version 2.0
 * @date 2023-07-26
 *
 * @copyright Copyright (c) 2023
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Jefferson L.  25/04/2023  first version
 *  1.0.0    Raynoan E.    15/07/2023  version esp32
 *  2.0.0    Raynoan E.    15/07/2023  add compatibility to afv extern module
 *  2.0.1    Jefferson L.  26/07/2023  add delay to avoid double detection
 */

#include <Arduino.h>

#include "system.h"
#include "module/AV.h"
#include "task/battery.h"

#define DEBUG true

#define PIN_SENSOR 32
#define SIGNAL_DELAY 5000

const uint8_t pin_battery = 3;
const uint8_t board_id = 2;

setup_t setup_board;
volatile board_t board; 
volatile uint32_t last_time = false;

// prototypes
void IRAM_ATTR ISR_send_signal();                            // function prototype
void OnDataSent(const uint8_t *, esp_now_send_status_t); // Datasent callback
void OnDataRecv(const uint8_t *, const uint8_t *, int);  // Datarecev callback

void setup() {
    pinMode(PIN_SENSOR, INPUT_PULLUP); // pin mode setting

    init_espnow();

#if DEBUG
    log_i("Board id: %i", board.id);                              // boar id
    log_i("Set up activation: %s", setup_board.active ? "On" : "Off"); // Showing board status
#endif

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);

    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);

    battery_config_t battery_config ;
    battery_config.R1 = 10000;
    battery_config.R2 = 20000;
     memcpy(battery_config.mac, address_ECU_BOX, sizeof(address_ECU_BOX));

    xTaskCreatePinnedToCore(
        task_battery,      // task function
        "battery voltage", // task name
        4096,              // stack size
        &battery_config,   // parameters
        10,                // priority
        &th_battery,       // handler 
        APP_CPU_NUM        // core number
    );
}

void loop() {
    vTaskDelete(NULL);
}

void IRAM_ATTR ISR_send_signal() {
    if (millis() - last_time > SIGNAL_DELAY)
        esp_now_send(address_ECU_BOX, (uint8_t *)& board, sizeof(board_t));
    
    last_time = millis();
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
#if DEBUG
    if (status == ESP_NOW_SEND_SUCCESS)
        log_i("Last Packet Send Status: Delivery success");
    else
        log_e("Last Packet Send Status: Delivery fail");
#endif
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    log_d("Endereço MAC: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    if (len == sizeof(setup_t)) {
        memcpy(&setup_board, incomingData, sizeof(setup_t));
#if DEBUG
        log_d("Set up received");
#endif
    } else
        log_e("Incoming data does not match");

    if (setup_board.active) {
        attachInterrupt(PIN_SENSOR, ISR_send_signal, FALLING); // initializing interrupt function
    } else {
        detachInterrupt(PIN_SENSOR);
    }
}