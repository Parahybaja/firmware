// -----tasks functions-----

void task_sensor_1(void*);
void task_sensor_2(void*);



// -----esp-now functions-----

void init_espnow(void);
void OnDataSent(const uint8_t*, esp_now_send_status_t);
void OnDataRecv(const uint8_t*, const uint8_t*, int);



// -----system-----

void init_system(void);
void init_system_handlers(void);
void ERROR(char*);
void ERROR(char*, bool);
void INFO(const char*);
void INFO(const char*, bool);
void send_debug(const char*);
void on_error(char*, bool);