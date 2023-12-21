#include <SPI.h>
#include <LoRa.h>
#include <esp_now.h>
#include <WiFi.h>

#define ss 5
#define rst 14
#define dio0 2

uint8_t senderMacAddress[] = {0xC8, 0xF0, 0x9E, 0x31, 0x87, 0xB8};
uint8_t loraReceiverMacAddress[] = {0xC8, 0xF0, 0x9E, 0x31, 0x8C, 0x10};
int counter = 0;

void onReceiveData(const uint8_t *mac, const uint8_t *data, int len) {
  Serial.println("Received data via ESP-NOW");
  // Handle the received data here
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(915E6)) {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t senderPeerInfo;
  memcpy(senderPeerInfo.peer_addr, senderMacAddress, 6);
  senderPeerInfo.channel = 0;
  senderPeerInfo.encrypt = false;
  if (esp_now_add_peer(&senderPeerInfo) != ESP_OK) {
    Serial.println("Failed to add sender peer");
    return;
  }

  esp_now_peer_info_t loraReceiverPeerInfo;
  memcpy(loraReceiverPeerInfo.peer_addr, loraReceiverMacAddress, 6);
  loraReceiverPeerInfo.channel = 0;
  loraReceiverPeerInfo.encrypt = false;
  if (esp_now_add_peer(&loraReceiverPeerInfo) != ESP_OK) {
    Serial.println("Failed to add LoRa receiver peer");
    return;
  }

  esp_now_register_recv_cb(onReceiveData);
}

void loop() {
  // Envia dados via ESP-NOW para o sender
  esp_now_send(senderMacAddress, (uint8_t*)&counter, sizeof(counter));

  // Verifica se há dados recebidos via LoRa
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      int receivedValue = LoRa.parseInt();
      Serial.print("Received LoRa packet: ");
      Serial.println(receivedValue);
    }
  }

  counter++;
  delay(10000);
}
