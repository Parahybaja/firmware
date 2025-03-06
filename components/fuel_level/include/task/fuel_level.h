/**
 * @file fuel_level.h
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
#include "driver/pulse_cnt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PCNT_HIGH_LIMIT 10000
#define PCNT_LOW_LIMIT  -10

void task_fuel_level(void*);

#ifdef __cplusplus
}
#endif