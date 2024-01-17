#include "task/task_example.h"

static const char *TAG = "task_example";

void task_example(void *arg){
    (void)arg;

    // create task variables

    // show remaining task space
    task_remaining_space();

    for (;;) {
        // do something
        ESP_LOGW(TAG, "do some work");

        // some delay
        vTaskDelay(pdMS_TO_TICKS(500));

        vTaskDelay(pdMS_TO_TICKS(1)); // free up the processor
    }
}
