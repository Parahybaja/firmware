#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define R1 10375.0
#define R2 2085.0

// alive signal pin
#define PIN_LED_ALIVE 12

const int potPin = 35;

int potValue = 0;
float voltage_read = 0;
float voltage_battery = 0;

void task_alive(void *arg){
    (void)arg;
    
    while (true){
        // alive signal
        digitalWrite(PIN_LED_ALIVE, HIGH);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        digitalWrite(PIN_LED_ALIVE, LOW);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    pinMode(PIN_LED_ALIVE, OUTPUT);
    Serial.begin(115200);
    xTaskCreate(task_alive,     // task function
            "alive signal", // task name
            1024,           // stack size
            NULL,           // parameters
            10,             // priority
            NULL);          // handler 
}

void loop() {
    // Reading potentiometer value
    potValue = analogRead(potPin);
    voltage_read = ((3.3*potValue)/4095);
    voltage_battery = ((voltage_read)/(R2/(R1+R2)));
    Serial.println(voltage_battery);
    delay(500);
}

