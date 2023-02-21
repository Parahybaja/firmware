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

/**
 * GYRO_CONFIG_[0,1,2,3] range = +- [250, 500,1000,2000] °/s
 *                       sensi =    [131,65.5,32.8,16.4] bit/(°/s)
 *
 * ACC_CONFIG_[0,1,2,3] range = +- [    2,   4,   8,  16] times the gravity (9.81m/s²)
 *                      sensi =    [16384,8192,4096,2048] bit/gravity
 */

// -----pinout-----
#define PIN_LED_ALIVE 12

// -----MPU6050 configs-----
#define MPU_ADDR        0x68
#define MPU_GYRO_CONFIG 3
#define MPU_ACC_CONFIG  3

MPU6050 mpu(Wire, MPU_ADDR);

uint32_t timer = false;
float gyro_offset[3], acc_offset[3]; // [0]=x, [1]=y, [2]=z

/**
 * @brief alive signal
 * 
 * @param arg void arg
 */
void task_alive(void *arg){
    (void)arg;
    
    pinMode(PIN_LED_ALIVE, OUTPUT);

    while (true){
        digitalWrite(PIN_LED_ALIVE, HIGH);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        digitalWrite(PIN_LED_ALIVE, LOW);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup() {
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

    byte status = mpu.begin(MPU_GYRO_CONFIG, MPU_ACC_CONFIG);
    Serial.print(F("MPU6050 status: "));
    Serial.println(status);
    while(status!=0){ } // stop everything if could not connect to MPU6050
    
    Serial.println(F("Calculating offsets, do not move MPU6050"));
    delay(1000);
    mpu.calcOffsets(true, true); // gyro and acc

    gyro_offset[0] = mpu.getGyroXoffset();
    gyro_offset[1] = mpu.getGyroYoffset();
    gyro_offset[2] = mpu.getGyroZoffset();

    acc_offset[0] = mpu.getAccXoffset();
    acc_offset[1] = mpu.getAccYoffset();
    acc_offset[2] = mpu.getAccZoffset();

    Serial.printf("Gyro offset: x=%f, y=%f, z=%f\n", gyro_offset[0], gyro_offset[1], gyro_offset[2]);
    Serial.printf("Acc offset: x=%f, y=%f, z=%f\n", acc_offset[0], acc_offset[1], acc_offset[2]);

    mpu.setGyroOffsets(gyro_offset[0], gyro_offset[1], gyro_offset[2]);
    mpu.setAccOffsets(acc_offset[0], acc_offset[1], acc_offset[2]);

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
