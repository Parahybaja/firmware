/**
 * @file fuel_em.h
 * @author Jefferson Lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief fuel emergency task
 * @version 1.0
 * @date 2024-01-15
 *
 * @copyright Copyright (c) 2024
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Jefferson L.  10/02/2023  first version
 *  1.0.0    Jefferson L.  15/01/2024  esp-idf convertion
 * 
 */

#pragma once

#include "system.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_FUEL_SEND_RATE_Hz 0.5 // Fuel task send rate in hertz

#define ACTIVE_LOW false
#define FUEL_AVERAGE_POINTS 5
#define THRESHOLD 0.5 // to be calibrated

void task_fuel_em(void*);

#ifdef __cplusplus
}
#endif