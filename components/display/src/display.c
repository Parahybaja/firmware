#include "task/display.h"

static const char *TAG = "display";

TaskHandle_t task_handle_user_interface;

nextion_t *nextion_handle;  // Declare nextion_handle globally

void task_display(void *arg){
    (void)arg;
   
    nextion_handle = nextion_driver_install( UART_NUM_2,
                                                       9600,
                                                       GPIO_NUM_17,
                                                       GPIO_NUM_16);


    /*Do basic configuration*/
    nextion_init (nextion_handle);

    /* Add a delay to ensure proper initialization */
    vTaskDelay(pdMS_TO_TICKS(5000));  // Adjust the delay as needed

    /*Set a callback for touch events.*/ 
    nextion_event_callback_set_on_touch(nextion_handle,
                                        callback_touch_event); 

     /*Initialize nextion objects */

    nextion_page_set(nextion_handle, "0");

    vTaskDelay(pdMS_TO_TICKS(1000)); 

    nextion_page_set(nextion_handle, "1");


    task_remaining_space();

            /*some delay*/ 

    vTaskDelay(pdMS_TO_TICKS(500));


    for (;;) {

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
 
        
         vTaskDelay(pdMS_TO_TICKS(500));
    }
}


void callback_touch_event(nextion_on_touch_event_t event)
{

    if (event.page_id == 0 && event.component_id == 1 && event.state == NEXTION_TOUCH_PRESSED)
    {
        ESP_LOGI(TAG, "button pressed");
        
    }
}

