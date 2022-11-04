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

typedef struct {
    boardID_t id;
    commands_t command;
} cmd_t;

typedef struct {
    boardID_t id;
    char msg[ESPNOW_BUFFER_SIZE];
} debug_t;

typedef struct {
    boardID_t id;
    float sensor_1;
    float sensor_2;
    float sensor_3;
    float sensor_4;
} system_t;

String msg;