/**
 * @brief init espnow configs
 * 
 */
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

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

    esp_now_register_send_cb(OnDataSent);
    
    esp_now_add_peer(address_sender, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
    esp_now_add_peer(address_receiver, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
    
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