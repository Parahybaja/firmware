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
 * @brief RPM interrupt service routine
 * 
 */
void IRAM_ATTR isr_rpm(void){ counter_rpm++; }

/**
 * @brief RPM task
 * 
 * @param arg void arg
 */
void task_RPM(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_send;
    uint16_t counter_rpm_copy;
    sensor_t rpm = {BOARDID, RPM, 0.0};

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_2: %d bytes remaining of the RPM task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----update timer-----
    timer_send = millis();

    while (true){
        if ((millis() - timer_send) >= TASK_RPM_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_RPM_SEND_RATE_ms;

            // -----get counts-----
            noInterrupts();
            counter_rpm_copy = counter_rpm;
            counter_rpm = false;
            interrupts();

            // -----calculate-----
            rpm.value = counter_rpm_copy / TASK_RPM_SEND_RATE_min; // general rpm calculation
            rpm.value /= 2; // compensate for the double peak of the signal

            // -----send RPM data through esp-now to receiver-----
            esp_now_send(address_receiver, (uint8_t *) &rpm, sizeof(rpm));
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief speedometer interrupt service routine
 * 
 */
void IRAM_ATTR isr_spdmt(void){ counter_spdmt++; }

/**
 * @brief  Read, calculate and send the speed
 * 
 * @param arg void arg
 */
void task_speedometer(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_send;
    uint16_t counter_spdmt_copy;
    sensor_t spdmt = {BOARDID, SPEEDOMETER, 0.0};

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_2: %d bytes remaining of the speedometer task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----attachISR-----
    pinMode(PIN_SPEEDOMETER, INPUT);
    attachInterrupt(PIN_SPEEDOMETER, isr_spdmt, CHANGE);

    // -----update timer-----
    timer_send = millis();

    while (true){
        if ((millis() - timer_send) >= TASK_SPDMT_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_SPDMT_SEND_RATE_ms;

            // -----get counts-----
            noInterrupts();
            counter_spdmt_copy = counter_spdmt;
            counter_spdmt = false;
            interrupts();

            // -----calculate-----
            float meters = (counter_spdmt_copy / WHEEL_EDGES) * WHEEL_CIRC;
            spdmt.value = meters / TASK_SPDMT_SEND_RATE_s; // in meter/second
            spdmt.value *= ms2kmh;

            // -----send spped data through esp-now to receiver-----
            esp_now_send(address_receiver, (uint8_t *) &spdmt, sizeof(spdmt));
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Read and send fuel emergency sensor data
 * 
 * @param arg void arg
 */
void task_fuel_emer(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_send;
    sensor_t fuel_emer = {BOARDID, FUEL_EMERGENCY, 0.0};

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_2: %d bytes remaining of the fuel emer task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----pin mode-----
    pinMode(PIN_FUEL_EMERGENCY, INPUT);

    // -----update timer-----
    timer_send = millis();

    while (true){
        if ((millis() - timer_send) >= TASK_F_EMER_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_F_EMER_SEND_RATE_ms;

            // -----read raw data-----
            fuel_emer.value = float(!digitalRead(PIN_FUEL_EMERGENCY));

            // -----send spped data through esp-now to receiver-----
            esp_now_send(address_receiver, (uint8_t *) &fuel_emer, sizeof(fuel_emer));
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
