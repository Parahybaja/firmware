// -----FreeRTOS objects-----
TaskHandle_t th_alive; // LED alive task handler
TaskHandle_t th_rpm;   // RPM task handler
TaskHandle_t th_spdmt; // speedometer task handler
TaskHandle_t th_fuel;  // emergency fuel task handler

// -----command lookup table-----
typedef enum {
    CMD_NULL,     /*empty command*/
    CMD_START,    /*start command*/
    CMD_STOP,     /*stop command*/
    CMD_NEW_FILE, /*new file command*/
    CMD_RESTART   /*restart command*/
} commands_t;

// -----board ID secret values-----
typedef enum {
    BOARD_01 = 0x7CE, /*1998*/
    BOARD_02 = 0x7CF, /*1998 + 1*/
    BOARD_03 = 0x7D0  /*1998 + 2*/
} boardID_t;

typedef enum {
    RPM,
    SPEEDOMETER,
    FUEL_LEVEL,
    FUEL_EMERGENCY,
    AMBIENT_TEMP,
    ROLLOVER,
    TILT_X,
    TILT_Y,
    TILT_Z
} sensors_t;

// -----debug data type definition-----
typedef struct {
    boardID_t id;
    char msg[ESPNOW_BUFFER_SIZE];
} debug_t;

// -----espnow command data type-----
typedef struct {
    boardID_t id;
    commands_t command;
} cmd_t;

// -----sensor data type definition-----
typedef struct {
    boardID_t id;
    sensors_t type;
    float value;  // sensor data must be float due to a bug in writing to the sd card
} sensor_t;

// -----interrupts global variables-----
volatile uint16_t counter_rpm, counter_spdmt;