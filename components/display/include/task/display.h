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

#define NEX_PAGE_INTRO "page0"
#define NEX_PAGE_MODE_BLACK "page1"
#define NEX_TEXT_SPEED "p1t0"
#define NEX_TEXT_BATTERY "p1t1"
#define NEX_TEXT_TEMP "p1t2"
#define NEX_TEXT_ROLL "p1t3"
#define NEX_TEXT_PITCH "p1t4"
#define NEX_DSBUTTON_FUEL_EM "p1bt0"
#define NEX_PROGRESSBAR_RPM "p1j0"
#define NEX_PAGE_MODE_WHITE "page2"
#define NEX_TEXT_SPEED_WHITE "p2t0"
#define NEX_PROGRESSBAR_RPM_WHITE "p2j0"

#define NEX_SYMBOL_DEGREE 0xB0 // hex value for °
#define NEX_SYMBOL_PERCENT 0x25 // hex value for %

#define NEX_RPM_MAX (float)4000
#define NEX_RPM_MIN (float)0
#define NEX_BAT_MAX (float)12.8 // battery 100%
#define NEX_BAT_MIN (float)11.0 // battery 0%

/**
 * @brief 
 * 
 * @param arg 
 */
void task_display(void*);

/**
 * @brief 
 * 
 * @param event 
 */
static void callback_touch_event(nextion_on_touch_event_t);

/**
 * @brief 
 * 
 * @param arg 
 */
static void process_callback_queue(void*);

/**
 * @brief 
 * 
 * @param value 
 * @param max 
 * @param min 
 * @return float 
 */
float convert_to_percent(float, float, float);

#ifdef __cplusplus
}
#endif