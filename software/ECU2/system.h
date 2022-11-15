/**
 * @brief init system general modules (pinMode and Serial)
 * 
 */
void init_system(void){
    // -----Serial initiallization-----
    Serial.begin(SERIAL_FREQ);
    while (!Serial){}; // wait until is initialized

    // -----header-----
    Serial.println("|--------------------|");
    Serial.println("|                    |");
    Serial.println("|        ECU2        |");
    Serial.println("|                    |");
    Serial.println("|--------------------|");
}

/**
 * @brief init system handlers such as queues and semaphores
 * 
 */
void init_system_handlers(void){
    INFO("INFO_2: no system handlers", false);
    delay(50);
}

/**
 * @brief init run time tasks
 * 
 */
void init_tasks(void){
    xTaskCreate(task_alive,     // task function
                "alive_signal", // task name
                2048,           // stack size
                NULL,           // parameters
                10,             // priority
                &th_alive);     // handler 

    xTaskCreate(task_RPM,   // task function
                "task_RPM", // task name
                2048,       // stack size
                NULL,       // parameters
                10,         // priority
                &th_rpm);   // handler 

    xTaskCreate(task_speedometer,   // task function
                "task_speedometer", // task name
                2048,               // stack size
                NULL,               // parameters
                10,                 // priority
                &th_spdmt);         // handler 

    xTaskCreate(task_fuel_emer,   // task function
                "task_fuel_emer", // task name
                2048,             // stack size
                NULL,             // parameters
                10,               // priority
                &th_fuel);        // handler 
}

/**
 * @brief print ERROR messages on serial and through esp-now:
 * 
 * @param returnMsg const char[]: message to be sent
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
 * @param returnMsg const char[]: message to be sent
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
 * @param returnMsg const char[]: message to be sent
 */
void INFO(const char returnMsg[]){
    Serial.println();
    Serial.println(returnMsg);
    send_debug(returnMsg);
}

/**
 * @brief print INFO messages on serial and through esp-now:
 * 
 * @param returnMsg const char[]: message to be sent
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
 * @param returnMsg const char[]: message to be sent
 */
void send_debug(const char returnMsg[]){
    if (sizeof(returnMsg) <= ESPNOW_BUFFER_SIZE){
        debug_t msg_data;
        msg_data.id = BOARDID;
        strcpy(msg_data.msg, returnMsg);
        esp_now_send(address_ECUBOX, (uint8_t *) &msg_data, sizeof(msg_data));
    }
    else
        Serial.println("ERROR_2: espnow buffer overflow");
}

/**
 * @brief send error messages through esp-now with option
 * 
 * @param returnMsg const char[]: message to be sent
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