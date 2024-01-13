#include <stdio.h>

#include "system.h"
#include "task_example.h"

static const char *TAG = "test-dir";

void app_main(void) {

    // -----fire up tasks-----
    xTaskCreatePinnedToCore(
        task_example,      // task function
        "task example", // task name
        2048,              // stack size
        NULL,              // parameters
        10,                // priority
        &th_example,       // handler 
        APP_CPU_NUM        // core number
    );
}
