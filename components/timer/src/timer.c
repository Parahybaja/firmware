#include "task/timer.h"

static const char *TAG = "timer";

void task_timer(void *arg){
    float start_time = esp_timer_get_time()/1000000;
    
    sensor_t hours = {
        .type = HOURS,
        .value = 0.0
    };

    sensor_t minutes = {
        .type = MINUTES,
        .value = 0.0
    };

    sensor_t seconds = {
        .type = SECONDS,
        .value = 0.0
    };

    
    for (;;) {
        float current_time = esp_timer_get_time()/1000000;

        int time = (int) (current_time - start_time);
        int remaining_time = (4 * 60 * 60) - time;

        hours.value = (float) (remaining_time / 3600);
        minutes.value = (remaining_time / 60) - (hours.value * 60);
        seconds.value = remaining_time - (minutes.value * 60) - (hours.value * 3600);
        
        xQueueSend(qh_hours, &hours, pdMS_TO_TICKS(0));
        xQueueSend(qh_minutes, &minutes, pdMS_TO_TICKS(0)); 
        xQueueSend(qh_seconds, &seconds, pdMS_TO_TICKS(0));

        ESP_LOGI(TAG, "send timer, hours: %f, minutes: %f, seconds: %f", hours.value, minutes.value, seconds.value);

        vTaskDelay(pdMS_TO_TICKS(1000)); // free up the processor
    }
}
