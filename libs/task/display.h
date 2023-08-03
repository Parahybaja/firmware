/**
 * @file task_display.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief display task, functions and configs
 * @version 0.3
 * @date 2023-02-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <Nextion.h>

#include "system.h"

#define DEGREE 0xB0 // hex value for °
#define SPONSOR_TIME 2000

// Nextion objects
NexPage page0 = NexPage(0,  0, "page0");
NexPage page1 = NexPage(1,  0, "page1");
NexText p1t0 = NexText(1, 2, "p1t0"); // speed
NexText p1t1 = NexText(1, 4, "p1t1"); // battery
NexText p1t2 = NexText(1, 5, "p1t2"); // temp
NexText p1t3 = NexText(1, 6, "p1t3"); // roll
NexText p1t4 = NexText(1, 7, "p1t4"); // pitch
NexDSButton p1bt0 = NexDSButton(1, 3, "p1bt0"); // fuel
NexProgressBar p1j0 = NexProgressBar(1, 1, "p1j0"); // RPM

// Register an object to the touch event list.  
NexTouch *nex_listen_list[] = {
    // &b0,
    // &b1,
    NULL
};

/**
 * @brief display control task
 * 
 * @param arg void arg
 */
void task_display(void*);

void task_display(void *arg){
    (void)arg;

    // -----create local variables-----
    sensor_t recv_sensor = {};
    char msg_buffer[10];
    float percent;

#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "%d bytes remaining of the display control task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    // ----- init nextion display -----
    vTaskDelay(100 / portTICK_PERIOD_MS);
    if (!nexInit(115200, 115200)) // debug serial baud rate
        ERROR("Could not connect to nextion display");
    log_i("connected to nextion display");

    // sponsor time
    vTaskDelay(SPONSOR_TIME / portTICK_PERIOD_MS);
    page1.show();

    while (true){
        // update nextion objects
        nexLoop(nex_listen_list);

        // speed
        if (xQueueReceive(qh_speed, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                // workaround speed bug
                recv_sensor.value /= 2.0;

                system_global.speed = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            snprintf(msg_buffer, 10, "%d", int(recv_sensor.value));
            p1t0.setText(msg_buffer);
            memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
        }

        // RPM
        else if (xQueueReceive(qh_rpm, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                // workaround RPM bug
                recv_sensor.value /= 4.0;

                system_global.rpm = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // convert RPM to percent
            if (recv_sensor.value > RPM_MAX)
                percent = 100.0;
            else if (recv_sensor.value < RPM_MIN)
                percent = 0.0;
            else
                percent = (recv_sensor.value / (RPM_MAX - RPM_MIN)) * 100.0;
            
            // print to display
            p1j0.setValue(percent);
        }

        // fuel
        else if (xQueueReceive(qh_fuel_emer, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                // workaround: invert fuel logic
                recv_sensor.value = (recv_sensor.value) ? 0 : 1; // ternary operator
                
                system_global.fuel_emer = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            if (recv_sensor.value)
                p1bt0.setValue(1);
            else
                p1bt0.setValue(0);
        }

        // battery
        else if (xQueueReceive(qh_battery, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.battery = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // convert battery to percent
            if (recv_sensor.value > BAT_MAX)
                percent = 100.0;
            else if (recv_sensor.value < BAT_MIN)
                percent = 0.0;
            else
                percent = ((recv_sensor.value - BAT_MIN) / (BAT_MAX - BAT_MIN)) * 100.0;

            // print to display
            snprintf(msg_buffer, 10, "%d%c", int(percent), 0x25);
            p1t1.setText(msg_buffer);
            memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
        }

        // temperature
        else if (xQueueReceive(qh_temp, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.temp = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            snprintf(msg_buffer, 10, "%d%cC", int(recv_sensor.value), DEGREE);
            p1t2.setText(msg_buffer);
            memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
        }

        // tilt_x - pitch
        else if (xQueueReceive(qh_tilt_x, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.tilt_x = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            snprintf(msg_buffer, 10, "%d%c", int(recv_sensor.value), DEGREE);
            p1t4.setText(msg_buffer);
            memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
        }

        // tilt_y - roll
        else if (xQueueReceive(qh_tilt_y, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.tilt_y = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            snprintf(msg_buffer, 10, "%d%c", int(recv_sensor.value), DEGREE);
            p1t3.setText(msg_buffer);
            memset(msg_buffer, 0, sizeof(msg_buffer)); // clear buffer
        }

        vTaskDelay(1 / portTICK_PERIOD_MS); // give time to send the espnow message
    }
}

#endif // __DISPLAY_H__