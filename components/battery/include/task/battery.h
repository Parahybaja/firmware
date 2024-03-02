/**
 * @file battery.h
 * @authors 
 *      Jefferson Lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief battery voltage task
 * @version 2.0
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Jefferson L.  10/02/2023  first version
 *  1.3.0    Jefferson L.  26/10/2023  add queue and espnow option 
 *  2.0.0    Jefferson L.  10/02/2024  esp-idf convertion
 *  2.1.0    Jefferson L.  20/02/2024  remove queue capability
 * 
 */

#pragma once

#include "system.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_BATTERY_RATE_Hz 0.2 // reading rate in hertz

// ----- battery configs -----
#define AS_PERCENT         false // convert voltage to percent
#define BAT_AVERAGE_POINTS 30 // read average total points
#define ADC_VOLTAGE        3.3 // esp32 ADC voltage reference
#define ADC_RESOLUTION     (float)4095 // esp32 ADC resolution

// Structure to store the values of R1 and R2
typedef struct {
    adc_channel_t adc_channel;
    float R1;
    float R2;
} battery_config_t;

void task_battery(void*);

#ifdef __cplusplus
}
#endif