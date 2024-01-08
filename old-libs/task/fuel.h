/**
 * @file task_fuel.h
 * @author Jefferson Lopes (jefferson.lopes@gmail.com)
 * @brief fuel emergency task
 * @version 0.3
 * @date 2023-02-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __FUEL_H__
#define __FUEL_H__

#include "system.h"

#define TASK_FUEL_SEND_RATE_Hz 0.7 // RPM task send rate in hertz
#define TASK_FUEL_SEND_RATE_ms (int(1000.0 / float(TASK_FUEL_SEND_RATE_Hz))) // rate period in milliseconds

#define ACTIVE_LOW false
#define FUEL_AVERAGE_POINTS 5
#define THRESHOLD 0.5 // to be calibrated

// ----- pinout -----
extern const uint8_t pin_fuel;

/**
 * @brief fuel task
 * 
 * @param arg void arg
 */
void task_fuel(void*);

void task_fuel(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_send;
    float sum;
    bool last_value = false;
    sensor_t fuel = {FUEL_EMERGENCY, 0.0};

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "%d bytes remaining of the fuel task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----update timer-----
    timer_send = millis();

    while (true){
        if ((millis() - timer_send) >= TASK_FUEL_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_FUEL_SEND_RATE_ms;

            // -----calculate-----
            sum = 0; // clean sum buffer
            for (int i=0; i < FUEL_AVERAGE_POINTS; i++) { 
                if (ACTIVE_LOW)
                    sum += !digitalRead(pin_fuel);
                else
                    sum += digitalRead(pin_fuel);
            }
            fuel.value = sum / float(FUEL_AVERAGE_POINTS);

            // ----- define high or low level -----
            // comment out to send raw data fuel average
            if (fuel.value <= THRESHOLD) // low fuel level
                fuel.value = true; // active low fuel emergency flag
            else 
                fuel.value = false; 

            // ----- send data just when is changed -----
            if (fuel.value != last_value) {
                // -----send fuel data through esp-now to receiver-----
                log_d("send fuel");
                esp_now_send(address_ECU_Front, (uint8_t *) &fuel, sizeof(fuel));
            }

            // ----- update last value -----
            last_value = fuel.value;
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

#endif // __FUEL_H__