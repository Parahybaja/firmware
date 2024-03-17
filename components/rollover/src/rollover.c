#include "task/rollover.h"

static const char *TAG = "task_rollover";

void task_rollover(void *arg){
    (void)arg;

    // -----create local variables-----
    const int send_rate_ms = (int)(1000.0 / (float)(TASK_ROLLOVER_RATE_Hz));
    uint32_t timer_send_ms;
    float roll_last = 0.0;

    sensor_t rollover = {
        .type = ROLLOVER, 
        .value = 0.0
    };

    sensor_t tilt_x = {
        .type = TILT_X, 
        .value = 0.0
    };

    sensor_t tilt_y = {
        .type = TILT_Y, 
        .value = 0.0
    };

    sensor_t tilt_z = {
        .type = TILT_Z, 
        .value = 0.0
    };

    sensor_t temp = {
        .type = AMBIENT_TEMP, 
        .value = 0.0
    };

    // show remaining task space
    print_task_remaining_space();
 
    // config MPU
    esp_err_t ret = mpu_init();

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error initializing");
        ESP_LOGW(TAG, "Deleting rollover task");
        vTaskDelete(NULL);
    }

    if (CALIBRATE == true) {
        ESP_LOGW(TAG, "Calculating offsets, do not move MPU6050");
        vTaskDelay(pdMS_TO_TICKS(500));
        
        /*call offset function*/
    }
    else {
        ESP_LOGW(TAG, "Setting offsets");
        
        // mpu.setAccOffsets(CALIB_ACC_X, CALIB_ACC_Y, CALIB_ACC_Z);
        // mpu.setGyroOffsets(CALIB_GYRO_X, CALIB_GYRO_Y, CALIB_GYRO_Z);
    }

    // log_d("offset acc: %f, %f, %f",
    //     mpu.getAccXoffset(),
    //     mpu.getAccYoffset(),
    //     mpu.getAccZoffset());

    // log_d("offset gyro: %f, %f, %f",
    //     mpu.getGyroXoffset(),
    //     mpu.getGyroYoffset(),
    //     mpu.getGyroZoffset());

    /*-----update timer-----*/
    timer_send_ms = esp_log_timestamp();

    while (true){
        /*-----get acc and gyro data and calculate the angles-----*/
        mpu_update();

        if ((esp_log_timestamp() - timer_send_ms) >= send_rate_ms){
            // -----add to timer-----
            timer_send_ms += send_rate_ms;

            // correct the assembly mounting
            tilt_x.value = -1*mpu_get_angle_x();
            tilt_y.value = -1*mpu_get_angle_y();
            tilt_z.value = mpu_get_angle_z();
            temp.value   = mpu_get_temp();

            if ((fabs(tilt_x.value) > 45) || (fabs(tilt_y.value) > 45))
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

            ESP_LOGI(TAG, "send rollover");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}