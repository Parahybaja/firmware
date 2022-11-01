// -----system configs-----
#define BOARDID              BOARD_02 //
#define SERIAL_FREQ          115200   // serial monitor baud rate
#define DEBUG_DELAY          5000     // delay between debug messages
#define DEBUG_MODE           true     // config if debug mode is active
#define REBOOT_ON_ERROR      false    // reboot on error flag
#define REBOOT_ON_DISCONNECT false    // reboot on disconnect flag
#define ESPNOW_BUFFER_SIZE   48       // espnow buffer char array size
#define QUEUE_BUFFER_SIZE    16
#define QUEUE_TIMEOUT 1000            // queue timeout in millisecond

// -----tasks configs-----
#define TASK_SD_RATE_Hz       1           // reading rate in hertz (must be greater then send rate)
#define TASK_SENDPACK_RATE_Hz 1           // reading rate in hertz (must be greater then send rate)

// -----convertions-----
#define TASK_SD_RATE_ms       (int(1000.0 / float(TASK_SD_RATE_Hz))) // rate perido in milliseconds
#define TASK_SENDPACK_RATE_ms (int(1000.0 / float(TASK_SENDPACK_RATE_Hz))) // rate perido in milliseconds

// -----VSPI pinout-----
#define VSPI_CLK  18
#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SS   05

// -----HSPI pinout-----
#define HSPI_CLK  27
#define HSPI_MISO 14
#define HSPI_MOSI 13
#define HSPI_SS   15

// -----microSD configs-----
#define BUFFER_SIZE   64           // block of data size to be written on SD card
#define FILE          "/test/file" // should be the test or subsystem name
#define TITLE         "Time,sensor1,sensor2,sensor3,sensor4\r\n" // file title
#define EXTENSION     ".csv"       // file extension (.csv, .txt, ...)

// -----general pinout-----
#define PIN_LED_ALIVE 02
#define PIN_EXAMPLE3  36 // example sensor pin
#define PIN_EXAMPLE4  39 // example sensor pin

// -----esp-now addresses-----
uint8_t address_control[]  = {0x5C, 0xCF, 0x7F, 0x65, 0xF1, 0x9E};
uint8_t address_sender[]   = {0x7C, 0x9E, 0xBD, 0xED, 0x52, 0x70};
//uint8_t address_receiver[] = {0x94, 0xB9, 0x7E, 0xC0, 0x27, 0xA8};
uint8_t address_receiver[] = {0x94, 0xB5, 0x5, 0x2D, 0x1E, 0x0C};