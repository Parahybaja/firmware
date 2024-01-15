#include "system.h"

static const char *TAG = "system";

// -----system run time data-----
system_t system_global = {
    .rpm = 0.0,
    .speed = 0.0,
    .fuel_em = 0.0,
    .battery = 0.0,
    .temp = 0.0,
    .rollover = 0.0,
    .tilt_x = 0.0,
    .tilt_y = 0.0,
    .tilt_z = 0.0
};

// -----FreeRTOS objects-----
TaskHandle_t th_example;
TaskHandle_t th_alive;

void task_remaining_space(void){
    TaskHandle_t handler = xTaskGetCurrentTaskHandle();

    uint32_t stackHighWaterMark = uxTaskGetStackHighWaterMark(handler);
    const char *task_name = pcTaskGetName(handler);

    ESP_LOGW(TAG, "task '%s' remaining stack space: %lu bytes", task_name, stackHighWaterMark);

    vTaskDelay(pdMS_TO_TICKS(50));
}
