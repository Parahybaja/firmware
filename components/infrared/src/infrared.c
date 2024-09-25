#include "task/infrared.h"

static const char *TAG = "task_infrared";

static TickType_t last_interrupt_time = 0;
static QueueHandle_t gpio_evt_queue = NULL;

static void pin_config(void) {
    esp_rom_gpio_pad_select_gpio(pin_infrared);
    gpio_set_direction(pin_infrared, GPIO_MODE_INPUT);
    gpio_pullup_dis(pin_infrared);
    gpio_pulldown_dis(pin_infrared);
    gpio_set_intr_type(pin_infrared, GPIO_INTR_NEGEDGE); // Interrupção na borda de descida
}

static void IRAM_ATTR infrared_handler(void* arg) {
    uint32_t io_num = (uint32_t)arg;
    TickType_t current_time = xTaskGetTickCount();

    if ((current_time - last_interrupt_time) * portTICK_PERIOD_MS > DEBOUNCE_DELAY_MS) {
        last_interrupt_time = current_time; 
        BaseType_t higher_priority_task_woken = pdFALSE;
        sensor_t infrared = {INFRARED, 1.0};  // Exemplo de evento detectado
        xQueueSendFromISR(gpio_evt_queue, &infrared, &higher_priority_task_woken);
        portYIELD_FROM_ISR(higher_priority_task_woken);
    }
}

void task_infrared(void* arg) {
    
    sensor_t infrared = {INFRARED, 0.0};
    const int send_rate_ms = (int)(1000.0 / (float)(TASK_INFRARED_RATE_Hz));
    bool last_value = false;

    // Configuração do pino e interrupção
    pin_config();
    gpio_evt_queue = xQueueCreate(queue_size, sizeof(sensor_t));
    if (gpio_evt_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create queue");
        vTaskDelete(NULL);
    }

    esp_err_t ret = gpio_install_isr_service(0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install ISR service: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
    }

    gpio_isr_handler_add(pin_infrared, infrared_handler, (void*)pin_infrared);

    // Loop da task
    for (;;) {
        if (xQueueReceive(gpio_evt_queue, &infrared, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Infrared detected: %.2f", infrared.value);

            // Aqui você pode adicionar código para enviar os dados via ESP-NOW, se necessário.
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // Liberar processador
    }
}
