/**
 * @file system.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief system's general types definitions and basic esp-now communication functions
 * @version 2.0
 * @date 2024-01-08
 *
 * @copyright Copyright (c) 2023
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Jefferson L.  10/02/2023  first version
 *  1.0.0    Jefferson L.  18/07/2023  move common_libs and rename a few modules
 *  1.1.0    Jefferson L.  26/10/2023  remove Wire begin
 *  2.0.0    Jefferson L.  08/01/2024  esp-idf convertion
 * 
 */

#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----FreeRTOS objects-----
extern TaskHandle_t th_example;

void func1(void);

#ifdef __cplusplus
}
#endif
