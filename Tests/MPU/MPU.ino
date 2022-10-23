/* Get all possible data from MPU6050
 * Accelerometer values are given as multiple of the gravity [1g = 9.81 m/s²]
 * Gyro values are given in deg/s
 * Angles are given in degrees
 * Note that X and Y are tilt angles and not pitch/roll.
 *
 * License: MIT
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <TwoMPU6050.h>
#include "Wire.h"

// alive signal pin
#define PIN_LED_ALIVE 12

MPU6050 mpu(Wire, 0x68);

long timer = 0;

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
    Wire.begin();

    xTaskCreate(task_alive,     // task function
                "alive signal", // task name
                1024,           // stack size
                NULL,           // parameters
                10,             // priority
                NULL);          // handler 

    Serial.println();
    Serial.println("|----------------------------|");
    Serial.println("|                            |");
    Serial.println("|        MPU6050 test        |");
    Serial.println("|                            |");
    Serial.println("|----------------------------|");
    Serial.println();
    
    byte status = mpu.begin();
    Serial.print(F("MPU6050 status: "));
    Serial.println(status);
    while(status!=0){ } // stop everything if could not connect to MPU6050
    
    Serial.println(F("Calculating offsets, do not move MPU6050"));
    delay(1000);
    mpu.calcOffsets(true,true); // gyro and accelero
    Serial.println("Done!\n");
  
}

void loop() {
    mpu.update();

    if(millis() - timer > 1000){ // print data every second
        Serial.print(F("TEMPERATURE: "));Serial.println(mpu.getTemp());
        Serial.print(F("ACCELERO  X: "));Serial.print(mpu.getAccX());
        Serial.print("\tY: ");Serial.print(mpu.getAccY());
        Serial.print("\tZ: ");Serial.println(mpu.getAccZ());
    
        Serial.print(F("GYRO      X: "));Serial.print(mpu.getGyroX());
        Serial.print("\tY: ");Serial.print(mpu.getGyroY());
        Serial.print("\tZ: ");Serial.println(mpu.getGyroZ());
    
        Serial.print(F("ACC ANGLE X: "));Serial.print(mpu.getAccAngleX());
        Serial.print("\tY: ");Serial.println(mpu.getAccAngleY());
        
        Serial.print(F("ANGLE     X: "));Serial.print(mpu.getAngleX());
        Serial.print("\tY: ");Serial.print(mpu.getAngleY());
        Serial.print("\tZ: ");Serial.println(mpu.getAngleZ());
        Serial.println(F("=====================================================\n"));
        timer = millis();
    }
}
