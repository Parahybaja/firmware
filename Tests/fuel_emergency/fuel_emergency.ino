/**
 * @file fuel_emergency.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief read the emergency fuel sensor (capacitive)
 * @version 0.1
 * @date 2022-10-31
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// -----FreeRTOS includes-----
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// -----pinout-----
#define PIN_LED_ALIVE 02
#define PIN_FUEL_EMER 16

// -----tasks config-----
#define TASK_F_EMER_SEND_RATE_Hz  1           // task send rate in hertz
#define TASK_F_EMER_SEND_RATE_ms  (int( 1000.0 / float(TASK_F_EMER_SEND_RATE_Hz))) // rate perido in milliseconds
#define TASK_F_EMER_SEND_RATE_s   (float (1.0 / float(TASK_F_EMER_SEND_RATE_Hz))) // rate perido in seconds
#define TASK_F_EMER_SEND_RATE_min (float(1.0 / (float(TASK_F_EMER_SEND_RATE_Hz) * 60.0))) // rate perido in min

// -----FreeRTOS objects-----
TaskHandle_t th_fuel_emer;
TaskHandle_t th_alive;

/**
 * @brief Read and print fuel emergency sensor data
 * 
 * @param arg void arg
 */
void task_fuel_emer(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_send;
    bool fuel_emer;

    // -----pin mode-----
    pinMode(PIN_FUEL_EMER, INPUT_PULLDOWN); // use pulldown to test without the sensor

    // -----update timer-----
    timer_send = millis();

    while (true){
        if ((millis() - timer_send) >= TASK_F_EMER_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_F_EMER_SEND_RATE_ms;

            // -----read raw data-----
            fuel_emer = digitalRead(PIN_FUEL_EMER);

            // -----print value-----
            Serial.print("TASK - Fuel Emergency: ");
            Serial.println(fuel_emer ? "true" : "false");
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief alive signal
 * 
 * @param arg void arg
 */
void task_alive(void *arg){
    (void)arg;

    pinMode(PIN_LED_ALIVE, OUTPUT); 
    
    while (true){
        digitalWrite(PIN_LED_ALIVE, HIGH);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        digitalWrite(PIN_LED_ALIVE, LOW);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup(){
    Serial.begin(115200);
    
    // -----fire up tasks-----
    xTaskCreate(task_alive,   // task function
                "task_alive", // task name
                2048,         // stack size
                NULL,         // parameters
                10,           // priority
                &th_alive);   // handler 

    xTaskCreate(task_fuel_emer,   // task function
                "task_fuel_emer", // task name
                2048,             // stack size
                NULL,             // parameters
                10,               // priority
                &th_fuel_emer);   // handler  
}

void loop(){
    // -----delete loop-----
    vTaskDelete(NULL);
}