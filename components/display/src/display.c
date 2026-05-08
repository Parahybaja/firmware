#include <string.h>
#include "task/display.h"
#include "driver/gpio.h"

#define BATTERY_MAX 13.2f
#define BATTERY_MIN 11.5f

#define NEX_UART_PORT      UART_NUM_2
#define NEX_TXD_PIN        GPIO_NUM_16
#define NEX_RXD_PIN        GPIO_NUM_17
#define NEX_GPIO_4X4_INPUT_PIN        GPIO_NUM_15

static const char *TAG = "display";

void sendNextionInt(const char* obj, int val) {
    char cmd[64];
    int len = snprintf(cmd, sizeof(cmd), "%s.val=%d", obj, val);
    uart_write_bytes(NEX_UART_PORT, cmd, len);
    const uint8_t end_cmd[] = {0xFF, 0xFF, 0xFF};
    uart_write_bytes(NEX_UART_PORT, (const char*)end_cmd, 3);
}

void task_display(void *arg) {
    (void)arg;

    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    uart_driver_install(NEX_UART_PORT, 1024 * 2, 0, 0, NULL, 0);
    uart_param_config(NEX_UART_PORT, &uart_config);
    uart_set_pin(NEX_UART_PORT, NEX_TXD_PIN, NEX_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    vTaskDelay(pdMS_TO_TICKS(500));
    sendNextionInt("page page0", 0); // Comando para garantir página 0

    for (;;) {
        // 1. Lê o pino do 4x4 fisicamente
        int level_4x4 = gpio_get_level(GPIO_4X4_INPUT);

        // 2. TRANCA A PORTA PARA LER E ESCREVER COM SEGURANÇA
        xSemaphoreTake(sh_global_vars, portMAX_DELAY);
        
        // Atualiza o 4x4 no quadro de avisos global
        system_global.fourxfour = (level_4x4 == 0); 

        // Faz uma cópia rápida de todos os dados para variáveis locais
        float disp_speed = system_global.speed;
        float disp_rpm   = system_global.rpm;
        float disp_temp  = system_global.temp;
        float disp_bat   = system_global.battery;
        float disp_roll  = system_global.tilt_x;
        float disp_pitch = system_global.tilt_y;
        //float disp_yaw   = system_global.tilt_z;
        float disp_fuel  = system_global.fuel_em;
        bool  disp_4x4   = system_global.fourxfour;

        // 3. DESTRANCA A PORTA (O LoRa ou os sensores já podem usar a struct)
        xSemaphoreGive(sh_global_vars);

        // 4. Calcula a porcentagem da bateria
        float perc_bat = ((disp_bat - BATTERY_MIN) / (BATTERY_MAX - BATTERY_MIN)) * 100.0f;
        if (perc_bat > 100) perc_bat = 100;
        if (perc_bat < 0) perc_bat = 0;

        // 5. Envia tudo para o Nextion
        sendNextionInt(NEX_VAR_SPEED,   (int)disp_speed);
        sendNextionInt(NEX_VAR_RPM,     (int)disp_rpm);
        sendNextionInt(NEX_VAR_TEMP,    (int)disp_temp);
        sendNextionInt(NEX_VAR_BATTERY, (int)perc_bat);
        sendNextionInt(NEX_VAR_ROLL,    (int)disp_roll);
        sendNextionInt(NEX_VAR_PITCH,   (int)disp_pitch);
        sendNextionInt(NEX_VAR_FUEL,    (int)disp_fuel);
        sendNextionInt(NEX_VAR_4X4,     disp_4x4 ? 1 : 0);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}