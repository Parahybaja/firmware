#include <stdio.h>
#include "task_example.h"
#include "system.h"

static const char *TAG = "task_example";

// -----FreeRTOS objects-----
TaskHandle_t th_example;

void task_example(void *arg){
    (void)arg;

    // create task variables

    // show remaining task space

    for (;;) {
        // do something
        ESP_LOGI(TAG, "so some work");

        // some delay
        vTaskDelay(pdMS_TO_TICKS(500));

        vTaskDelay(pdMS_TO_TICKS(1)); // free up the processor
    }
}
