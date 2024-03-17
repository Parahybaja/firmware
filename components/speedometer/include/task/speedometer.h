/**
 * @file alive.h
 * @authors 
 *      Jefferson Lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief calculate speed
 * @version 1.0
 * @date 2024-03-08
 *
 * @copyright Copyright (c) 2023
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.3.0    Jefferson L.  26/10/2023  fix speed calculation
 *  1.0.0    Jefferson L.  08/03/2024  idf convertion
 * 
 */

#pragma once

#include "system.h"
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_SPEED_SEND_RATE_Hz  2 // speedometer task send rate in hertz

#define PCNT_HIGH_LIMIT 10000
#define PCNT_LOW_LIMIT  -10

// ----- constants -----
#define PI          3.1415f
#define ms2kmh      3.6f
#define WHEEL_DIA   0.45f            // diameter in meters
#define WHEEL_CIRC  (PI * WHEEL_DIA) // wheel circumference
#define WHEEL_EDGES 6.0f             // 6 edges per revolution

/**
 * @brief keep blinking a LED to ensure the CPU is working properly
 * 
 */
void task_speed(void*);

#ifdef __cplusplus
}
#endif