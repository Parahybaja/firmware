#include "espnow_callback.h"

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_now.h"

static const char *TAG = "espnow_callback";

void espnow_send_callback(const uint8_t* mac_addr, esp_now_send_status_t status) {
    if (mac_addr == NULL) {
        ESP_LOGE(TAG, "Send callback arg error");
        return;
    }

    #if DEBUG_ESPNOW_CALLBACK
        if (status == ESP_NOW_SEND_SUCCESS) {
            ESP_LOGI(TAG, "Send successfully");
        }
        else {
            ESP_LOGE(TAG, "Send fail");
        }
    #endif
}

void espnow_recv_callback(const esp_now_recv_info_t* recv_info, const uint8_t* data, int len) {
    uint8_t *mac_addr = recv_info->src_addr;
    uint8_t *des_addr = recv_info->des_addr;

    if (mac_addr == NULL || data == NULL || len <= 0) {
        ESP_LOGE(TAG, "Receive cb arg error");
        return;
    }

    if (len == sizeof(sensor_t)) {
        sensor_t recv_sensor;
        memcpy(&recv_sensor, data, len);

        if (recv_sensor.type == BATTERY){
            // -----send RPM data through queue-----
            xQueueSend(qh_battery, &recv_sensor, pdMS_TO_TICKS(0));
        }
        else if (recv_sensor.type == RPM){
            // -----send RPM data through queue-----
            xQueueSend(qh_rpm, &recv_sensor, pdMS_TO_TICKS(0));
        }
        else if (recv_sensor.type == SPEEDOMETER){
            // -----send speed data through queue-----
            xQueueSend(qh_speed, &recv_sensor, pdMS_TO_TICKS(0));
        }
        else if (recv_sensor.type == FUEL_EMERGENCY){
            // -----send speed data through queue-----
            xQueueSend(qh_fuel_emer, &recv_sensor, pdMS_TO_TICKS(0));
        }
        else {
            ESP_LOGE(TAG, "unknown sensor type");
        }
    }
    else {
        ESP_LOGE(TAG, "unrecognized packet");
    }
}

void register_callbacks(void) {
    ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_callback));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_callback));
}
