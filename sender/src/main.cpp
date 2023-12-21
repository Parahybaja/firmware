#include <Arduino.h>

#include <WiFi.h>
#include <esp_now.h>

uint8_t receiverMacAddress[] = {0xC8, 0xF0, 0x9E, 0x31, 0x8C, 0x10};

int counter = 0;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    else {
        Serial.println("ESPNOW initialized");
    }

    esp_now_register_send_cb(OnDataSent);

    esp_now_peer_info_t receiverPeerInfo = {};
    memcpy(receiverPeerInfo.peer_addr, receiverMacAddress, 6);
    receiverPeerInfo.channel = 0;
    receiverPeerInfo.encrypt = false;
    if (esp_now_add_peer(&receiverPeerInfo) != ESP_OK) {
        Serial.println("Failed to add receiver peer");
        return;
    }
}

void loop() {
    esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t*)&counter, sizeof(counter));
    
    counter++;
    
    if (result == ESP_OK) {
        Serial.println("Sent with success\n");
    }
    else {
        Serial.println("Error sending the data\n");
    }
    
    delay(1000);
}