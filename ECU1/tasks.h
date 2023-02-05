/**
 * @brief alive signal
 * 
 * @param arg void arg
 */
void task_alive(void *arg){
    (void)arg;

    pinMode(PIN_LED_ALIVE, OUTPUT); 
    
    while (true){
        digitalWrite(PIN_LED_ALIVE, HIGH);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        digitalWrite(PIN_LED_ALIVE, LOW);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief battery level task example
 * 
 * @param arg void arg
 */
void task_battery(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_send;
    uint16_t adc_read;
    const float bat_max = 13.0;
    const float bat_min = 11.8;
    float voltage_read, voltage_bat;
    sensor_t bat = {
        .id = BOARDID, 
        .type = BATTERY, 
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
        "INFO_1: %d bytes remaining of the battery task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----update timer-----
    timer_send = millis();

    while (true){
        if ((millis() - timer_send) >= TASK_BATTERY_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_BATTERY_RATE_ms;

            // -----calculate average-----
            adc_read = analogRead(PIN_BATTERY);
            voltage_read = (3.3 * adc_read) / 4095.0;
            voltage_bat = voltage_read / (R2 / (R1 + R2));
            bat.value = voltage_bat;

            // -----convert to %-----
            if (bat.value > bat_max){
                bat.value = 100.0;
            }
            else if (bat.value < bat_min) {
                bat.value = 0.0;
            }
            else {
                bat.value = ((bat.value - bat_min) / (bat_max - bat_min)) * 100.0; 
            }
            
            // -----send bat data through queue-----
            xQueueSend(qh_battery, &bat, pdMS_TO_TICKS(0));
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief MPU6050 rollover sensor task
 * 
 * @param arg void arg
 */
void task_rollover(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_send;
    float roll_last;
    byte status;
    sensor_t rollover = {BOARDID, ROLLOVER, 0.0};
    sensor_t tilt_x = {BOARDID, TILT_X, 0.0};
    sensor_t tilt_y = {BOARDID, TILT_Y, 0.0};
    sensor_t tilt_z = {BOARDID, TILT_Z, 0.0};
    MPU6050 mpu(Wire, MPU_ADDR);


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

    xSemaphoreTake(sh_i2c, portMAX_DELAY);    
    // config MPU
    status = mpu.begin(MPU_GYRO_CONFIG, MPU_ACC_CONFIG);
    while(status!=0){ 
        INFO("INFO_1: erro initializating MPU6050");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    } // stop everything if could not connect to MPU6050

    mpu.setGyroOffsets(gyro_offset[0], gyro_offset[1], gyro_offset[2]);
    mpu.setAccOffsets(acc_offset[0], acc_offset[1], acc_offset[2]);

    xSemaphoreGive(sh_i2c);

    // -----update timer-----
    timer_send = millis();

    while (true){
        // -----update mpu-----
        xSemaphoreTake(sh_i2c, portMAX_DELAY);
        mpu.update();

        // read data
        if ((millis() - timer_send) >= TASK_ROLLOVER_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_ROLLOVER_RATE_ms;

            // correct the assembly mounting
            tilt_x.value = -1*mpu.getAngleY();
            tilt_y.value = mpu.getAngleX();
            tilt_z.value = mpu.getAngleZ();

            if ((abs(tilt_x.value) > 45) || (abs(tilt_y.value) > 45))
                rollover.value = 1.0;
            else
                rollover.value = 0.0;

            if (rollover.value != roll_last)
                esp_now_send(address_ECUBOX, (uint8_t *) &rollover, sizeof(rollover));
            
            roll_last = rollover.value;

            // -----send tilt data through queue-----
            xQueueSend(qh_tilt_x, &tilt_x, pdMS_TO_TICKS(0));
            xQueueSend(qh_tilt_y, &tilt_y, pdMS_TO_TICKS(0)); 
            xQueueSend(qh_tilt_z, &tilt_z, pdMS_TO_TICKS(0)); 
        }
        xSemaphoreGive(sh_i2c);
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
    sensor_t recv_bat = {BOARDID, BATTERY, 0.0};

    // xSemaphoreTake(sh_i2c, portMAX_DELAY);
        display_setup(); 
        display_sponsors();
    // xSemaphoreGive(sh_i2c);

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
        if (xQueueReceive(qh_battery, &recv_bat, pdMS_TO_TICKS(1))){
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.rpm = recv_bat.value;
            xSemaphoreGive(sh_global_vars);

            // xSemaphoreTake(sh_i2c, portMAX_DELAY);
            display_battery(recv_bat.value);
            // xSemaphoreGive(sh_i2c);
        }
        // if (xQueueReceive(qh_sensor_2, &recv_sensor2, pdMS_TO_TICKS(1))){
        //     // display function simulation
        //     Serial.print("sensor2:");
        //     Serial.println(recv_sensor2.value);

        //     xSemaphoreTake(sh_global_vars, portMAX_DELAY);
        //         system_global.sensor_2 = recv_sensor2.value;
        //     xSemaphoreGive(sh_global_vars);
        // }
        // if (xQueueReceive(qh_sensor_3, &recv_sensor3, pdMS_TO_TICKS(1))){
        //     // display function simulation
        //     Serial.print("sensor3:");
        //     Serial.println(recv_sensor3.value);

        //     xSemaphoreTake(sh_global_vars, portMAX_DELAY);
        //         system_global.sensor_3 = recv_sensor3.value;
        //     xSemaphoreGive(sh_global_vars);
        // }
        // if (xQueueReceive(qh_sensor_4, &recv_sensor4, pdMS_TO_TICKS(1))){
        //     // display function simulation
        //     Serial.print("sensor4:");
        //     Serial.println(recv_sensor4.value);

        //     xSemaphoreTake(sh_global_vars, portMAX_DELAY);
        //         system_global.sensor_4 = recv_sensor4.value;
        //     xSemaphoreGive(sh_global_vars);
        // }
        
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
            esp_now_send(address_ECUBOX, (uint8_t *) &system_copy, sizeof(system_copy));
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}