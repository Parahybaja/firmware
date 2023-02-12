/**
 * @file task_RPM.h
 * @author Jefferson Lopes (jefferson.lopes@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __RPM_H__
#define __RPM_H__

#include "communication.h"

#define PIN_RPM 04

volatile uint16_t counter_rpm;

/**
 * @brief RPM interrupt service routine
 * 
 */
void IRAM_ATTR isr_rpm(void);

/**
 * @brief RPM task
 * 
 * @param arg void arg
 */
void task_RPM(void*);

void IRAM_ATTR isr_rpm(void){ counter_rpm++; }

void task_RPM(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_send;
    uint16_t counter_rpm_copy;
    sensor_t rpm = {RPM, 0.0};

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_2: %d bytes remaining of the RPM task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // attach interupt
    pinMode(PIN_RPM, INPUT);
    attachInterrupt(PIN_RPM, isr_rpm, HIGH);

    // -----update timer-----
    timer_send = millis();

    while (true){
        if ((millis() - timer_send) >= TASK_RPM_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_RPM_SEND_RATE_ms;

            // -----get counts-----
            noInterrupts();
            counter_rpm_copy = counter_rpm;
            counter_rpm = false;
            interrupts();

            // -----calculate-----
            rpm.value = counter_rpm_copy / TASK_RPM_SEND_RATE_min; // general rpm calculation
            rpm.value /= 2; // compensate for the double peak of the signal

            // -----send RPM data through esp-now to receiver-----
            esp_now_send(address_ECU_Front, (uint8_t *) &rpm, sizeof(rpm));
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

#endif // __RPM_H__