/**
 * @file hall_check.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief check and print hall raw data
 * @version 0.1
 * @date 2022-10-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// -----FreeRTOS includes-----
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// -----pinout-----
#define PIN_LED_ALIVE 02
#define PIN_HALL_1    14
#define PIN_HALL_2    27
#define PIN_HALL_3    26
#define PIN_HALL_4    25
#define PIN_HALL_5    33
#define PIN_HALL_6    32

// -----tasks config-----
#define TASK_HALL_SEND_RATE_Hz  1           // task send rate in hertz
#define TASK_HALL_SEND_RATE_ms  (int( 1000.0 / float(TASK_HALL_SEND_RATE_Hz))) // rate perido in milliseconds
#define TASK_HALL_SEND_RATE_s   (float (1.0 / float(TASK_HALL_SEND_RATE_Hz))) // rate perido in seconds
#define TASK_HALL_SEND_RATE_min (float(1.0 / (float(TASK_HALL_SEND_RATE_Hz) * 60.0))) // rate perido in min

// -----FreeRTOS objects-----
TaskHandle_t th_hall_check;
TaskHandle_t th_alive;

/**
 * @brief Read and print hall raw data
 * 
 * @param arg void arg
 */
void task_hall_check(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_send;
    bool hall_read[6];

    // -----pin mode-----
    pinMode(PIN_HALL_1, INPUT_PULLUP);
    pinMode(PIN_HALL_2, INPUT_PULLUP);
    pinMode(PIN_HALL_3, INPUT_PULLUP);
    pinMode(PIN_HALL_4, INPUT_PULLUP);
    pinMode(PIN_HALL_5, INPUT_PULLUP);
    pinMode(PIN_HALL_6, INPUT_PULLUP);

    // -----update timer-----
    timer_send = millis();

    while (true){
        if ((millis() - timer_send) >= TASK_HALL_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_HALL_SEND_RATE_ms;

            // -----read raw data-----
            hall_read[0] = !digitalRead(PIN_HALL_1);
            hall_read[1] = !digitalRead(PIN_HALL_2);
            hall_read[2] = !digitalRead(PIN_HALL_3);
            hall_read[3] = !digitalRead(PIN_HALL_4);
            hall_read[4] = !digitalRead(PIN_HALL_5);
            hall_read[5] = !digitalRead(PIN_HALL_6);

            // -----print value-----
            Serial.print(  "H1:"); Serial.print(hall_read[0] ? " true" : "false");
            Serial.print(", H2:"); Serial.print(hall_read[1] ? " true" : "false");
            Serial.print(", H3:"); Serial.print(hall_read[2] ? " true" : "false");
            Serial.print(", H4:"); Serial.print(hall_read[3] ? " true" : "false");
            Serial.print(", H5:"); Serial.print(hall_read[4] ? " true" : "false");
            Serial.print(", H6:"); Serial.print(hall_read[5] ? " true" : "false");
            Serial.println();
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

    xTaskCreate(task_hall_check,   // task function
                "task_hall_check", // task name
                2048,              // stack size
                NULL,              // parameters
                10,                // priority
                &th_hall_check);   // handler  
}

void loop(){
    // -----delete loop-----
    vTaskDelete(NULL);
}