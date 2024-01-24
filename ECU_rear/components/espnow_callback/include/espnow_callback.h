/**
 * @file espnow_callback.h
 * @authors 
 *      Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief espnow callback funtions
 * @version 2.0
 * @date 2024-01-24
 *
 * @copyright Copyright (c) 2024
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Jefferson L.  19/02/2023  last version
 *  1.0.0    Jefferson L.  02/08/2023  add AFV task to callbacks
 *  1.0.1    Jefferson L.  02/08/2023  update to use reset_time_counter function
 *  2.0.0    Jefferson L.  22/01/2024  esp-idf convertion
 * 
 */

#pragma once

#include "system.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief set to true to enable log prints on the espnow callback
 * and false to disable
 * 
 */
#define DEBUG_ESPNOW_CALLBACK true

/**
 * @brief ESP-NOW on data sent callback
 *
 * @param mac_addr device MAC address
 * @param status esp-now status
 */
void espnow_send_callback(const uint8_t*, esp_now_send_status_t);

/**
 * @brief ESP-NOW on data receive callback
 *
 * @param mac device MAC address
 * @param incomingData data received
 * @param len array size
 */
void espnow_recv_callback(const esp_now_recv_info_t*, const uint8_t*, int);

/**
 * @brief ESP-NOW register callbacks functions
 * 
 */
void register_callbacks(void);

#ifdef __cplusplus
}
#endif