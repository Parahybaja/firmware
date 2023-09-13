/**
 * @file main.cpp
 * @authors
 *      Jefferson Lopes (jefferson.lopes@ee.ufcg.edu.br)
 *      Vitor Lucas
 *      Raynoan Émilly (raynoan.emilly@estudante.ufcg.edu.br)
 * @brief speed mesurement point of the validation AV
 * @version 2.0
 * @date 2023-07-18
 *
 * @copyright Copyright (c) 2023
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Jefferson L.  25/04/2023  first version
 *  1.0.0    Raynoan E.    15/07/2023  version esp32
 *  2.0.0    Raynoan E.    15/07/2023  add compatibility to afv extern module 
 *  2.0.1    Jefferson L.  26/07/2023  fix speed measurement
 */
#include <Arduino.h>

#include"system.h"
#include "module/AV.h"
#include "task/battery.h"

#define DEBUG false

#define PIN_SENSOR1  32
#define PIN_SENSOR2  33
#define MS2KMH       3.6
#define SIGNAL_DELAY 1000

const uint8_t pin_battery = 36;
const uint8_t board_id = 3;

setup_t setup_board;  
volatile board_t board; // use volatile to all variables used inside Interrupt Service Routine
volatile uint32_t last_initial = false;
volatile uint32_t last_final   = false;

// prototypes
void OnDataSent(const uint8_t *, esp_now_send_status_t); // Datasent callback
void OnDataRecv(const uint8_t *, const uint8_t *, int);  // Datarecev callback
void IRAM_ATTR ISR_send_initial(void);
void IRAM_ATTR ISR_send_final(void);

void setup() {
    pinMode(PIN_SENSOR1, INPUT_PULLUP);
    pinMode(PIN_SENSOR2, INPUT_PULLUP);

    init_espnow();

#if DEBUG
    log_i("Board id: %u", board.id);  // board id 
    log_i("Set up activation: %s", setup_board.active ? "On" : "Off"); // Showing board status
#endif

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);

    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);

    // battery_config_t battery_config ;
    // battery_config.R1 = 10000;
    // battery_config.R2 = 20000;
    // memcpy(battery_config.mac, address_ECU_BOX, sizeof(address_ECU_BOX));

    // xTaskCreatePinnedToCore(
    //     task_battery,      // task function
    //     "battery voltage", // task name
    //     4096,              // stack size
    //     &battery_config,   // parameters
    //     10,                // priority
    //     &th_battery,       // handler 
    //     APP_CPU_NUM        // core number
    // );
}

void loop() {
    vTaskDelete(NULL);
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

        if (setup_board.active) {
            attachInterrupt(PIN_SENSOR1, ISR_send_initial, FALLING);
        } else {
            detachInterrupt(PIN_SENSOR1);
            detachInterrupt(PIN_SENSOR2);
        }
    } else
        log_e("Incoming data does not match");
}

void IRAM_ATTR ISR_send_initial() {
    if (millis() - last_initial > SIGNAL_DELAY) {
        detachInterrupt(PIN_SENSOR1);
        
        initial_time = millis();
        
        attachInterrupt(PIN_SENSOR2, ISR_send_final, FALLING);
    }

    last_initial = millis();
}

void IRAM_ATTR ISR_send_final() {
    if (millis() - last_final > SIGNAL_DELAY) {
        detachInterrupt(PIN_SENSOR2);

        final_time = millis();

        // distance_m = distance(cm) / 100
        // delta_time_s = (final_time(ms) - initial_time(ms)) / 1000
        // speed = distance / delta_time 
        board.speed = ((setup_board.distance / 100.0) / ((final_time - initial_time) / 1000.0)) * MS2KMH;

        esp_now_send(address_ECU_BOX, (uint8_t *)&board, sizeof(board_t));

    #if DEBUG
        log_d("velor %.2f kmh", board.speed );
        log_d("tempo %.2f s",(final_time - initial_time) / 1000.0);
    #endif

        initial_time = false;
        final_time = false;

        attachInterrupt(PIN_SENSOR1, ISR_send_initial, FALLING);
    }

    last_final = millis();
}
