#include "task/sd_logger.h"

static const char *TAG = "sd_logger";

void task_sdlogger(void *arg) {
    for(;;) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}