#ifndef __ROLLOVER_H__
#define __ROLLOVER_H__

#include <TwoMPU6050.h>
#include "Wire.h"
#include "communication.h"

// -----MPU configs-----
#define MPU_ADDR        0x68
#define MPU_GYRO_CONFIG 3
#define MPU_ACC_CONFIG  3
/**
 * GYRO_CONFIG_[0,1,2,3] range = +- [250, 500,1000,2000] °/s
 *                       sensi =    [131,65.5,32.8,16.4] bit/(°/s)
 *
 *  ACC_CONFIG_[0,1,2,3] range = +- [    2,   4,   8,  16] times the gravity (9.81m/s²)
 *                       sensi =    [16384,8192,4096,2048] bit/gravity
 */

// -----MPU6050 configs-----
float gyro_offset[] = {0.0, 0.0, 0.0};
float acc_offset[]  = {0.0, 0.0, 0.0};

void task_rollover(void*);

/**
 * @brief MPU6050 rollover sensor task
 * 
 * @param arg void arg
 */
void task_rollover(void *arg){
    (void)arg;

    // -----create local variables-----
    uint32_t timer_send;
    float roll_last;
    byte status;
    sensor_t rollover = {ROLLOVER, 0.0};
    sensor_t tilt_x = {TILT_X, 0.0};
    sensor_t tilt_y = {TILT_Y, 0.0};
    sensor_t tilt_z = {TILT_Z, 0.0};
    MPU6050 mpu(Wire, MPU_ADDR);


#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_1: %d bytes remaining of the sensor 4 task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    xSemaphoreTake(sh_i2c, portMAX_DELAY);    
    // config MPU
    status = mpu.begin(MPU_GYRO_CONFIG, MPU_ACC_CONFIG);
    while(status!=0){ 
        INFO("INFO_1: erro initializating MPU6050");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    } // stop everything if could not connect to MPU6050

    mpu.setGyroOffsets(gyro_offset[0], gyro_offset[1], gyro_offset[2]);
    mpu.setAccOffsets(acc_offset[0], acc_offset[1], acc_offset[2]);

    xSemaphoreGive(sh_i2c);

    // -----update timer-----
    timer_send = millis();

    while (true){
        // -----update mpu-----
        xSemaphoreTake(sh_i2c, portMAX_DELAY);
        mpu.update();

        // read data
        if ((millis() - timer_send) >= TASK_ROLLOVER_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_ROLLOVER_RATE_ms;

            // correct the assembly mounting
            tilt_x.value = -1*mpu.getAngleY();
            tilt_y.value = mpu.getAngleX();
            tilt_z.value = mpu.getAngleZ();

            if ((abs(tilt_x.value) > 45) || (abs(tilt_y.value) > 45))
                rollover.value = 1.0;
            else
                rollover.value = 0.0;

            if (rollover.value != roll_last)
                esp_now_send(address_ECUBOX, (uint8_t *) &rollover, sizeof(rollover));
            
            roll_last = rollover.value;

            // -----send tilt data through queue-----
            xQueueSend(qh_tilt_x, &tilt_x, pdMS_TO_TICKS(0));
            xQueueSend(qh_tilt_y, &tilt_y, pdMS_TO_TICKS(0)); 
            xQueueSend(qh_tilt_z, &tilt_z, pdMS_TO_TICKS(0)); 
        }
        xSemaphoreGive(sh_i2c);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

#endif // __ROLLOVER_H__