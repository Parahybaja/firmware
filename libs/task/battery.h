/**
 * @file task_battery.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief battery task, functions and configs
 * @version 1.3
 * @date 2023-10-26
 * 
 * @copyright Copyright (c) 2023
 * 
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  1.3.0    Jefferson L.  26/10/2023  add queue and espnow option
 */

#ifndef __BATTERY_H__
#define __BATTERY_H__

#include "system.h"

#define TASK_BATTERY_RATE_Hz 0.2 // reading rate in hertz (must be greater then send rate)
#define TASK_BATTERY_RATE_ms (int(1000.0 / float(TASK_BATTERY_RATE_Hz))) // rate perido in milliseconds

// ----- battery configs -----
#define AS_PERCENT      false // convert voltage to percent
#define BAT_AVERAGE_POINTS 30 // read average total points
#define ADC_VOLTAGE       3.3 // esp32 ADC voltage reference
#define ADC_RESOLUTION float(4095) // esp32 ADC resolution

// Structure to store the values of R1 and R2
struct battery_config_t {
    uint8_t mac[6] ;
    float R1;
    float R2;
};


// ----- pinout -----
extern const uint8_t pin_battery;

/**
 * @brief battery level task example
 * 
 * @param arg void arg
 */
void task_battery(void*);

void task_battery(void *arg){

    battery_config_t* battery_config = (battery_config_t*)arg;

    bool all_zeros = false;
    uint8_t count; 
    float R1 = battery_config->R1;
    float R2 = battery_config->R2;
    uint8_t mac[6];
    memcpy(mac, battery_config->mac, sizeof(mac));

    // Loop through the array
    for (int i = 0; i < 6; i++) {
        if (mac[i] == 0) {
            count++;
        }
    }

    if (count == 6) {
        all_zeros = true;
    } else {
        all_zeros = false;
    }
    
    // -----create local variables-----
    uint32_t sum;
    uint16_t adc_read;
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
        "%d bytes remaining of the battery task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    while(true){
        // ----- sum average -----
        sum = false; // clean sum
        for (int i=0; i < BAT_AVERAGE_POINTS; i++) {
            sum += analogRead(pin_battery);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        adc_read = sum / float(BAT_AVERAGE_POINTS);

        // -----calculate voltage-----
        voltage_read = (ADC_VOLTAGE * adc_read) / ADC_RESOLUTION;
        voltage_bat = voltage_read / (R2 / (R1 + R2));
        bat.value = voltage_bat;

        // -----convert to percent-----
    #if (AS_PERCENT)
        if (bat.value > BAT_MAX)
            bat.value = 100.0;
        else if (bat.value < BAT_MIN)
            bat.value = 0.0;
        else
            bat.value = ((bat.value - BAT_MIN) / (BAT_MAX - BAT_MIN)) * 100.0; 
    #endif
        
        if(all_zeros) {
            // -----send tilt data through queue-----
            xQueueSend(qh_battery, &bat, pdMS_TO_TICKS(0));
        }
        else {
            // -----send system data through esp-now-----
            esp_now_send(mac, (uint8_t *) &bat, sizeof(sensor_t));
        }

        vTaskDelay(TASK_BATTERY_RATE_ms / portTICK_PERIOD_MS);
    }
}

#endif // __BATTERY_H__