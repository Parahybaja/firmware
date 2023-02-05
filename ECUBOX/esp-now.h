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

    esp_now_register_send_cb(OnDataSent);

    // register peer
    esp_now_peer_info_t peerInfo = {};
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    memcpy(peerInfo.peer_addr, address_ECU1, 6);
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
      Serial.println("ERROR: Failed to add peer");
    Serial.println("INFO: Peer added");
    delay(50); // give time to send the espnow message
    
    esp_now_register_recv_cb(OnDataRecv);
}

/**
 * @brief ESP-NOW on data sent callback
 * 
 * @param mac_addr device MAC address
 * @param status esp-now status
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
#if DEBUG_MODE
    Serial.print("\r\nLast Packet Send Status: ");
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
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
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
