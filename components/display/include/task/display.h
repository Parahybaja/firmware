/**
 * @file task_example.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief general task example
 * @version 0.1
 * @date 2024-01-13
 *
 * @copyright Copyright (c) 2024
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Jefferson L.  10/02/2023  first version
 * 
 */

#pragma once

#include "system.h"
#include "esp32_driver_nextion/nextion.h"
#include "esp32_driver_nextion/page.h"
#include "esp32_driver_nextion/component.h"

#ifdef __cplusplus
extern "C" {
#endif

void task_display(void*);

void callback_touch_event(nextion_on_touch_event_t event);


#ifdef __cplusplus
}
#endif