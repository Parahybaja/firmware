#include "task/display.h"

static const char *TAG = "display";

nextion_t *nextion_handle; // Declare nextion_handle globally
static TaskHandle_t task_handle_user_interface; // handler to use with touch callback
int current_page_num;

void task_display(void *arg) {
    (void)arg;

    sensor_t recv_sensor;
    char msg_buffer[10];
    float percent;

    nextion_handle = nextion_driver_install(
        UART_NUM_2,
        115200,
        GPIO_NUM_17,
        GPIO_NUM_16
    );

    nex_err_t nex_init_err = nextion_init(nextion_handle);

    if (nex_init_err != 0) {
        ESP_LOGE(TAG, "Error initializing nextion display");
    }

    // Start a task that will handle touch notifications.
    xTaskCreate(
        process_callback_queue,
        "user_interface",
        2048,
        NULL,
        5,
        &task_handle_user_interface
    );

    /* Set a callback for touch events. */
    nextion_event_callback_set_on_touch(
        nextion_handle,
        callback_touch_event
    );

    /* if there's no error with the nextion initialization */
    if (nex_init_err == 0) {
        /* display initialization routine */
        nextion_page_set(nextion_handle, NEX_PAGE_NAME_INTRO);
        current_page_num = NEX_PAGE_ID_INTRO;
        ESP_LOGI(TAG, "page:%i", current_page_num);
        
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    print_task_remaining_space();

    vTaskDelay(pdMS_TO_TICKS(500));

    for (;;) {

        // speed
        if (xQueueReceive(qh_speed, &recv_sensor, pdMS_TO_TICKS(0))){
            // update global system var in a protected environment
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.speed = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            /* if there's no error with the nextion initialization */
            if (nex_init_err == 0) {
                // print to display
                snprintf(msg_buffer, 10, "%d", (int)recv_sensor.value);
                if (current_page_num == NEX_PAGE_ID_LIGHT) {
                    nextion_component_set_text(nextion_handle, NEX_TEXT_SPEED_L, msg_buffer);
                }
                memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
            }
        }

        // RPM
        if (xQueueReceive(qh_rpm, &recv_sensor, pdMS_TO_TICKS(0))){
            // update global system var in a protected environment
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.rpm = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            /* if there's no error with the nextion initialization */
            if (nex_init_err == 0) {
                percent = convert_to_percent(recv_sensor.value, NEX_RPM_MAX, NEX_RPM_MIN);

                // print to display
                if (current_page_num == NEX_PAGE_ID_LIGHT) {
                    nextion_component_set_value(nextion_handle, NEX_PROGRESSBAR_RPM_L, percent);
                }
            }
        }

        // fuel
        if (xQueueReceive(qh_fuel_emer, &recv_sensor, pdMS_TO_TICKS(0))){
            // update global system var in a protected environment
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.fuel_em = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            /* if there's no error with the nextion initialization */
            if (nex_init_err == 0) {
                // print to display
                if (current_page_num == NEX_PAGE_ID_LIGHT) {
                    nextion_component_set_boolean(nextion_handle, NEX_DSBUTTON_FUEL_EM_L, (bool)recv_sensor.value);
                }
            }
        }

        // temperature
        if (xQueueReceive(qh_temp, &recv_sensor, pdMS_TO_TICKS(0))){
            // update global system var in a protected environment
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.temp = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            /* if there's no error with the nextion initialization */
            if (nex_init_err == 0) {
                // print to display
                snprintf(msg_buffer, 10, "%d", (int)recv_sensor.value);
                if (current_page_num == NEX_PAGE_ID_LIGHT) {
                    nextion_component_set_text(nextion_handle, NEX_TEXT_TEMP_L, msg_buffer);
                }
                memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
            }
        }

        // tilt_x - roll
        if (xQueueReceive(qh_tilt_x, &recv_sensor, pdMS_TO_TICKS(0))){
            // update global system var in a protected environment
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.tilt_x = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            /* if there's no error with the nextion initialization */
            if (nex_init_err == 0) {
                // print to display
                snprintf(msg_buffer, 10, "%d%c", (int)recv_sensor.value, NEX_SYMBOL_DEGREE);
                if (current_page_num == NEX_PAGE_ID_LIGHT) {
                    nextion_component_set_text(nextion_handle, NEX_TEXT_ROLL_L, msg_buffer);
                }
                memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
            }
        }

        // tilt_y - pitch
        if (xQueueReceive(qh_tilt_y, &recv_sensor, pdMS_TO_TICKS(0))){
            // update global system var in a protected environment
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.tilt_y = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            /* if there's no error with the nextion initialization */
            if (nex_init_err == 0) {
                // print to display
                snprintf(msg_buffer, 10, "%d%c", (int)recv_sensor.value, NEX_SYMBOL_DEGREE);
                if (current_page_num == NEX_PAGE_ID_LIGHT) {
                    nextion_component_set_text(nextion_handle, NEX_TEXT_PITCH_L, msg_buffer);
                }
                memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
            }

            memset(&recv_sensor, 0, sizeof(recv_sensor));
        }

        // battery
        if (xQueueReceive(qh_battery, &recv_sensor, pdMS_TO_TICKS(0))){
            // update global system var in a protected environment
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.battery = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            /* if there's no error with the nextion initialization */
            if (nex_init_err == 0) {
                percent = convert_to_percent(recv_sensor.value, NEX_BAT_MAX, NEX_BAT_MIN);

                // print to display
                snprintf(msg_buffer, 10, "%d", (int)percent);
                if (current_page_num == NEX_PAGE_ID_LIGHT) {
                    nextion_component_set_text(nextion_handle, NEX_TEXT_BATTERY_L, msg_buffer);
                }
                memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
            }
        }

        // timer - hour
        if (xQueueReceive(qh_hours, &recv_sensor, pdMS_TO_TICKS(0))) {
            // update global system var in a protected environment
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.battery = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            /* if there's no error with the nextion initialization */
            if (nex_init_err == 0) {
                // print to display
                snprintf(msg_buffer, 10, "%02d", (int)recv_sensor.value);
                if (current_page_num == NEX_PAGE_ID_ENDURO) {
                    nextion_component_set_text(nextion_handle, NEX_TEXT_HOUR_L, msg_buffer);
                }
                memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
            }
        }

        // timer - minutes
        if (xQueueReceive(qh_minutes, &recv_sensor, pdMS_TO_TICKS(0))) {
            // update global system var in a protected environment
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.battery = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            /* if there's no error with the nextion initialization */
            if (nex_init_err == 0) {
                // print to display
                snprintf(msg_buffer, 10, "%02d", (int)recv_sensor.value);
                if (current_page_num == NEX_PAGE_ID_ENDURO) {
                    nextion_component_set_text(nextion_handle, NEX_TEXT_MINUTE_L, msg_buffer);
                }
                memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
            }
        }

        // timer - seconds
        if (xQueueReceive(qh_seconds, &recv_sensor, pdMS_TO_TICKS(0))) {
            // update global system var in a protected environment
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.battery = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            /* if there's no error with the nextion initialization */
            if (nex_init_err == 0) {
                // print to display
                snprintf(msg_buffer, 10, "%02d", (int)recv_sensor.value);
                if (current_page_num == NEX_PAGE_ID_ENDURO) {
                    nextion_component_set_text(nextion_handle, NEX_TEXT_SECOND_L, msg_buffer);
                }
                memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
            }
        }

        // lap
        if (xQueueReceive(qh_lap, &recv_sensor, pdMS_TO_TICKS(0))) {
            update global system var in a protected environment
           xSemaphoreTake(sh_global_vars, portMAX_DELAY);
               system_global.battery = recv_sensor.value;
           xSemaphoreGive(sh_global_vars);

            /* if there's no error with the nextion initialization */
           if (nex_init_err == 0) {
                print to display
               snprintf(msg_buffer, 10, "%d", (int)recv_sensor.value);
               if (current_page_num == NEX_PAGE_ID_ENDURO) {
                   nextion_component_set_text(nextion_handle, NEX_TEXT_LAP_L, msg_buffer);
               }
               memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
           }
        }

        // lap time - minutes
        if (xQueueReceive(qh_lap_minutes, &recv_sensor, pdMS_TO_TICKS(0))) {
            update global system var in a protected environment
           xSemaphoreTake(sh_global_vars, portMAX_DELAY);
               system_global.battery = recv_sensor.value;
           xSemaphoreGive(sh_global_vars);

            /* if there's no error with the nextion initialization */
           if (nex_init_err == 0) {
                print to display
               snprintf(msg_buffer, 10, "%02d", (int)recv_sensor.value);
               if (current_page_num == NEX_PAGE_ID_ENDURO) {
                   nextion_component_set_text(nextion_handle, NEX_TEXT_LAP_MINUTES_L, msg_buffer);
               }
               memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
           }
        }

        // lap time - seconds
        if (xQueueReceive(qh_lap_seconds, &recv_sensor, pdMS_TO_TICKS(0))) {
            update global system var in a protected environment
           xSemaphoreTake(sh_global_vars, portMAX_DELAY);
               system_global.battery = recv_sensor.value;
           xSemaphoreGive(sh_global_vars);

            /* if there's no error with the nextion initialization */
           if (nex_init_err == 0) {
                print to display
               snprintf(msg_buffer, 10, "%02d", (int)recv_sensor.value);
               if (current_page_num == NEX_PAGE_ID_ENDURO) {
                   nextion_component_set_text(nextion_handle, NEX_TEXT_LAP_SECONDS_L, msg_buffer);
               }
               memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
           }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void callback_touch_event(nextion_on_touch_event_t event){
   ESP_LOGI(TAG, "page:%i, comp:%i", event.page_id, event.component_id);

    if (event.page_id == NEX_PAGE_ID_INTRO && event.state == NEXTION_TOUCH_PRESSED) {
        ESP_LOGI(TAG, "page 0 pressed");

        xTaskNotify(
            task_handle_user_interface,
            event.page_id,
            eSetValueWithOverwrite
        );
    }
    else if (event.page_id == NEX_PAGE_ID_LIGHT && event.state == NEXTION_TOUCH_PRESSED) {
        ESP_LOGI(TAG, "page 1 pressed");

        xTaskNotify(
            task_handle_user_interface,
            event.page_id,
            eSetValueWithOverwrite
        );
    }
    else if (event.page_id == NEX_PAGE_ID_ENDURO && event.state == NEXTION_TOUCH_PRESSED) {
        ESP_LOGI(TAG, "page 2 pressed");

        xTaskNotify(
            task_handle_user_interface,
            event.page_id,
            eSetValueWithOverwrite
        );
    }
}

void process_callback_queue(void *arg){
    uint32_t notify_page_id;

    for (;;){
        notify_page_id = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        /* change pages logic */
        if (notify_page_id == NEX_PAGE_ID_INTRO){
            nextion_page_set(nextion_handle, NEX_PAGE_NAME_LIGHT);
            current_page_num = NEX_PAGE_ID_LIGHT;
        }
        else if (notify_page_id == NEX_PAGE_ID_LIGHT){
            nextion_page_set(nextion_handle, NEX_PAGE_NAME_ENDURO);
            current_page_num = NEX_PAGE_ID_ENDURO;
        }
        else if (notify_page_id == NEX_PAGE_ID_ENDURO){
            nextion_page_set(nextion_handle, NEX_PAGE_NAME_LIGHT);
            current_page_num = NEX_PAGE_ID_LIGHT;
        }
        else {
            ESP_LOGE(TAG, "undefined touch id");
        }

        ESP_LOGI(TAG, "received task notify");
        ESP_LOGI(TAG, "page:%i", current_page_num);
    }
}

float convert_to_percent(float value, float max, float min){
    float percent;

    // convert battery to percent
    if (value > max)
        percent = 100.0;
    else if (value < min)
        percent = 0.0;
    else
        percent = ((value - min) / (max - min)) * 100.0;

    return percent;
}