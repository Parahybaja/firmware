void init_espnow(void){
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        ESP.restart();
        delay(1);
    }
    uint8_t receiverAddress[] = {0x5C, 0xCF, 0x7F, 0xB1, 0x2A, 0xE6};
    
    esp_now_register_send_cb(OnDataSent);
    
    // register peer
    esp_now_peer_info_t peerInfo;
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    // register first peer  
    memcpy(peerInfo.peer_addr, address_receiver, 6);
    
    esp_now_register_recv_cb(OnDataRecv); 
}

/**
 * @brief ESP-NOW on data sent callback
 * 
 * @param mac_addr device MAC address
 * @param status esp-now status
 */
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
#if DEBUG_MODE
    Serial.print("\r\nLast Packet Send Status: ");
    if (sendStatus == 0)
        Serial.println("Delivery success");
    else
        Serial.println("Delivery fail");
#endif
}

/**
 * @brief ESP-NOW on data receive callback
 * 
 * @param mac device MAC address
 * @param incomingData data received
 * @param len array size
 */
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
    #if DEBUG_MODE
        Serial.print("packet received size: ");
        Serial.println(len);
    #endif

    if (len == sizeof(debug_t)){
        debug_t debug;
        memcpy(&debug, incomingData, sizeof(debug));

        Serial.print("received: ");
        Serial.print(debug.msg);
        Serial.print("\n\n");
    }
    else if (len == sizeof(system_t)){
        system_t system;
        memcpy(&system, incomingData, sizeof(system));

        Serial.println('system data received');
    }
}

/* ESP-NOW on data sent callback */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
#if DEBUG_MODE
    // -----check if the data was delivered-----
    Serial.print("Send status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
#endif
}

/* ESP-NOW on data receive callback*/
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len){
    set_up config = {};
    memcpy(&config, incomingData, sizeof(config));

    if (config.command == CMD_START){
        xTaskCreatePinnedToCore(task_readData,         // task function
                                "readData",            // task name
                                2048,                  // stack size
                                NULL,                  // parameters
                                10,                    // priority
                                &TaskHandle_readData,  // handler  
                                APP_CPU_NUM);          // CPU core number

        xTaskCreatePinnedToCore(task_writeData, 
                                "writeData", 
                                8128, 
                                NULL, 
                                5, 
                                &TaskHandle_writeData, 
                                PRO_CPU_NUM);

        INFO("INFO: start writing data");
        delay(50); // give time to send the espnow message
    }
    else if (config.command == CMD_STOP){
        vTaskDelete(TaskHandle_readData);
        vTaskDelete(TaskHandle_writeData);

        INFO("INFO: stop writing data");
        delay(50); // give time to send the espnow message
    }
    else if (config.command == CMD_NEW_FILE){ 
        // -----create new file-----
        createFile();

        INFO("INFO: new file created");
        delay(50); // give time to send the espnow message
    }
    else if (config.command == CMD_RESTART){
        INFO("INFO: restarting . . .\n----------------------");
        delay(50); // give time to send the espnow message

        ESP.restart();
    }
    else{
        INFO("INFO: command does not exist");
        delay(50); // give time to send the espnow message
    }
}
