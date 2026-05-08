#include "task/rollover.h"
#include "system.h"
#include "esp_log.h"
#include <math.h>

static const char *TAG = "task_rollover";

void task_rollover(void *arg){
    
    const uint8_t calibrate = (uint8_t)(uintptr_t)arg;

    // -----create local variables-----
    const int send_rate_ms = (int)(1000.0 / (float)(TASK_ROLLOVER_RATE_Hz));
    uint32_t timer_send_ms;

    // show remaining task space
    print_task_remaining_space();
 
    // config MPU
    esp_err_t ret = mpu_init();

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error initializing");
        ESP_LOGW(TAG, "Deleting rollover task");
        vTaskDelete(NULL);
    }

    if (calibrate == true) {
        ESP_LOGW(TAG, "Calculating offsets, do not move MPU6050");
        vTaskDelay(pdMS_TO_TICKS(500));
        
        /*call offset function*/
        mpu_calc_offset(true, true);

        ESP_LOGW(TAG, "offset acc: %f, %f, %f",
            mpu_get_acc_x_offset(),
            mpu_get_acc_y_offset(),
            mpu_get_acc_z_offset()
        );

        ESP_LOGW(TAG, "offset gyro: %f, %f, %f",
            mpu_get_gyro_x_offset(),
            mpu_get_gyro_y_offset(),
            mpu_get_gyro_z_offset()
        );
    }
    else {
        ESP_LOGW(TAG, "Setting offsets");
        
        mpu_set_acc_offset(CALIB_ACC_X, CALIB_ACC_Y, CALIB_ACC_Z);
        mpu_set_gyro_offset(CALIB_GYRO_X, CALIB_GYRO_Y, CALIB_GYRO_Z);

        mpu_set_angle_offset(0, 50, 0);
    }

    /*-----update timer-----*/
    timer_send_ms = esp_log_timestamp();

    while (true){
        /*-----get acc and gyro data and calculate the angles-----*/
        mpu_update();

        if ((esp_log_timestamp() - timer_send_ms) >= send_rate_ms){
            // -----add to timer-----
            timer_send_ms += send_rate_ms;

            // correct the assembly mounting
            float calc_tilt_x = -1.0f * mpu_get_angle_x();
            float calc_tilt_y = -1.0f * mpu_get_angle_y();
            float calc_tilt_z = mpu_get_angle_z();
            float calc_temp   = mpu_get_temp();
            
            float calc_rollover = 0.0f;
            if ((fabs(calc_tilt_x) > 45) || (fabs(calc_tilt_y) > 45)) {
                calc_rollover = 1.0f;
            }

            // ==========================================
            // ATUALIZA O QUADRO DE AVISOS GLOBAL
            // ==========================================
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
            system_global.tilt_x   = calc_tilt_x;
            system_global.tilt_y   = calc_tilt_y;
            system_global.tilt_z   = calc_tilt_z;
            system_global.temp     = calc_temp;
            system_global.rollover = calc_rollover;
            xSemaphoreGive(sh_global_vars);
            // ==========================================

            ESP_LOGD(TAG, "rollover data updated in global struct");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}