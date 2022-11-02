/**
 * @brief init system general modules (pinMode, Serial and SPI)
 * 
 */
void init_system(void){
    // -----Serial initiallization-----
    Serial.begin(SERIAL_FREQ);
    while (!Serial){}; // wait until is initialized
    Wire.begin();

    // -----header-----
    Serial.println("|--------------------|");
    Serial.println("|                    |");
    Serial.println("|        ECU1        |");
    Serial.println("|                    |");
    Serial.println("|--------------------|");

    // -----both SPI inittialization-----
    vspi = new SPIClass(VSPI);
    hspi = new SPIClass(HSPI);

    vspi->begin(VSPI_CLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
    hspi->begin(HSPI_CLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

    pinMode(vspi->pinSS(), OUTPUT);
    pinMode(hspi->pinSS(), OUTPUT);
}

/**
 * @brief init system handlers such as queues and semaphores
 * 
 */
void init_system_handlers(void){
    // -----create semaphores-----
    sh_SD = xSemaphoreCreateMutex();
    if(sh_SD == NULL)
        ERROR("ERROR_1: SD Semaphore init failed", false);

    sh_global_vars = xSemaphoreCreateMutex();
    if(sh_global_vars == NULL)
        ERROR("ERROR_1: global vars Semaphore init failed", false);

    // -----create queues-----
    qh_rpm        = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_speed      = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_fuel_level = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_fuel_emer  = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_battery    = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_rollover   = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_tilt_x     = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_tilt_y     = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_tilt_z     = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
}

/**
 * @brief init run time tasks
 * 
 */
void init_tasks(void){
    xTaskCreate(task_display_control, // task function
                "display control",    // task name
                2048,                 // stack size
                NULL,                 // parameters
                10,                   // priority
                &th_display_control); // handler 

    xTaskCreate(task_battery,   // task function
                "task_battery", // task name
                2048,           // stack size
                NULL,           // parameters
                10,             // priority
                &th_battery);   // handler  

    // xTaskCreate(task_sensor_4, // task function
    //             "sensor 4",    // task name
    //             2048,          // stack size
    //             NULL,          // parameters
    //             10,            // priority
    //             &th_sensor_4); // handler 

    xTaskCreate(task_send_pack, // task function
                "send pack",    // task name
                2048,           // stack size
                NULL,           // parameters
                10,             // priority
                &th_send_pack); // handler 

    xTaskCreate(task_write_SD, // task function
                "write to SD", // task name
                8048,          // stack size
                NULL,          // parameters
                10,            // priority
                &th_SD);       // handler
}

/**
 * @brief print ERROR messages on serial and through esp-now:
 * 
 * @param returnMsg const *char: message to be sent
 */
void ERROR(char returnMsg[]){
    Serial.println();
    Serial.println(returnMsg);
    send_debug(returnMsg);

    on_error(returnMsg, true);
}

/**
 * @brief print ERROR messages on serial and through esp-now:
 * 
 * @param returnMsg const *char: message to be sent
 * @param espnow_active bool: esp-now active flag
 */
void ERROR(char returnMsg[], bool espnow_active){
    Serial.println();
    Serial.println(returnMsg);
    if (espnow_active)
        send_debug(returnMsg);
    
    on_error(returnMsg, espnow_active);
}

/**
 * @brief print INFO messages on serial and through esp-now:
 * 
 * @param returnMsg const *char: message to be sent
 */
void INFO(const char returnMsg[]){
    Serial.println();
    Serial.println(returnMsg);
    send_debug(returnMsg);
}

/**
 * @brief print INFO messages on serial and through esp-now:
 * 
 * @param returnMsg const *char: message to be sent
 * @param espnow_active bool: esp-now active flag
 */
void INFO(const char returnMsg[], bool espnow_active){
    Serial.println();
    Serial.println(returnMsg);
    if (espnow_active)
        send_debug(returnMsg);
}

/**
 * @brief send debug messages through esp-now
 * 
 * @param returnMsg const *char: message to be sent
 */
void send_debug(const char returnMsg[]){
    if (sizeof(returnMsg) <= ESPNOW_BUFFER_SIZE){
        debug_t msg_data;
        msg_data.id = BOARDID;
        strcpy(msg_data.msg, returnMsg);
        esp_now_send(address_control, (uint8_t *) &msg_data, sizeof(msg_data));
    }
    else
        Serial.println("ERROR_1: espnow buffer overflow");
}

/**
 * @brief send error messages through esp-now with option
 * 
 * @param returnMsg const *char: message to be sent
 * @param espnow_active bool: esp-now active flag
 */
void on_error(char returnMsg[], bool espnow_active){
    if (REBOOT_ON_ERROR){
        ESP.restart();
        delay(1);
    }
    else{
        while(true){
            Serial.println(returnMsg);

            if (espnow_active)
                send_debug(returnMsg);
            
            delay(DEBUG_DELAY);
        }
    }
}