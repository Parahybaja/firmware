#include "task/fuel_em.h"

static const char *TAG = "task_fuel_em";

void task_fuel_em(void *arg){
    (void)arg;

    // create task variables
    const int send_rate = int(1000.0 / float(TASK_FUEL_SEND_RATE_Hz));
    uint32_t timer_send;
    float sum;
    bool last_value = false;
    sensor_t fuel = {FUEL_EMERGENCY, 0.0};

    // show remaining task space
    task_remaining_space();

    // -----update timer-----
    timer_send = esp_log_timestamp();

    for (;;) {
        if ((esp_log_timestamp() - timer_send) >= send_rate){
            // -----add to timer-----
            timer_send += send_rate;

            // -----calculate-----
            sum = 0; // clean sum buffer
            for (int i=0; i < FUEL_AVERAGE_POINTS; i++) { 
                if (ACTIVE_LOW)
                    sum += !digitalRead(pin_fuel);
                else
                    sum += digitalRead(pin_fuel);
            }
            fuel.value = sum / float(FUEL_AVERAGE_POINTS);

            // ----- define high or low level -----
            // comment out to send raw data fuel average
            if (fuel.value <= THRESHOLD) // low fuel level
                fuel.value = true; // active low fuel emergency flag
            else 
                fuel.value = false; 

            // ----- send data just when is changed -----
            if (fuel.value != last_value) {
                // -----send fuel data through esp-now to receiver-----
                ESP_LOGD(TAG, "send fuel_em");
                // esp_now_send(address_ECU_Front, (uint8_t *) &fuel, sizeof(fuel));
            }

            // ----- update last value -----
            last_value = fuel.value;
        }

        vTaskDelay(pdMS_TO_TICKS(1)); // free up the processor
    }
}
