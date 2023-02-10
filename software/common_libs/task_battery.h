#ifndef __BATTERY_H__
#define __BATTERY_H__

#include "communication.h"

#define TASK_BATTERY_RATE_Hz  0.1     // reading rate in hertz (must be greater then send rate)
#define TASK_BATTERY_RATE_ms  (int(1000.0 / float(TASK_BATTERY_RATE_Hz))) // rate perido in milliseconds

// -----battery configs-----
#define R1 10375.0
#define R2 2085.0
#define BAT_MAX 13.0
#define BAT_MIN 11.8

void task_battery(void*);

/**
 * @brief battery level task example
 * 
 * @param arg void arg
 */
void task_battery(void *arg){
    (void)arg;

    pinMode(PIN_BATTERY, INPUT);

    // -----create local variables-----
    uint32_t timer_send;
    uint16_t adc_read;
    const float bat_max = BAT_MAX;
    const float bat_min = BAT_MIN;
    float voltage_read, voltage_bat;
    sensor_t bat = {
        .type = BATTERY, 
        .value = 0.0
    };

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_1: %d bytes remaining of the battery task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----update timer-----
    timer_send = millis();

    while (true){
        if ((millis() - timer_send) >= TASK_BATTERY_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_BATTERY_RATE_ms;

            // -----calculate average-----
            adc_read = analogRead(PIN_BATTERY);
            voltage_read = (3.3 * adc_read) / 4095.0;
            voltage_bat = voltage_read / (R2 / (R1 + R2));
            bat.value = voltage_bat;

            // -----convert to %-----
            // if (bat.value > bat_max){
            //     bat.value = 100.0;
            // }
            // else if (bat.value < bat_min) {
            //     bat.value = 0.0;
            // }
            // else {
            //     bat.value = ((bat.value - bat_min) / (bat_max - bat_min)) * 100.0; 
            // }
            
            // -----send bat data through queue-----
            xQueueSend(qh_battery, &bat, pdMS_TO_TICKS(0));
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

#endif // __BATTERY_H__