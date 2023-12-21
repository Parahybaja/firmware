#include <esp_now.h>
#include <WiFi.h>

uint8_t receiverMacAddress[] = {0xC8,0xF0,0x9E,0x31,0x92,0xE8};
int counter = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t receiverPeerInfo;
  memcpy(receiverPeerInfo.peer_addr, receiverMacAddress, 6);
  receiverPeerInfo.channel = 0;
  receiverPeerInfo.encrypt = false;
  if (esp_now_add_peer(&receiverPeerInfo) != ESP_OK) {
    Serial.println("Failed to add receiver peer");
    return;
  }
}

void loop() {
  esp_now_send(receiverMacAddress, (uint8_t*)&counter, sizeof(counter));
  counter++;
  delay(10000);
}