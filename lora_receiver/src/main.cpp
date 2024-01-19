#include <Arduino.h>

#include <LoRa.h>
#include <SPI.h>  // include libraries

const long frequency = 915E6; // LoRa Frequency

const int csPin = 5;    // LoRa radio chip select
const int resetPin = 14;  // LoRa radio reset
const int irqPin = 2;    // change for your board; must be a hardware interrupt pin

void setup() {
    Serial.begin(115200);  // initialize serial
    while (!Serial);

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
    // try to parse packet
    int packetSize = LoRa.parsePacket();

    if (packetSize) {
        // received a packet
        Serial.print("Received packet '");

        // read packet
        while (LoRa.available()) {
            Serial.print((char)LoRa.read());
        }

        // print RSSI of packet
        Serial.print("' with RSSI ");
        Serial.println(LoRa.packetRssi());
    }

    delay(1);
}
