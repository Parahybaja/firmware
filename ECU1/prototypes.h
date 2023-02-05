// -----tasks functions-----

void task_sensor_3(void*);
void task_sensor_4(void*);
void task_display_control(void*);
void task_write_SD(void*);
void task_send_pack(void*);



// -----SD card module functions-----

void init_file(void);
void writeFile(fs::FS&, const char*, const char*);
void appendFile(fs::FS&, const char*, const char*);
void createFile(void);
String data_to_string(system_t*);



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