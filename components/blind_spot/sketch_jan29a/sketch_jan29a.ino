#include <Arduino.h>

#define RX_PIN 3   // GPIO3 (RX0) como RX (pino de recepção)
#define TX_PIN 1   // GPIO1 como TX (pino de transmissão)
#define SERIAL_BAUD 9600   // Taxa de baud para comunicação serial

void setup() {
  Serial.begin(SERIAL_BAUD);
}

void loop() {
  getDistance();
  delay(100);  // Adiciona um pequeno atraso entre leituras
}

void getDistance() {
  unsigned int distance;
  byte startByte, h_data, l_data, sum = 0;
  byte buf[3];

  // Espera até que pelo menos 3 bytes estejam disponíveis
  while (Serial.available() < 3) {
    delay(10);
  }

  Serial.readBytes(buf, 3);
  startByte = buf[0];
  h_data = buf[1];
  l_data = buf[2];
  sum = h_data + l_data;

  if (startByte == 255 && sum == buf[2]) {
    distance = (h_data << 8) + l_data;
    Serial.print("Distância [mm]: ");
    Serial.println(distance);
  } else {
    Serial.println("Resultado inválido");
  }
}