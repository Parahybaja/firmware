/**
 * @brief RPM task
 * 
 * @param arg void arg
 */
void task_RPM(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_read, timer_send;
    uint16_t buffer_count = 0;
    float sum;
    sensor_t sensor1 = {BOARDID, SENSOR_01, 0.0};

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_2: %d bytes remaining of the sensor 1 task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----update timer-----
    timer_read = timer_send = millis();

    while (true){
        if ((millis() - timer_read) >= TASK1_READING_RATE_ms){
            // -----add to timer-----
            timer_read += TASK1_READING_RATE_ms;

            // -----read sensors-----
            sum += analogRead(PIN_EXAMPLE1) * 1,234;

            // -----update buffer-----
            buffer_count++;
        }

        if ((millis() - timer_send) >= TASK1_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK1_SEND_RATE_ms;

            // -----calculate average-----
            sensor1.value = float(sum / buffer_count);

            // -----clear buffer-----
            buffer_count = false;

            // -----send sensor 1 data through esp-now to receiver-----
            esp_now_send(address_receiver, (uint8_t *) &sensor1, sizeof(sensor1));
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief sensor 2 task example
 * 
 * @param arg void arg
 */
void task_sensor_2(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_read, timer_send;
    uint16_t buffer_count = 0;
    float sum;
    sensor_t sensor2 = {BOARDID, SENSOR_02, 0.0};

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_2: %d bytes remaining of the sensor 2 task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // -----update timer-----
    timer_read = timer_send = millis();

    while (true){
        if ((millis() - timer_read) >= TASK2_READING_RATE_ms){
            // -----add to timer-----
            timer_read += TASK2_READING_RATE_ms;

            // -----read sensors-----
            sum += analogRead(PIN_EXAMPLE2) * 4,321;

            // -----update buffer-----
            buffer_count++;
        }

        if ((millis() - timer_send) >= TASK2_SEND_RATE_ms){
            // -----add to timer-----
            timer_send += TASK2_SEND_RATE_ms;

            // -----calculate average-----
            sensor2.value = float(sum / buffer_count);

            // -----clear buffer-----
            buffer_count = false;

            // -----send sensor 2 data through esp-now to receiver-----
            esp_now_send(address_receiver, (uint8_t *) &sensor2, sizeof(sensor2));
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
