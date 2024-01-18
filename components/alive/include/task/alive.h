/**
 * @file alive.h
 * @authors 
 *      Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief control alive signal, shows that it's not lockup
 * @version 2.0
 * @date 2024-01-15
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

#include "system.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief keep blinking a LED to ensure the CPU is working properly
 * 
 */
void task_alive_LED(void*);

#ifdef __cplusplus
}
#endif
