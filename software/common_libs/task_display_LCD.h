/**
 * @file task_display.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief display task, functions and configs
 * @version 0.1
 * @date 2023-02-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __DISPLAY_LCD_H__
#define __DISPLAY_LCD_H__

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include "communication.h"

LiquidCrystal_I2C lcd_top(0x27,16,2);
LiquidCrystal_I2C lcd_bot(0x26,16,2);

/**
 * @brief display control task
 * 
 * @param arg void arg
 */
void task_display_LCD(void*);

void task_display_LCD(void *arg){
    (void)arg;

    // -----create local variables-----
    sensor_t recv_sensor = {};

    // init display
    lcd_top.init();
    lcd_bot.init();
    lcd_top.backlight();
    lcd_bot.backlight();

    // print titles
    lcd_top.setCursor(0, 0);
    lcd_top.print("speed: ");
    lcd_top.setCursor(0, 1);
    lcd_top.print("RPM: ");

    lcd_bot.setCursor(0, 0);
    lcd_bot.print("comb: ");
    lcd_bot.setCursor(0, 1);
    lcd_bot.print("battery: ");


#if DEBUG_MODE
    // see the remaining space of this task
    // always after create the variables
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    char info[ESPNOW_BUFFER_SIZE];
    snprintf(
        info, 
        ESPNOW_BUFFER_SIZE,
        "INFO_1: %d bytes remaining of the display control task space", 
        uxHighWaterMark); 
    INFO(info);
    vTaskDelay(50 / portTICK_PERIOD_MS); // give time to send the espnow message
#endif

    while (true){

        if (xQueueReceive(qh_speed, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.speed = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            lcd_top.setCursor(8, 0);
            lcd_top.print(recv_sensor.value);
        }
        else if (xQueueReceive(qh_rpm, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.rpm = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            lcd_top.setCursor(6, 1);
            lcd_top.print(recv_sensor.value);
        }
        else if (xQueueReceive(qh_fuel_emer, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.fuel_emer = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            lcd_bot.setCursor(6, 0);
            lcd_bot.print(recv_sensor.value);
        }
        else if (xQueueReceive(qh_battery, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.battery = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            lcd_bot.setCursor(10, 1);
            lcd_bot.print(recv_sensor.value);
        }

        vTaskDelay(1 / portTICK_PERIOD_MS); // give time to send the espnow message
    }
}

#endif // __DISPLAY_LCD_H__