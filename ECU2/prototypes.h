// -----tasks functions-----

void task_alive(void*);
void isr_rpm(void);
void task_RPM(void*);
void isr_spdmt(void);
void task_speedometer(void*);
void task_fuel_emer(void*);



// -----esp-now functions-----

void init_espnow(void);
void OnDataSent(const uint8_t*, esp_now_send_status_t);
void OnDataRecv(const uint8_t*, const uint8_t*, int);



// -----system-----

void init_system(void);
void init_system_handlers(void);
void init_tasks(void);
void ERROR(char*);
void ERROR(char*, bool);
void INFO(const char*);
void INFO(const char*, bool);
void send_debug(const char*);
void on_error(char*, bool);