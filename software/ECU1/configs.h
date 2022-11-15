// -----system configs-----
#define BOARDID              BOARD_02 //
#define SERIAL_FREQ          115200   // serial monitor baud rate
#define DEBUG_DELAY          5000     // delay between debug messages
#define DEBUG_MODE           true    // config if debug mode is active
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
#define TASK_SD_RATE_Hz       1       // reading rate in hertz (must be greater then send rate)
#define TASK_SENDPACK_RATE_Hz 1       // reading rate in hertz (must be greater then send rate)
#define TASK_BATTERY_RATE_Hz  0.1     // reading rate in hertz (must be greater then send rate)
#define TASK_ROLLOVER_RATE_Hz 1       // reading rate in hertz (must be greater then send rate)

// -----convertions-----
#define TASK_SD_RATE_ms       (int(1000.0 / float(TASK_SD_RATE_Hz))) // rate perido in milliseconds
#define TASK_SENDPACK_RATE_ms (int(1000.0 / float(TASK_SENDPACK_RATE_Hz))) // rate perido in milliseconds
#define TASK_BATTERY_RATE_ms  (int(1000.0 / float(TASK_BATTERY_RATE_Hz))) // rate perido in milliseconds
#define TASK_ROLLOVER_RATE_ms (int(1000.0 / float(TASK_ROLLOVER_RATE_Hz))) // rate perido in milliseconds

// -----VSPI pinout-----
#define VSPI_CLK  18
#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SS   05
#define VSPI_RST  17

// -----HSPI pinout-----
#define HSPI_CLK  27
#define HSPI_MISO 14
#define HSPI_MOSI 13
#define HSPI_SS   04

// -----microSD configs-----
#define BUFFER_SIZE 64           // block of data size to be written on SD card
#define FILE        "/PB22_reg/file" // should be the test or subsystem name
#define TITLE       "time,speed,rpm,fuel_emer,battery,temp,rollover,tilt_x,tilt_y,tilt_z\r\n" // file title
#define EXTENSION   ".csv"       // file extension (.csv, .txt, ...)

// -----battery configs-----
#define R1 10375.0
#define R2 2085.0

// -----general pinout-----
#define PIN_LED_ALIVE 12
#define PIN_BATTERY   35

// -----MPU6050 configs-----
float gyro_offset[] = {0.0, 0.0, 0.0};
float acc_offset[]  = {0.0, 0.0, 0.0};

// -----esp-now addresses-----
uint8_t address_ECUBOX[] = {0x94, 0xB5, 0x55, 0x2D, 0x1E, 0x0C};
uint8_t address_ECU1[]   = {0x0C, 0xB8, 0x15, 0xC3, 0xF0, 0x50};
uint8_t address_ECU2[]   = {0x7C, 0x9E, 0xBD, 0xED, 0x52, 0x70};