#include "task/fuel_em.h"
#include "system.h"
#include "esp_log.h"

static const char *TAG = "task_fuel_em";

void task_fuel_em(void *arg){
    
    const gpio_num_t gpio_pin = (gpio_num_t)arg;

    // -----config gpio-----
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL<<gpio_pin);
    io_conf.pull_down_en = false;
    io_conf.pull_up_en = false;
    ESP_ERROR_CHECK(gpio_config(&io_conf)); 

    // create task variables
    const int send_rate_ms = (int)(1000.0 / (float)(TASK_FUEL_SEND_RATE_Hz));
    uint32_t timer_send_ms;
    float sum;
    
    // Novas variáveis no lugar do sensor_t
    float fuel_val = 0.0f;
    float last_value = -1.0f; // Inicializa com -1 para forçar o primeiro envio

    // show remaining task space
    print_task_remaining_space();

    // -----update timer-----
    timer_send_ms = esp_log_timestamp();

    for (;;) {
        if ((esp_log_timestamp() - timer_send_ms) >= send_rate_ms){
            // -----add to timer-----
            timer_send_ms += send_rate_ms;

            // -----calculate-----
            sum = 0; // clean sum buffer
            for (int i=0; i < FUEL_AVERAGE_POINTS; i++) { 
                if (ACTIVE_LOW)
                    sum += !gpio_get_level(gpio_pin);
                else
                    sum += gpio_get_level(gpio_pin);
            }
            fuel_val = sum / (float)(FUEL_AVERAGE_POINTS);

            // ----- define high or low level -----
            if (fuel_val <= THRESHOLD) // low fuel level
                fuel_val = 1.0f; // active emergency flag
            else 
                fuel_val = 0.0f; 

            // ----- Atualiza a struct apenas quando o valor mudar -----
            if (fuel_val != last_value) {
                ESP_LOGI(TAG, "Fuel state changed. Updating global struct to: %.1f", fuel_val);
                
                // Atualiza o Quadro de Avisos Global
                xSemaphoreTake(sh_global_vars, portMAX_DELAY);
                system_global.fuel_em = fuel_val;
                xSemaphoreGive(sh_global_vars);
                
                // update last value
                last_value = fuel_val;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // free up the processor
    }
}