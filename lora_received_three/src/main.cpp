#include <SPI.h>
#include <LoRa.h>

#define ss 5
#define rst 14
#define dio0 2

int counter = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(866E6)) {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      int receivedValue = LoRa.parseInt();
      Serial.print("Received LoRa packet: ");
      Serial.println(receivedValue);
    }
  }

  delay(10000);
}