#include "task/display.h"

static const char *TAG = "display";

nextion_t *nextion_handle;  // Declare nextion_handle globally

static TaskHandle_t task_handle_user_interface;

void task_display(void *arg){
    (void)arg;
   
    nextion_handle = nextion_driver_install( 
        UART_NUM_2,
        115200,
        GPIO_NUM_17,
        GPIO_NUM_16
    );

    /*Do basic configuration*/
    nextion_init(nextion_handle);

    // Start a task that will handle touch notifications.
    xTaskCreate(
        process_callback_queue,
        "user_interface",
        2048,
        NULL,
        5,
        &task_handle_user_interface
    );

    /*Set a callback for touch events.*/ 
    nextion_event_callback_set_on_touch(
        nextion_handle,
        callback_touch_event
    ); 

    /* display initialization routine */
    nextion_page_set(nextion_handle, "page0");
    vTaskDelay(pdMS_TO_TICKS(1000)); 
    nextion_page_set(nextion_handle, "page1");

    print_task_remaining_space();

    vTaskDelay(pdMS_TO_TICKS(500));

    for (;;) {
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void callback_touch_event(nextion_on_touch_event_t event) {
    ESP_LOGI(TAG, "page:%i, comp:%i", event.page_id, event.component_id);

    if (event.page_id == 0 && event.component_id == 1 && event.state == NEXTION_TOUCH_PRESSED) {
        ESP_LOGI(TAG, "button1 pressed");
        
        xTaskNotify(
            task_handle_user_interface,
            event.component_id,
            eSetValueWithOverwrite
        );
    }
    else if (event.page_id == 1 && event.component_id == 3 && event.state == NEXTION_TOUCH_PRESSED) {
        ESP_LOGI(TAG, "button2 pressed");
        
        xTaskNotify(
            task_handle_user_interface,
            event.component_id,
            eSetValueWithOverwrite
        );
    }
}

[[noreturn]] static void process_callback_queue(void *arg) {
    const uint8_t MAX_TEXT_LENGTH = 50;
    char text_buffer[MAX_TEXT_LENGTH];
    size_t text_length = MAX_TEXT_LENGTH;
    int32_t number;
    uint32_t component_id;

    for (;;) {
        component_id = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (component_id == 1) {
            nextion_page_set(nextion_handle, "page1");
        }
        else if (component_id == 3) {
            nextion_page_set(nextion_handle, "page0");
        }

        ESP_LOGI(TAG, "received task notify");
    }
}