/**
 * @file RPM.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief RPM sensor test (inductive)
 * @version 0.1
 * @date 2022-10-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// -----FreeRTOS includes-----
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#define PIN_LED_ALIVE          02
#define PIN_RPM                04
#define TASK_RPM_SEND_RATE_Hz  1           // task 1 send rate in hertz
#define TASK_RPM_SEND_RATE_ms  (int(1000.0 / float(TASK_RPM_SEND_RATE_Hz))) // rate perido in milliseconds
#define TASK_RPM_SEND_RATE_min (float(1.0 / (float(TASK_RPM_SEND_RATE_Hz) * 60.0))) // rate perido in min

TaskHandle_t th_rpm;
TaskHandle_t th_alive;

volatile uint16_t rpm_counter;

void IRAM_ATTR isr_rpm(){ rpm_counter++; }

void task_RPM(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_send;
    uint16_t buffer_count = 0;
    uint16_t rpm_counter_copy;
    float rpm;

    // -----attachISR-----
    pinMode(PIN_RPM, INPUT);
    attachInterrupt(PIN_RPM, isr_rpm, RISING);

    // -----update timer-----
    timer_send = millis();

    while (true){
        if ((millis() - timer_send) >= TASK_RPM_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_RPM_SEND_RATE_ms;

            // -----get counts-----
            noInterrupts();
            rpm_counter_copy = rpm_counter;
            rpm_counter = false;
            interrupts();

            // -----calculate-----
            rpm = rpm_counter_copy / TASK_RPM_SEND_RATE_min; // general rpm calculation
            rpm /= 2; // compensate for the double peak of the signal

            // -----print value-----
            Serial.printf("RPM: %.2f\n", rpm);
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void task_alive(void *arg){
    (void)arg;

    pinMode(PIN_LED_ALIVE, OUTPUT); 
    
    while (true){
        // alive signal
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

    xTaskCreate(task_RPM,   // task function
                "tasl_RPM", // task name
                2048,       // stack size
                NULL,       // parameters
                10,         // priority
                &th_rpm);   // handler  
}

void loop(){
    
}