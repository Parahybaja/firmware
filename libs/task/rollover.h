/**
 * @file task_rollover.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief accelerometer task, functions and configs
 * @version 1.2
 * @date 2023-10-26
 * 
 * @copyright Copyright (c) 2023
 * 
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  1.2.0    Jefferson L.  26/10/2023  add MPU calibration
 */

#ifndef __ROLLOVER_H__
#define __ROLLOVER_H__

#include <TwoMPU6050.h>
#include <Wire.h>

#include "system.h"

#define TASK_ROLLOVER_RATE_Hz 1 // reading rate in hertz (must be greater then send rate)
#define TASK_ROLLOVER_RATE_ms (int(1000.0 / float(TASK_ROLLOVER_RATE_Hz))) // rate perido in milliseconds

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

// -----MPU offset-----
#define CALIB_ACC_X 0.531088 // usada para definir o zero do eixo x
#define CALIB_ACC_Y 0.045418 // usada para definir o zero do eixo y
#define CALIB_ACC_Z -0.308320 // usada para definir o zero do eixo z
#define CALIB_GYRO_X -1.591225 // usada para definir o zero do eixo x
#define CALIB_GYRO_Y -0.218293 // usada para definir o zero do eixo y
#define CALIB_GYRO_Z 0.441586 // usada para definir o zero do eixo z
#define CALIBRATE false

/**
 * @brief MPU6050 rollover sensor task
 * 
 * @param arg void arg
 */
void task_rollover(void*);

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
    sensor_t temp   = {AMBIENT_TEMP, 0.0};
    MPU6050 mpu(Wire, MPU_ADDR);

    Wire.begin(5, 18); // hot fix for GPIO burnout

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "%d bytes remaining of the rollover task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif
 
    // config MPU
    status = mpu.begin(MPU_GYRO_CONFIG, MPU_ACC_CONFIG);
    while(status!=0){ 
        INFO("erro initializating MPU6050");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    } // stop everything if could not connect to MPU6050
    log_i("Connected to MPU6050");

    if (CALIBRATE) {
        log_w("Calculating offsets, do not move MPU6050");
        delay(1000);
        mpu.calcOffsets(true, true);  // gyro and accelero
        log_w("Done!");
    }
    else {
        log_w("Setting offsets");
        mpu.setAccOffsets(CALIB_ACC_X, CALIB_ACC_Y, CALIB_ACC_Z);
        mpu.setGyroOffsets(CALIB_GYRO_X, CALIB_GYRO_Y, CALIB_GYRO_Z);
    }

    log_d("offset acc: %f, %f, %f",
        mpu.getAccXoffset(),
        mpu.getAccYoffset(),
        mpu.getAccZoffset());

    log_d("offset gyro: %f, %f, %f",
        mpu.getGyroXoffset(),
        mpu.getGyroYoffset(),
        mpu.getGyroZoffset());

    // -----update timer-----
    timer_send = millis();

    while (true){
        // -----update mpu-----
        mpu.update();

        // read data
        if ((millis() - timer_send) >= TASK_ROLLOVER_RATE_ms){
            // -----add to timer-----
            timer_send += TASK_ROLLOVER_RATE_ms;

            // correct the assembly mounting
            tilt_x.value = -1*mpu.getAngleY();
            tilt_y.value = mpu.getAngleX();
            tilt_z.value = mpu.getAngleZ();
            temp.value   = mpu.getTemp();

            if ((abs(tilt_x.value) > 45) || (abs(tilt_y.value) > 45))
                rollover.value = 1.0;
            else
                rollover.value = 0.0;

            if (rollover.value != roll_last)
                xQueueSend(qh_rollover, &rollover, pdMS_TO_TICKS(0));
            
            roll_last = rollover.value;

            // -----send tilt data through queue-----
            xQueueSend(qh_tilt_x, &tilt_x, pdMS_TO_TICKS(0));
            xQueueSend(qh_tilt_y, &tilt_y, pdMS_TO_TICKS(0)); 
            xQueueSend(qh_temp, &temp, pdMS_TO_TICKS(0));
            // xQueueSend(qh_tilt_z, &tilt_z, pdMS_TO_TICKS(0)); 

            log_i("send rollover");
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

#endif // __ROLLOVER_H__