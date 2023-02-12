/**
 * @file task_speed.h
 * @author Jefferson Lopes (jefferson.lopes@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __SPEEDOMETER_H__
#define __SPEEDOMETER_H__

#include "communication.h"

// ----- pinout -----
#define PIN_SPEEDOMETER 17

// ----- constants -----
#define PI          3.1415
#define ms2kmh      3.6
#define WHEEL_DIA   0.4              // diameter in meters
#define WHEEL_CIRC  (PI * WHEEL_DIA) // wheel circumference
#define WHEEL_EDGES 6                // 6 edges per revolution

volatile uint16_t counter_spdmt;

/**
 * @brief speedometer interrupt service routine
 * 
 */
void IRAM_ATTR isr_spdmt(void);

/**
 * @brief  Read, calculate and send the speed
 * 
 * @param arg void arg
 */
void task_speedometer(void*);

void IRAM_ATTR isr_spdmt(void){ counter_spdmt++; }

void task_speedometer(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_send;
    uint16_t counter_spdmt_copy;
    sensor_t spdmt = {SPEEDOMETER, 0.0};

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_2: %d bytes remaining of the speedometer task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

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
            spdmt.value = meters / TASK_SPDMT_SEND_RATE_s; // in meter/second
            spdmt.value *= ms2kmh;          

            // -----send spped data through esp-now to receiver-----
            esp_now_send(address_ECU_Front, (uint8_t *) &spdmt, sizeof(spdmt));
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

#endif // __SPEEDOMETER_H__