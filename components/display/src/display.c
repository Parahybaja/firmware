#include "task/display.h"

static const char *TAG = "display";

nextion_t *nextion_handle; // Declare nextion_handle globally
int page_num;
static TaskHandle_t task_handle_user_interface; // handler to use with touch callback

void task_display(void *arg) {
    (void)arg;

    nextion_handle = nextion_driver_install(
        UART_NUM_2,
        115200,
        GPIO_NUM_17,
        GPIO_NUM_16);

    sensor_t recv_sensor = {};
    char msg_buffer[10];
    float percent;

    /*Do basic configuration*/
    nex_err_t nex_err = nextion_init(nextion_handle);

    if (nex_err != 0) {
        ESP_LOGE(TAG, "Error initializing nextion display");

        vTaskDelete(NULL);
    }

    // Start a task that will handle touch notifications.
    xTaskCreate(
        process_callback_queue,
        "user_interface",
        2048,
        NULL,
        5,
        &task_handle_user_interface);

    /*Set a callback for touch events.*/
    nextion_event_callback_set_on_touch(
        nextion_handle,
        callback_touch_event);

    /* display initialization routine */
    nextion_page_set(nextion_handle, NEX_PAGE_INTRO);
    ESP_LOGI(TAG, "page:%i", page_num);
    vTaskDelay(pdMS_TO_TICKS(2000));
    nextion_page_set(nextion_handle, NEX_PAGE_MODE_BLACK);
    page_num=1;
    ESP_LOGI(TAG, "page:%i", page_num);
    print_task_remaining_space();
    vTaskDelay(pdMS_TO_TICKS(500));

    for (;;) {

        // speed
        if (xQueueReceive(qh_speed, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var in a protected environment
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
            system_global.speed = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            snprintf(msg_buffer, 10, "%d", (int)recv_sensor.value);
            nextion_component_set_text(nextion_handle, NEX_TEXT_SPEED, msg_buffer);
            memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
        }

        // RPM
        else if (xQueueReceive(qh_rpm, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
            system_global.rpm = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            percent = convert_to_percent(recv_sensor.value, NEX_RPM_MAX, NEX_RPM_MIN);

            // print to display
            nextion_component_set_value(nextion_handle, NEX_PROGRESSBAR_RPM, percent);
        }

        // fuel
        else if (xQueueReceive(qh_fuel_emer, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
            system_global.fuel_em = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            nextion_component_set_boolean(nextion_handle, NEX_DSBUTTON_FUEL_EM, (bool)recv_sensor.value);
        }

        // battery
        else if (xQueueReceive(qh_battery, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
            system_global.battery = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            percent = convert_to_percent(recv_sensor.value, NEX_BAT_MAX, NEX_BAT_MIN);

            // print to display
            snprintf(msg_buffer, 10, "%d%c", (int)percent, NEX_SYMBOL_PERCENT);
            nextion_component_set_text(nextion_handle, NEX_TEXT_BATTERY, msg_buffer);
            memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
        }

        // temperature
        else if (xQueueReceive(qh_temp, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
            system_global.temp = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            snprintf(msg_buffer, 10, "%d%cC", (int)recv_sensor.value, NEX_SYMBOL_DEGREE);
            nextion_component_set_text(nextion_handle, NEX_TEXT_TEMP, msg_buffer);
            memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
        }

        // tilt_x - pitch
        else if (xQueueReceive(qh_tilt_x, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
            system_global.tilt_x = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            snprintf(msg_buffer, 10, "%d%c", (int)recv_sensor.value, NEX_SYMBOL_DEGREE);
            nextion_component_set_text(nextion_handle, NEX_TEXT_PITCH, msg_buffer);
            memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
        }

        // tilt_y - roll
        else if (xQueueReceive(qh_tilt_y, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
            system_global.tilt_y = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            snprintf(msg_buffer, 10, "%d%c", (int)recv_sensor.value, NEX_SYMBOL_DEGREE);
            nextion_component_set_text(nextion_handle, NEX_TEXT_ROLL, msg_buffer);
            memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void callback_touch_event(nextion_on_touch_event_t event){
   // ESP_LOGI(TAG, "page:%i, comp:%i", event.page_id, event.component_id);

    if (event.page_id == 1 && event.component_id == 3 && event.state == NEXTION_TOUCH_PRESSED){
        ESP_LOGI(TAG, "button1 pressed");

        xTaskNotify(
            task_handle_user_interface,
            event.component_id,
            eSetValueWithOverwrite);
    }
    else if (event.page_id == 2 && event.component_id == 0 && event.state == NEXTION_TOUCH_PRESSED){
        ESP_LOGI(TAG, "button2 pressed");

        xTaskNotify(
            task_handle_user_interface,
            event.component_id,
            eSetValueWithOverwrite);
    }
}

[[noreturn]] static void process_callback_queue(void *arg){
    const uint8_t MAX_TEXT_LENGTH = 50;
    char text_buffer[MAX_TEXT_LENGTH];
    size_t text_length = MAX_TEXT_LENGTH;
    int32_t number;
    uint32_t touch_id;

    for (;;){
        touch_id = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (touch_id == 3){
            nextion_page_set(nextion_handle, NEX_PAGE_MODE_WHITE);
            page_num = 2;
        }
        else if (touch_id == 6){
            nextion_page_set(nextion_handle, NEX_PAGE_INTRO);
            page_num = 0;
        }
        else if (touch_id == 0){
            nextion_page_set(nextion_handle, NEX_PAGE_MODE_BLACK);
            page_num = 1;
        }

        ESP_LOGI(TAG, "received task notify");
        ESP_LOGI(TAG, "page:%i", page_num);
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