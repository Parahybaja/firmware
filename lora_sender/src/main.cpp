#include <Arduino.h>

#include <WiFi.h>
#include <esp_now.h>
#include <LoRa.h>
#include <SPI.h>  // include libraries

uint8_t senderMacAddress[] = {0xC8, 0xF0, 0x9E, 0x31, 0x8C, 0x10};

const long frequency = 915E6; // LoRa Frequency

const int csPin = 5;    // LoRa radio chip select
const int resetPin = 14;  // LoRa radio reset
const int irqPin = 2;    // change for your board; must be a hardware interrupt pin

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    int recv;
    memcpy(&recv, incomingData, sizeof(recv));
    Serial.print("received: ");
    Serial.println(recv);

    LoRa.beginPacket();
    LoRa.print(recv);
    LoRa.endPacket();

    Serial.println("lora send");
}

void setup() {
    Serial.begin(115200);  // initialize serial
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

    esp_now_peer_info_t receiverPeerInfo = {};
    memcpy(receiverPeerInfo.peer_addr, senderMacAddress, 6);
    receiverPeerInfo.channel = 0;
    receiverPeerInfo.encrypt = false;
    if (esp_now_add_peer(&receiverPeerInfo) != ESP_OK) {
        Serial.println("Failed to add receiver peer");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);

    LoRa.setPins(csPin, resetPin, irqPin);

    if (!LoRa.begin(frequency)) {
        Serial.println("LoRa init failed. Check your connections.");
        while (true)
            ;  // if failed, do nothing
    }
    else {
        Serial.println("LoRa initialized");
    }
}

void loop() {
    LoRa.beginPacket();
    LoRa.print("hello");
    LoRa.endPacket();

    delay(1000);
}
