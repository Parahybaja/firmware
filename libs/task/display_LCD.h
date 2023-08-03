/**
 * @file task_display.h
 * @author Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief display task, functions and configs
 * @version 0.4
 * @date 2023-02-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _DISPLAY_LCD_H_
#define _DISPLAY_LCD_H_

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include "system.h"

#define LCD_LIGHTNING 0
#define LCD_BAR       1
#define OFFSET_RPM    4
#define OFFSET_SPEED  1
#define OFFSET_BAT    11

LiquidCrystal_I2C lcd(0x27, 16, 2);

/**
 * @brief display control task
 * 
 * @param arg void arg
 */
void task_display_LCD(void*);

void print_LCD(float, uint8_t, uint8_t);

void print_LCD(float value, uint8_t digits, uint8_t precision) {
    char str[10];
    char filter[10];

    snprintf(filter, sizeof(filter), "%%%d.%df", digits, precision);

    snprintf(str, sizeof(str), filter, value);

    lcd.print(str);
}

void task_blink_LCD(void *arg){
    (void)arg;

    while (true) {
        lcd.noBacklight();

        vTaskDelay(100 / portTICK_PERIOD_MS);

        lcd.backlight();

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_display_LCD(void *arg){
    (void)arg;

    // -----create local variables-----
    sensor_t recv_sensor = {};
    float percent;
    uint16_t last_RPM;
    uint8_t bar_length, last_bar_length;
    byte lightning[] = {
        B00001,
        B00110,
        B01100,
        B11110,
        B11111,
        B00111,
        B01100,
        B10000
    };
    byte bar[] = {
        B01110,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B01110
    };

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

    // init display
    lcd.init();
    lcd.clear();
    lcd.createChar(LCD_LIGHTNING, lightning);
    lcd.createChar(LCD_BAR, bar);
    lcd.backlight();
    log_i("display initialized");

    // ----- print titles -----
    // speed
    lcd.setCursor(OFFSET_SPEED, 0);
    lcd.print("-- kmh");

    // battery
    lcd.setCursor(OFFSET_BAT-1, 0);
    lcd.write(LCD_LIGHTNING);
    lcd.setCursor(OFFSET_BAT, 0);
    lcd.print("---%");

    // RPM
    lcd.setCursor(0, 1);
    lcd.print("RPM");

    while (true){

        // speed
        if (xQueueReceive(qh_speed, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.speed = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // print to display
            lcd.setCursor(1, 0);
            print_LCD(recv_sensor.value, 2, 0);
        }

        // RPM
        else if (xQueueReceive(qh_rpm, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.rpm = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // ----- convert to bar -----
            // Calcula o comprimento da barra com base na RPM
            bar_length = map(int(recv_sensor.value), 0, 4000, 0, 12);

            // Verifica se o valor da RPM ou da barra mudou desde a última atualização
            if (int(recv_sensor.value) != last_RPM || bar_length != last_bar_length) {
                // Atualiza a barra no display
                if (bar_length > last_bar_length) {
                    for (int i = last_bar_length; i < bar_length; i++) {
                        lcd.setCursor(OFFSET_RPM + i, 1);
                        lcd.write(LCD_BAR);
                    }
                } else if (bar_length < last_bar_length) {
                    for (int i = bar_length; i < last_bar_length; i++) {
                        lcd.setCursor(OFFSET_RPM + i, 1);
                        lcd.print(" ");
                    }
                }

                // Salva o valor da RPM e da barra para a próxima verificação
                last_RPM = int(recv_sensor.value);
                last_bar_length = bar_length;
            }
        }

        // fuel
        else if (xQueueReceive(qh_fuel_emer, &recv_sensor, pdMS_TO_TICKS(1))){
            // update global system var
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.fuel_emer = recv_sensor.value;
            xSemaphoreGive(sh_global_vars);

            // create or delete task
            if (recv_sensor.value) {
                // check if the task is already running
                if (th_blink_LCD == NULL) {
                    // create blink task
                    xTaskCreatePinnedToCore(
                        task_blink_LCD,  // task function
                        "display blink", // task name
                        1024,            // stack size
                        &lcd,            // parameters
                        8,               // priority
                        &th_blink_LCD,   // handler 
                        APP_CPU_NUM      // core number
                    );
                }
            }
            else {
                // check the task state
                if (th_blink_LCD != NULL) {
                    vTaskDelete(th_blink_LCD);
                    th_blink_LCD = NULL;
                }

                // turn on display
                lcd.backlight();
            }
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
            lcd.setCursor(11, 0);
            print_LCD(percent, 3, 0);
        }

        vTaskDelay(1 / portTICK_PERIOD_MS); // give time to send the espnow message
    }
}

#endif // _DISPLAY_LCD_H_