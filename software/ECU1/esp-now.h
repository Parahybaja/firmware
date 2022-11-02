/**
 * @brief init esp-now
 */
void init_espnow(){    
    // -----ESPNOW settings-----
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_now_init() != ESP_OK)    // check if was initialized successfully
        ERROR("ERROR_1: initializing ESP-NOW", false); // DO NOT send through esp-now
    INFO("INFO_1: Done initializing ESP-NOW", false); // DO NOT send through esp-now
    delay(50); // give time to send the espnow message
    esp_now_register_send_cb(OnDataSent);

    // register peer
    esp_now_peer_info_t peerInfo = {};
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    // control peer
    memcpy(peerInfo.peer_addr, address_control, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("ERROR_1: Failed to add control peer", false); // DO NOT send through esp-now
    INFO("INFO_1: control peer added");
    delay(50); // give time to send the espnow message

    // sender peer
    memcpy(peerInfo.peer_addr, address_sender, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("ERROR_1: Failed to add sender peer", false); // DO NOT send through esp-now
    INFO("INFO_1: sender peer added");
    delay(50); // give time to send the espnow message

    esp_now_register_recv_cb(OnDataRecv);
}

/**
 * @brief ESP-NOW on data sent callback
 * 
 * @param mac_addr device MAC address
 * @param status esp-now status
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
#if DEBUG_MODE
    // -----check if the data was delivered-----
    Serial.print("Send status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
#endif
}

/**
 * @brief ESP-NOW on data receive callback
 * 
 * @param mac device MAC address
 * @param incomingData data received
 * @param len array size 
 */
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len){
    if (len ==  sizeof(cmd_t)){
        cmd_t config = {};
        memcpy(&config, incomingData, sizeof(config));

        if (config.command == CMD_START){
            INFO("INFO_1: start writing data");
            delay(50); // give time to send the espnow message
        }
        else if (config.command == CMD_STOP){
            INFO("INFO_1: stop writing data");
            delay(50); // give time to send the espnow message
        }
        else if (config.command == CMD_NEW_FILE){ 
            INFO("INFO_1: new file created");
            delay(50); // give time to send the espnow message
        }
        else if (config.command == CMD_RESTART){
            INFO("INFO_1: restarting . . .\n----------------------");
            delay(50); // give time to send the espnow message

            ESP.restart();
        }
        else{
            INFO("INFO_1: command does not exist");
            delay(50); // give time to send the espnow message
        }
    }
    else if (len ==  sizeof(sensor_t)){
        sensor_t sensor = {};
        memcpy(&sensor, incomingData, sizeof(sensor));

        // if (sensor.type == SENSOR_01){
        //     // -----send sensor 1 data through queue-----
        //     xQueueSend(qh_sensor_1, &sensor, pdMS_TO_TICKS(0));
        // }
        // else if (sensor.type == SENSOR_02){
        //     // -----send sensor 2 data through queue-----
        //     xQueueSend(qh_sensor_2, &sensor, pdMS_TO_TICKS(0));
        // }
        // else {
        //     INFO("INFO_1: unknown sensor type");
        // }
    }
    else {
        INFO("INFO_1: unknown incoming data");
    }
}