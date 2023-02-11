/**
 * @file configs.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief general configuration file
 * @version 0.1
 * @date 2023-02-10
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
#define QUEUE_BUFFER_SIZE    16
#define QUEUE_TIMEOUT        1000     // queue timeout in millisecond

// -----tasks configs-----
#define TASK_ROLLOVER_RATE_Hz 1 // reading rate in hertz (must be greater then send rate)
#define TASK_SENDPACK_RATE_Hz 1 // reading rate in hertz (must be greater then send rate)
#define TASK_SD_RATE_Hz       1 // reading rate in hertz (must be greater then send rate)

// -----convertions-----
#define TASK_SENDPACK_RATE_ms (int(1000.0 / float(TASK_SENDPACK_RATE_Hz))) // rate perido in milliseconds
#define TASK_ROLLOVER_RATE_ms (int(1000.0 / float(TASK_ROLLOVER_RATE_Hz))) // rate perido in milliseconds
#define TASK_SD_RATE_ms (int(1000.0 / float(TASK_SD_RATE_Hz))) // rate perido in milliseconds

// -----general pinout-----
#define PIN_LED_ALIVE 12
#define PIN_BATTERY   35

// -----esp-now addresses-----
uint8_t address_ECU_BOX[]   = {0x94, 0xB5, 0x55, 0x2D, 0x1E, 0x0C};
uint8_t address_ECU_Front[] = {0x94, 0xB5, 0x55, 0x2D, 0x1A, 0x60};
uint8_t address_ECU_Rear[]  = {0x7C, 0x9E, 0xBD, 0xED, 0x52, 0x70};

// backups
// uint8_t address_ECU_DevKitC[]  = {0x94, 0xB9, 0x7E, 0xC0, 0x27, 0xA8};

#endif // __CONFIGS_H__