#include <esp_now.h>
#include <WiFi.h>

uint8_t address_master[] = {0x0C, 0xB8, 0x15, 0xC3, 0xF0, 0x50};
uint8_t address_slave[]  = {0x94, 0xB5, 0x55, 0x2D, 0x1E, 0x0C};

uint32_t timer;
uint16_t data_sent = 1998;

void setup(){
    // -----ESPNOW settings-----
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_now_init() != ESP_OK)    // check if was initialized successfully
        Serial.println("ERROR: initializing ESP-NOW");
    Serial.println("INFO: Done initializing ESP-NOW");
    esp_now_register_send_cb(OnDataSent);

    // register peer
    esp_now_peer_info_t peerInfo = {};
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    // slave peer
    memcpy(peerInfo.peer_addr, address_master, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
        Serial.println("ERROR: Failed to add control peer");
    Serial.println("INFO: control peer added");

    esp_now_register_recv_cb(OnDataRecv);

    timer = millis();
}

void loop(){

}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
#if DEBUG_MODE
    // -----check if the data was delivered-----
    Serial.print("Send status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
#endif
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len){
    if (len ==  sizeof(uint16_t)){
        uint16_t received;
        memcpy(&received, incomingData, sizeof(received));

        esp_now_send(address_master, (uint8_t *) &received, sizeof(received));
    }
}