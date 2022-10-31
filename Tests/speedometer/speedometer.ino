/**
 * @file speedometer.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief Speedometer sensor test (inductive sensor)
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
#define PIN_LED_ALIVE   2
#define PIN_SPEEDOMETER 4

// -----tasks config-----
#define TASK_SPDMT_SEND_RATE_Hz  1           // task send rate in hertz
#define TASK_SPDMT_SEND_RATE_ms  (int( 1000.0 / float(TASK_SPDMT_SEND_RATE_Hz))) // rate perido in milliseconds
#define TASK_SPDMT_SEND_RATE_s   (float (1.0 / float(TASK_SPDMT_SEND_RATE_Hz))) // rate perido in seconds
#define TASK_SPDMT_SEND_RATE_min (float(1.0 / (float(TASK_SPDMT_SEND_RATE_Hz) * 60.0))) // rate perido in min

// -----general configs-----
#define PI          3.1415
#define ms2kmh      3.6
#define WHEEL_DIA   0.4              // meters
#define WHEEL_CIRC  (PI * WHEEL_DIA) // wheel circumference
#define WHEEL_EDGES 6                // 6 edges per revolution

// -----FreeRTOS objects-----
TaskHandle_t th_spdmt;
TaskHandle_t th_alive;

// -----global counter variable-----
volatile uint16_t counter_spdmt;

/**
 * @brief speedometer interrupt service routine
 * 
 */
void IRAM_ATTR isr_spdmt(void){ counter_spdmt++; }

/**
 * @brief Read, calculate and print the speed
 * 
 * @param arg void arg
 */
void task_speedometer(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_send;
    uint16_t counter_spdmt_copy;
    float speed;

    // -----attachISR-----
    pinMode(PIN_SPEEDOMETER, INPUT);
    attachInterrupt(PIN_SPEEDOMETER, isr_spdmt, CHANGE);

    // -----update timer-----
    timer_send = millis();

    while (true){
        if ((millis() - timer_send) >= TASK_SPDMT_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_SPDMT_SEND_RATE_ms;

            // -----get counts-----
            noInterrupts();
            counter_spdmt_copy = counter_spdmt;
            counter_spdmt = false;
            interrupts();

            // -----calculate-----
            float meters = (counter_spdmt_copy / WHEEL_EDGES) * WHEEL_CIRC;
            speed = meters / TASK_SPDMT_SEND_RATE_s; // in meter/second
            speed *= ms2kmh;

            // -----print value-----
            Serial.printf("speed: %.2f km/h\n", speed);
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

    xTaskCreate(task_speedometer,   // task function
                "task_speedometer", // task name
                2048,               // stack size
                NULL,               // parameters
                10,                 // priority
                &th_spdmt);         // handler  
}

void loop(){
    // -----delete loop-----
    vTaskDelete(NULL);
}