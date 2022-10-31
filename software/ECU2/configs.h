// -----system configs-----
#define BOARDID               BOARD_03
#define SERIAL_FREQ           115200      // serial monitor baud rate
#define DEBUG_DELAY           5000        // delay between debug messages
#define DEBUG_MODE            true        // config if debug mode is active
#define REBOOT_ON_ERROR       false       // reboot on error flag
#define REBOOT_ON_DISCONNECT  false       // reboot on disconnect flag
#define ESPNOW_BUFFER_SIZE    64          // espnow buffer char array size

// -----tasks configs-----
#define TASK1_SEND_RATE_Hz    1           // task 1 send rate in hertz
#define TASK1_READING_RATE_Hz 5           // reading rate in hertz (must be greater then send rate)
#define TASK2_SEND_RATE_Hz    1           // task 2 send rate in hertz
#define TASK2_READING_RATE_Hz 5           // reading rate in hertz (must be greater then send rate)

// -----convertions-----
#define TASK1_SEND_RATE_ms    (int(1000.0 / float(TASK1_SEND_RATE_Hz)))    // rate perido in milliseconds
#define TASK1_READING_RATE_ms (int(1000.0 / float(TASK1_READING_RATE_Hz))) // rate perido in milliseconds
#define TASK2_SEND_RATE_ms    (int(1000.0 / float(TASK2_SEND_RATE_Hz)))    // rate perido in milliseconds
#define TASK2_READING_RATE_ms (int(1000.0 / float(TASK2_READING_RATE_Hz))) // rate perido in milliseconds

// -----pinout-----
#define PIN_LED_ALIVE      02 // esp32 v1 builtin lED
#define PIN_RPM            04
#define PIN_FUEL_EMERGENCY 16
#define PIN_SPEEDOMETER    17
#define PIN_FUEL_HALL1     26
#define PIN_FUEL_HALL2     25
#define PIN_FUEL_HALL3     33
#define PIN_FUEL_HALL4     32
#define PIN_FUEL_HALL5     35
#define PIN_FUEL_HALL6     34

// -----esp-now addresses-----
uint8_t address_control[]  = {0x5C, 0xCF, 0x7F, 0x65, 0xF1, 0x9E};
uint8_t address_sender[]   = {0x7C, 0x9E, 0xBD, 0xED, 0x52, 0x70};
uint8_t address_receiver[] = {0x94, 0xB9, 0x7E, 0xC0, 0x27, 0xA8};