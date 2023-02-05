/**
 * @brief init esp-now
 */
void init_espnow(){    
    // -----ESPNOW settings-----
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_now_init() != ESP_OK)    // check if was initialized successfully
        ERROR("ERROR_2: initializing ESP-NOW", false); // DO NOT send through esp-now
    INFO("INFO_2: Done initializing ESP-NOW", false); // DO NOT send through esp-now
    delay(50); // give time to send the espnow message
    esp_now_register_send_cb(OnDataSent);

    // register peer
    esp_now_peer_info_t peerInfo = {};
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    // control peer
    memcpy(peerInfo.peer_addr, address_ECUBOX, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("ERROR_2: Failed to add ECUBOX peer", false); // DO NOT send through esp-now
    INFO("INFO_2: control peer added");
    delay(50); // give time to send the espnow message

    // receiver peer
    memcpy(peerInfo.peer_addr, address_ECU1, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        ERROR("ERROR_2: Failed to add ECU1 peer", false); // DO NOT send through esp-now
    INFO("INFO_2: receiver peer added");
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
    if (len == sizeof(cmd_t)){
        cmd_t config = {};
        memcpy(&config, incomingData, sizeof(config));

        if (config.command == CMD_START){
            INFO("INFO_2: start writing data");
            delay(50); // give time to send the espnow message
        }
        else if (config.command == CMD_STOP){
            INFO("INFO_2: stop writing data");
            delay(50); // give time to send the espnow message
        }
        else if (config.command == CMD_RESTART){
            INFO("INFO_2: restarting . . .\n----------------------");
            delay(50); // give time to send the espnow message

            ESP.restart();
        }
        else{
            INFO("INFO_2: command does not exist");
            delay(50); // give time to send the espnow message
        }
    }
    else{
        INFO("INFO_2: unknown incoming data");
    }
}