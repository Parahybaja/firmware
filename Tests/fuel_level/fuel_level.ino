/**
 * @file fuel_level.ino
 * @author jefferson lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief fuel level sensor test (hall sensors)
 * @note not ready
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
#define PIN_HALL_1    26
#define PIN_HALL_2    25
#define PIN_HALL_3    33
#define PIN_HALL_4    32
#define PIN_HALL_5    35
#define PIN_HALL_6    34

// -----tasks config-----
#define TASK_LVL_SEND_RATE_Hz    0.2 // task send rate in hertz
#define TASK_LVL_READING_RATE_Hz 1   // task reading rate in hertz

// -----convertions-----
#define TASK_LVL_SEND_RATE_ms  (int( 1000.0 / float(TASK_LVL_SEND_RATE_Hz))) // rate perido in milliseconds
#define TASK_LVL_SEND_RATE_s   (float (1.0 / float(TASK_LVL_SEND_RATE_Hz))) // rate perido in seconds
#define TASK_LVL_SEND_RATE_min (float(1.0 / (float(TASK_LVL_SEND_RATE_Hz) * 60.0))) // rate perido in min
#define TASK_LVL_READING_RATE_ms  (int( 1000.0 / float(TASK_LVL_READING_RATE_Hz))) // rate perido in milliseconds
#define TASK_LVL_READING_RATE_s   (float (1.0 / float(TASK_LVL_READING_RATE_Hz))) // rate perido in seconds
#define TASK_LVL_READING_RATE_min (float(1.0 / (float(TASK_LVL_READING_RATE_Hz) * 60.0))) // rate perido in min

// -----FreeRTOS objects-----
TaskHandle_t th_fuel_level;
TaskHandle_t th_alive;

/**
 * @brief Read, calculate and print the speed
 * 
 * @param arg void arg
 */
void task_fuel_level(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_read, timer_send;
    bool hall_read[6];
    uint16_t buffer_count;
    float sum_level;

    // -----pin mode-----
    pinMode(PIN_HALL_1, INPUT);
    pinMode(PIN_HALL_2, INPUT);
    pinMode(PIN_HALL_3, INPUT);
    pinMode(PIN_HALL_4, INPUT);
    pinMode(PIN_HALL_5, INPUT);
    pinMode(PIN_HALL_6, INPUT);

    // -----update timer-----
    timer_read = timer_send = millis();

    while (true){
        if ((millis() - timer_read) >= TASK_LVL_READING_RATE_ms){
            // -----add to timer-----
            timer_read += TASK_LVL_READING_RATE_ms;

            // -----read sensors-----
            hall_read[0] = digitalRead(PIN_HALL_1);
            hall_read[1] = digitalRead(PIN_HALL_2);
            hall_read[2] = digitalRead(PIN_HALL_3);
            hall_read[3] = digitalRead(PIN_HALL_4);
            hall_read[4] = digitalRead(PIN_HALL_5);
            hall_read[5] = digitalRead(PIN_HALL_6);

            // -----calculate fuel level-----
            sum_level = 

            // -----update buffer count-----
            buffer_count++;
        }

        if ((millis() - timer_send) >= TASK_LVL_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_LVL_SEND_RATE_ms;

            // -----calculate average-----
            sensor2.value = float(sum / buffer_count);

            // -----clear buffer-----
            buffer_count = false;

            // -----send sensor 2 data through esp-now to receiver-----
            esp_now_send(address_receiver, (uint8_t *) &sensor2, sizeof(sensor2));
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