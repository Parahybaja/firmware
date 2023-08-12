/**
 * @file configs.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief general configuration file
 * @version 0.2
 * @date 2023-02-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CONFIGS_H__
#define __CONFIGS_H__

// -----system configs-----
#define SERIAL_FREQ          115200   // serial monitor baud rate
#define DEBUG_DELAY          5000     // delay between debug messages
#define DEBUG_MODE           true     // config if debug mode is active
#define REBOOT_ON_ERROR      false    // reboot on error flag
#define REBOOT_ON_DISCONNECT false    // reboot on disconnect flag
#define ESPNOW_BUFFER_SIZE   64       // espnow buffer char array size
#define QUEUE_BUFFER_SIZE    32
#define QUEUE_TIMEOUT        1000     // queue timeout in millisecond

// -----calculation configs-----
#define RPM_MAX (float(4000))
#define RPM_MIN (float(0))
#define BAT_MAX (float(12.8)) // battery 100%
#define BAT_MIN (float(11.0)) // battery 0%

// -----esp-now addresses-----
const uint8_t address_ECU_BOX[]   = {0x94, 0xB5, 0x55, 0x2D, 0x1E, 0x0C};
const uint8_t address_ECU_Front[] = {0x94, 0xB5, 0x55, 0x2D, 0x1A, 0x60};
const uint8_t address_ECU_Rear[]  = {0x7C, 0x9E, 0xBD, 0xED, 0x52, 0x70};
const uint8_t address_module_1[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8D, 0x38};
const uint8_t address_module_2[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8A, 0xD8};
const uint8_t address_module_3[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8B, 0xA0};

#endif // __CONFIGS_H__