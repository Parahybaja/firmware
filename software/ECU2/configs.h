// -----system configs-----
#define BOARDID               BOARD_03
#define SERIAL_FREQ           115200      // serial monitor baud rate
#define DEBUG_DELAY           5000        // delay between debug messages
#define DEBUG_MODE            true        // config if debug mode is active
#define REBOOT_ON_ERROR       false       // reboot on error flag
#define REBOOT_ON_DISCONNECT  false       // reboot on disconnect flag
#define ESPNOW_BUFFER_SIZE    64          // espnow buffer char array size

// -----tasks configs-----
#define TASK_RPM_SEND_RATE_Hz   1 // RPM task send rate in hertz
#define TASK_SPDMT_SEND_RATE_Hz 1 // speedometer task send rate in hertz
#define TASK_F_EMER_SEND_RATE_Hz  1           // task send rate in hertz

// -----convertions-----
#define TASK_RPM_SEND_RATE_ms     (int(1000.0 / float(TASK_RPM_SEND_RATE_Hz))) // rate perido in milliseconds
#define TASK_RPM_SEND_RATE_min    (float(1.0 / (float(TASK_RPM_SEND_RATE_Hz) * 60.0))) // rate perido in min
#define TASK_SPDMT_SEND_RATE_ms   (int( 1000.0 / float(TASK_SPDMT_SEND_RATE_Hz))) // rate perido in milliseconds
#define TASK_SPDMT_SEND_RATE_s    (float (1.0 / float(TASK_SPDMT_SEND_RATE_Hz))) // rate perido in seconds
#define TASK_SPDMT_SEND_RATE_min  (float(1.0 / (float(TASK_SPDMT_SEND_RATE_Hz) * 60.0))) // rate perido in min
#define TASK_F_EMER_SEND_RATE_ms  (int( 1000.0 / float(TASK_F_EMER_SEND_RATE_Hz))) // rate perido in milliseconds
#define TASK_F_EMER_SEND_RATE_s   (float (1.0 / float(TASK_F_EMER_SEND_RATE_Hz))) // rate perido in seconds
#define TASK_F_EMER_SEND_RATE_min (float(1.0 / (float(TASK_F_EMER_SEND_RATE_Hz) * 60.0))) // rate perido in min

// -----constants-----
#define PI          3.1415
#define ms2kmh      3.6
#define WHEEL_DIA   0.4              // diameter in meters
#define WHEEL_CIRC  (PI * WHEEL_DIA) // wheel circumference
#define WHEEL_EDGES 6                // 6 edges per revolution

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
uint8_t address_ECUBOX[] = {0x5C, 0xCF, 0x7F, 0x65, 0xF1, 0x9E};
uint8_t address_ECU1[]   = {0x7C, 0x9E, 0xBD, 0xED, 0x52, 0x70};
//uint8_t address_ECU1[] = {0x94, 0xB9, 0x7E, 0xC0, 0x27, 0xA8};
uint8_t address_ECU2[]   = {0x94, 0xB5, 0x5, 0x2D, 0x1E, 0x0C};