/**
 * @file main.cpp
 * @brief ECU Box main
 * @version 4.0
 * @date 2023-08-02
 * 
 * @authors
 *      Jefferson Lopes (jefferson.lopes@ee.ufcg.edu.br)
 * 
 * @copyright Copyright (c) 2023
 * 
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  3.1.0    Jefferson L.  19/02/2023  last version
 *  4.0.0    Jefferson L.  02/08/2023  add AFV task running in the main
 */

#include <Arduino.h>

#include "system.h"
#include "espnow_callback.h"
#include "task/alive.h"
#include "task/battery.h"
#include "module/AV.h"

// ----- pinout -----
const uint8_t pin_alive_LED = 33;

String msg;

void cmd_server(void);
void activation(bool);
void set_calib(uint8_t);
void send_setup(setup_t);

void setup(){
    // init system general modules (pinMode and Serial)
    init_system();

    // -----header-----
    log_i("embedded system: ECU BOX");

    pinMode(pin_alive_LED, OUTPUT);

    // init system handlers such as queues and semaphores
    init_espnow();

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    // -----final confirmation-----
    INFO("it's all configured!");

    xTaskCreatePinnedToCore(
        task_alive_LED, // task function
        "alive LED",    // task name
        2048,           // stack size
        NULL,           // parameters
        5,              // priority
        &th_alive,      // handler
        APP_CPU_NUM     // core number
    );   
}

void loop(){
    cmd_server();
}

void cmd_server(void) {
    while (Serial.available()) msg += Serial.readString();

    if (msg == "rst\n") {
        // clean all variables
        msg = "";

        reset_time_counter();

        snprintf(str_buffer, sizeof(str_buffer), "AFV:----- Rebooting -----%c", CMD_NEWLINE); // format string
        Serial.println(str_buffer); // send string to the server
        memset(str_buffer, 0, sizeof(str_buffer)); // reset buffer

        // TODO: send restart signal
    } 
    else if (msg == "dtl\n") {
        // clean all variables
        msg = "";

        reset_time_counter();

        snprintf(str_buffer, sizeof(str_buffer), "AFV:----- Deleted last one -----%c", CMD_NEWLINE); // format string
        Serial.println(str_buffer); // send string to the server
        memset(str_buffer, 0, sizeof(str_buffer)); // reset buffer
    } 
    else if (msg == "on\n") {
        msg = "";

        activation(true);

        reset_time_counter();

        snprintf(str_buffer, sizeof(str_buffer), "AFV:----- Turned on -----%c", CMD_NEWLINE); // format string
        Serial.println(str_buffer); // send string to the server
        memset(str_buffer, 0, sizeof(str_buffer)); // reset buffer
    } 
    else if (msg == "off\n") {
        msg = "";

        activation(false);

        reset_time_counter();

        snprintf(str_buffer, sizeof(str_buffer), "AFV:----- Turned off -----%c", CMD_NEWLINE); // format string
        Serial.println(str_buffer); // send string to the server
        memset(str_buffer, 0, sizeof(str_buffer)); // reset buffer
    } 
    else if (msg.indexOf("set ") != -1) { // search for 'set ' in the string
        String str = "";

        str.concat(msg[4]);
        str.concat(msg[5]);

        long number = str.toInt();

#if DEBUG
        Serial.println("Message received: ");
        Serial.print(msg);
        Serial.print("Message[4]: ");
        Serial.println(msg[4]);
        Serial.print("Message[5]: ");
        Serial.println(msg[5]);
        Serial.print("String number: ");
        Serial.println(str);
        Serial.print("Int number: ");
        Serial.println(number);
#endif

        if (number) {
            set_calib(number);

            snprintf(str_buffer, sizeof(str_buffer), "AFV:----- Set to: %i cm -----%c", number, CMD_NEWLINE); // format string
            Serial.println(str_buffer); // send string to the server
            memset(str_buffer, 0, sizeof(str_buffer)); // reset buffer
        }
        else {
            snprintf(str_buffer, sizeof(str_buffer), "AFV:ERROR:invalid input command%c", CMD_NEWLINE); // format string
            Serial.println(str_buffer); // send string to the server
            memset(str_buffer, 0, sizeof(str_buffer)); // reset buffer
        }

        msg = "";
    } else if (msg != "") {
        snprintf(str_buffer, sizeof(str_buffer), "AFV:ERROR:'%s' does not exist%c", msg, CMD_NEWLINE); // format string
        Serial.println(str_buffer); // send string to the server
        memset(str_buffer, 0, sizeof(str_buffer)); // reset buffer

        msg = "";
    }
}

void activation(bool act) {
    setup_t setup_boards;
    setup_boards.active = act;

    send_setup(setup_boards);
}

void set_calib(uint8_t calib) {
    setup_t setup_boards;
    setup_boards.active = true;
    setup_boards.distance = calib;

    send_setup(setup_boards);
}

void send_setup(setup_t setup_boards) {
    if (esp_now_send(address_module_1, (uint8_t *)&setup_boards, sizeof(setup_t)) != ESP_OK) {
        snprintf(str_buffer, sizeof(str_buffer), "AFV:ERROR on module 1%c", msg, CMD_NEWLINE); // format string
        Serial.println(str_buffer); // send string to the server
        memset(str_buffer, 0, sizeof(str_buffer)); // reset buffer
    }

    if (esp_now_send(address_module_2, (uint8_t *)&setup_boards, sizeof(setup_t)) != ESP_OK) {
        snprintf(str_buffer, sizeof(str_buffer), "AFV:ERROR on module 2%c", msg, CMD_NEWLINE); // format string
        Serial.println(str_buffer); // send string to the server
        memset(str_buffer, 0, sizeof(str_buffer)); // reset buffer
    }

    if (esp_now_send(address_module_3, (uint8_t *)&setup_boards, sizeof(setup_t)) != ESP_OK) {
        snprintf(str_buffer, sizeof(str_buffer), "AFV:ERROR on module 3%c", msg, CMD_NEWLINE); // format string
        Serial.println(str_buffer); // send string to the server
        memset(str_buffer, 0, sizeof(str_buffer)); // reset buffer
    }
}