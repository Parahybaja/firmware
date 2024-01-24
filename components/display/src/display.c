#include "task/display.h"

static const char *TAG = "display";

TaskHandle_t task_handle_user_interface;

void task_display(void *arg){
    (void)arg;
   
   nextion_t *nextion_handle = nextion_driver_install( UART_NUM_2,
                                                       9600,
                                                       GPIO_NUM_17,
                                                       GPIO_NUM_16);


    /*Do basic configuration*/
    nextion_init (nextion_handle);

    /*Set a callback for touch events.*/ 
    nextion_event_callback_set_on_touch(nextion_handle,
                                        callback_touch_event); 

     /*Initialize nextion objects */

    nextion_page_set(nextion_handle, "0");

    /*nextion_page_set(nextion_handle, "1");*/

    /*p1t0_handle = nextion_text_init(nextion_handle, "p1t0");
    p1t1_handle = nextion_text_init(nextion_handle, "p1t1");
    p1t2_handle = nextion_text_init(nextion_handle, "p1t2");
    p1t3_handle = nextion_text_init(nextion_handle, "p1t3");
    p1t4_handle = nextion_text_init(nextion_handle, "p1t4");
    p1bt0_handle = nextion_button_init(nextion_handle, "p1bt0");
    p1j0_handle = nextion_progressbar_init(nextion_handle, "p1j0");*/



    task_remaining_space();

            /*some delay*/ 

     vTaskDelay(pdMS_TO_TICKS(500));


    for (;;) {

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // Get the text value from a component.
        /*nextion_component_get_text(nextion_handle,
                                   "value_text",
                                   text_buffer,
                                   &text_length);**/

        // Get the integer value from a component.
        /*nextion_component_get_value(nextion_handle,
                                    "value_number",
                                    &number);

        ESP_LOGI(TAG, "text: %s", text_buffer);
        ESP_LOGI(TAG, "number: %lu", number);*/ 
        
         vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void callback_touch_event(nextion_on_touch_event_t event)
{
    if (event.page_id == 0 && event.component_id == 2 && event.state == NEXTION_TOUCH_PRESSED)
    {
        ESP_LOGI(TAG, "button pressed");

        /*xTaskNotify(task_handle_user_interface,
                    event.component_id,
                    eSetValueWithOverwrite);*/
    }
}
