#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Wire.h>
 
// alive signal pin
#define PIN_LED_ALIVE 12

void task_alive(void *arg){
    (void)arg;
    
    while (true){
        // alive signal
        Serial.println("alive");
        digitalWrite(PIN_LED_ALIVE, HIGH);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        digitalWrite(PIN_LED_ALIVE, LOW);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    pinMode(PIN_LED_ALIVE, OUTPUT);

    Wire.begin();
    Serial.begin(115200);

    xTaskCreate(task_alive,     // task function
                "alive signal", // task name
                1024,           // stack size
                NULL,           // parameters
                10,             // priority
                NULL);          // handler 

    Serial.println();
    Serial.println("|---------------------------|");
    Serial.println("|                           |");
    Serial.println("|        I2C scanner        |");
    Serial.println("|                           |");
    Serial.println("|---------------------------|");
    Serial.println();
}
 
void loop() {
    byte error, address;
    int nDevices;
    Serial.println("Scanning...");
    nDevices = 0;
    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
        Serial.print("I2C device found at address 0x");
        if (address < 16) {
            Serial.print("0");
        }
        Serial.println(address, HEX);
        nDevices++;
        }
        else if (error == 4) {
        Serial.print("Unknow error at address 0x");
        if (address < 16) {
            Serial.print("0");
        }
        Serial.println(address, HEX);
        }    
    }
    if (nDevices == 0) {
        Serial.println("No I2C devices found\n");
    }
    else {
        Serial.println("done\n");
    }
    delay(5000);          
}