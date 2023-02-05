#ifndef CONFIGS_H
#define CONFIGS_H

// -----system configs-----
#define SERIAL_FREQ          115200   // serial monitor baud rate
#define DEBUG_DELAY          5000     // delay between debug messages
#define DEBUG_MODE           true     // config if debug mode is active
#define REBOOT_ON_ERROR      false    // reboot on error flag
#define REBOOT_ON_DISCONNECT false    // reboot on disconnect flag
#define ESPNOW_BUFFER_SIZE   64       // espnow buffer char array size
#define QUEUE_BUFFER_SIZE    16
#define QUEUE_TIMEOUT        1000     // queue timeout in millisecond

// -----MPU configs-----
#define MPU_ADDR        0x68
#define MPU_GYRO_CONFIG 3
#define MPU_ACC_CONFIG  3
/**
 * GYRO_CONFIG_[0,1,2,3] range = +- [250, 500,1000,2000] °/s
 *                       sensi =    [131,65.5,32.8,16.4] bit/(°/s)
 *
 *  ACC_CONFIG_[0,1,2,3] range = +- [    2,   4,   8,  16] times the gravity (9.81m/s²)
 *                       sensi =    [16384,8192,4096,2048] bit/gravity
 */

// -----tasks configs-----
#define TASK_ROLLOVER_RATE_Hz 1       // reading rate in hertz (must be greater then send rate)

// -----convertions-----
#define TASK_SENDPACK_RATE_ms (int(1000.0 / float(TASK_SENDPACK_RATE_Hz))) // rate perido in milliseconds
#define TASK_ROLLOVER_RATE_ms (int(1000.0 / float(TASK_ROLLOVER_RATE_Hz))) // rate perido in milliseconds

// -----general pinout-----
#define PIN_LED_ALIVE 12
#define PIN_BATTERY   35

// -----esp-now addresses-----
uint8_t address_ECU_BOX[]   = {0x94, 0xB5, 0x55, 0x2D, 0x1E, 0x0C};
uint8_t address_ECU_Front[] = {0x0C, 0xB8, 0x15, 0xC3, 0xF0, 0x50};
uint8_t address_ECU_Rear[]  = {0x7C, 0x9E, 0xBD, 0xED, 0x52, 0x70};

#endif