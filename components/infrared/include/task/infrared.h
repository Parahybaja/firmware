/**
 * @file infrared.h
 * @authors 
 *      Raynoan Emilly (raynoan.batista@ee.ufcg.edu.br)
 *      Flaviano Medeiros (flaviano.medeiros@estudante.ufcg.edu.br)
 * @brief Infrared sensor task
 * @version 0.1
 * @date 2024-10-23
 * 
 * @copyright Copyright (c) 2024
 * 
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Raynoan Emilly       23/09/2024  First version
 */

#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "driver/gpio.h"
#include "system.h"

#ifdef __cplusplus
extern "C" {
#endif
#define TASK_INFRARED_RATE_Hz 1 // reading rate in hertz
#define pin_infrared  GPIO_NUM_26
#define queue_size 10
#define DEBOUNCE_DELAY_MS 100

void task_infrared(void*);

#ifdef __cplusplus
}
#endif
