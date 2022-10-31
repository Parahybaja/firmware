/**
 * @brief sensor 3 task example
 * 
 * @param arg void arg
 */
void task_sensor_3(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_read, timer_send;
    uint16_t buffer_count = 0;
    float sum;
    sensor_t sensor3 = {
        .id = BOARDID, 
        .type = SENSOR_03, 
        .value = 0.0
    };

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_1: %d bytes remaining of the sensor 3 task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----update timer-----
    timer_read = timer_send = millis();

    while (true){
        if ((millis() - timer_read) >= TASK3_READING_RATE_ms){
            // -----add to timer-----
            timer_read += TASK3_READING_RATE_ms;

            // -----read sensors-----
            sum += analogRead(PIN_EXAMPLE3) * 1,234;

            // -----update buffer-----
            buffer_count++;
        }
        if ((millis() - timer_send) >= TASK3_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK3_SEND_RATE_ms;

            // -----calculate average-----
            sensor3.value = float(sum / buffer_count);

            // -----clear buffer-----
            buffer_count = false;

            // -----send sensor 3 data through queue-----
            xQueueSend(qh_sensor_3, &sensor3, pdMS_TO_TICKS(0));
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief sensor 4 task example
 * 
 * @param arg void arg
 */
void task_sensor_4(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_read, timer_send;
    uint16_t buffer_count = 0;
    float sum;
    sensor_t sensor4 = {
        .id = BOARDID, 
        .type = SENSOR_04, 
        .value = 0.0
    };

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_1: %d bytes remaining of the sensor 4 task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----update timer-----
    timer_read = timer_send = millis();

    while (true){
        if ((millis() - timer_read) >= TASK4_READING_RATE_ms){
            // -----add to timer-----
            timer_read += TASK4_READING_RATE_ms;

            // -----read sensors-----
            sum += analogRead(PIN_EXAMPLE4) * 4,321;

            // -----update buffer-----
            buffer_count++;
        }
        if ((millis() - timer_send) >= TASK4_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK4_SEND_RATE_ms;

            // -----calculate average-----
            sensor4.value = float(sum / buffer_count);

            // -----clear buffer-----
            buffer_count = false;

            // -----send sensor 4 data through queue-----
            xQueueSend(qh_sensor_4, &sensor4, pdMS_TO_TICKS(0));  
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief display control task
 * 
 * @param arg void arg
 */
void task_display_control(void *arg){
    (void)arg;

    // -----create local variables-----
    sensor_t recv_sensor1 = {BOARDID, SENSOR_01, 0.0};
    sensor_t recv_sensor2 = {BOARDID, SENSOR_02, 0.0};
    sensor_t recv_sensor3 = {BOARDID, SENSOR_03, 0.0};
    sensor_t recv_sensor4 = {BOARDID, SENSOR_04, 0.0};

    //init_display(); 

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_1: %d bytes remaining of the display control task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    while (true){
        if (xQueueReceive(qh_sensor_1, &recv_sensor1, pdMS_TO_TICKS(1))){
            // display function simulation
            Serial.print("sensor1:");
            Serial.println(recv_sensor1.value);

            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.sensor_1 = recv_sensor1.value;
            xSemaphoreGive(sh_global_vars);
        }
        if (xQueueReceive(qh_sensor_2, &recv_sensor2, pdMS_TO_TICKS(1))){
            // display function simulation
            Serial.print("sensor2:");
            Serial.println(recv_sensor2.value);

            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.sensor_2 = recv_sensor2.value;
            xSemaphoreGive(sh_global_vars);
        }
        if (xQueueReceive(qh_sensor_3, &recv_sensor3, pdMS_TO_TICKS(1))){
            // display function simulation
            Serial.print("sensor3:");
            Serial.println(recv_sensor3.value);

            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.sensor_3 = recv_sensor3.value;
            xSemaphoreGive(sh_global_vars);
        }
        if (xQueueReceive(qh_sensor_4, &recv_sensor4, pdMS_TO_TICKS(1))){
            // display function simulation
            Serial.print("sensor4:");
            Serial.println(recv_sensor4.value);

            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.sensor_4 = recv_sensor4.value;
            xSemaphoreGive(sh_global_vars);
        }
        
        vTaskDelay(1 / portTICK_PERIOD_MS); // give time to send the espnow message
    }
}

/**
 * @brief read from the global vars and write to SD card
 * 
 * @param arg void arg
 */
void task_write_SD(void *arg){
    (void)arg;

    // -----create local variables-----
    system_t data_received;
    system_t buffer_data[BUFFER_SIZE];
    uint32_t timer;
    uint16_t buffer_count = 0;

    init_file(); // only return when initialize correctly

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_1: %d bytes remaining of the write SD task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----update timer-----
    timer = millis();

    while (true){
        if ((millis() - timer) >= TASK_SD_RATE_ms){
            // -----increment timer-----
            timer += TASK_SD_RATE_ms;

            // add to buffer
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                buffer_data[buffer_count] = system_global;
            xSemaphoreGive(sh_global_vars);

            // -----write data to SD card-----
            if(++buffer_count >= BUFFER_SIZE){
                // reset buffer counter
                buffer_count = false;

                // convert data to string
                String data_string = data_to_string(buffer_data);
                
                // add data to the file
                appendFile(SD, file_name.c_str(), data_string.c_str());

                // -----clear buffer-----
                memset(buffer_data, 0, sizeof(buffer_data));
            }
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}


/**
 * @brief read from the global vars and send pack to BOXECU
 * 
 * @param arg void arg
 */
void task_send_pack(void *arg){
    (void)arg;

    // -----create local variables-----
    system_t system_copy;
    uint32_t timer;

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_1: %d bytes remaining of the send pack task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----update timer-----
    timer = millis();

    while (true){
        if ((millis() - timer) >= TASK_SENDPACK_RATE_ms){
            // -----increment timer-----
            timer += TASK_SENDPACK_RATE_ms;

            // -----add to buffer-----
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_copy = system_global;
            xSemaphoreGive(sh_global_vars);

            // -----send system data through esp-now-----
            esp_now_send(address_control, (uint8_t *) &system_copy, sizeof(system_copy));
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}