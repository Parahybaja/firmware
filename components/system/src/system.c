#include "system.h"

static const char *TAG = "system";

// -----system run time data (Quadro de Avisos)-----
system_t system_global = {0}; // O "{0}" inicializa tudo com zero automaticamente
bool lora_initialized_flag = false;

// -----FreeRTOS objects-----
TaskHandle_t th_lora, th_alive, th_display_nextion, th_rollover;
SemaphoreHandle_t sh_global_vars;

// -----esp-now addresses-----
const uint8_t mac_address_TCU[]       = {0xC8, 0xF0, 0x9E, 0x31, 0x8C, 0xA0};
const uint8_t mac_address_ECU_box[]   = {0xC8, 0xF0, 0x9E, 0x31, 0x8C, 0xA0};
const uint8_t mac_address_ECU_front[] = {0xC8, 0xF0, 0x9E, 0x31, 0x8D, 0x39};
const uint8_t mac_address_ECU_rear[]  = {0xD8, 0x13, 0x2A, 0x2E, 0xCA, 0xB4};
const uint8_t mac_address_module_1[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x88, 0x9E};
const uint8_t mac_address_module_2[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8A, 0xD8};
const uint8_t mac_address_module_3[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8B, 0xA0};


void print_task_remaining_space(void) {
    TaskHandle_t handler = xTaskGetCurrentTaskHandle();
    uint32_t stackHighWaterMark = uxTaskGetStackHighWaterMark(handler);
    const char *task_name = pcTaskGetName(handler);

    ESP_LOGW(TAG, "task '%s' remaining stack space: %lu bytes", task_name, stackHighWaterMark);
    vTaskDelay(pdMS_TO_TICKS(50));
}

void print_mac_address(void) {
    uint8_t mac[ESP_NOW_ETH_ALEN];
    esp_err_t ret = esp_wifi_get_mac(ESP_IF_WIFI_AP, mac);
    
    if (ret == ESP_OK) {
        ESP_LOGW(TAG, "MAC Address: " MACSTR "", MAC2STR(mac));
    } else {
        ESP_LOGE(TAG, "Failed to get MAC address");
    }
}

// Substitua estas duas funções no seu system.c:

simplified_system_t system_to_simplified(const system_t *original) {
    simplified_system_t simplified = {
        .key          = TELEMETRY_KEY,
        .rpm          = (uint16_t)original->rpm,
        .speed        = (uint8_t)original->speed,
        .fuel_level   = (uint8_t)(original->fuel_level * 100),
        .fuel_em      = (original->fuel_em != 0),
        .battery      = (uint8_t)(original->battery * 15),
        .temp         = (int8_t)original->temp,
        .rollover     = (original->rollover != 0),
        .tilt_x       = (int16_t)original->tilt_x, // Cast ajustado
        .tilt_y       = (int16_t)original->tilt_y, // Cast ajustado
        .tilt_z       = (int16_t)original->tilt_z, // Cast ajustado
        .blind_spot_l = (original->blind_spot_l != 0),
        .blind_spot_r = (original->blind_spot_r != 0),
        .fourxfour    = (uint8_t)original->fourxfour
    };
    return simplified;
}

system_t simplified_to_system(const simplified_system_t *simplified) {
    system_t original = {
        .rpm          = (float) simplified->rpm,
        .speed        = (float) simplified->speed,
        .fuel_level   = (float) simplified->fuel_level / 100.0f,
        .fuel_em      = (float)(simplified->fuel_em ? 1 : 0),
        .battery      = (float) simplified->battery / 15.0f,
        .temp         = (float) simplified->temp,
        .rollover     = (float)(simplified->rollover ? 1 : 0),
        .tilt_x       = (float) simplified->tilt_x,
        .tilt_y       = (float) simplified->tilt_y,
        .tilt_z       = (float) simplified->tilt_z,
        .blind_spot_l = (float)(simplified->blind_spot_l ? 1 : 0),
        .blind_spot_r = (float)(simplified->blind_spot_r ? 1 : 0),
        .fourxfour    = (float)(simplified->fourxfour ? 1 : 0)
    };
    return original;
}

void system_queue_init(void) {
    // Agora o sistema inicializa apenas o semáforo que protege a system_global
    sh_global_vars = xSemaphoreCreateMutex();
    if(sh_global_vars == NULL) {
        ESP_LOGE(TAG, "global vars Semaphore init failed");
    }
}

void system_espnow_init(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE));
    ESP_ERROR_CHECK(esp_timer_early_init());

    ESP_ERROR_CHECK(esp_now_init());

    esp_now_peer_info_t* peer = malloc(sizeof(esp_now_peer_info_t));
    if (peer == NULL) {
        ESP_LOGE(TAG, "Malloc peer information fail");
        esp_now_deinit();
        esp_restart();
    }
    memset(peer, 0, sizeof(esp_now_peer_info_t));
    peer->channel = ESPNOW_CHANNEL;
    peer->ifidx = ESP_IF_WIFI_AP;
    peer->encrypt = false;

    memcpy(peer->peer_addr, mac_address_TCU, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(peer));
    
    memcpy(peer->peer_addr, mac_address_ECU_front, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(peer));

    memcpy(peer->peer_addr, mac_address_ECU_rear, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(peer));

    memcpy(peer->peer_addr, mac_address_module_1, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(peer));

    memcpy(peer->peer_addr, mac_address_module_2, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(peer));

    memcpy(peer->peer_addr, mac_address_module_3, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(peer));
    
    free(peer);
}

void system_lora_init(int cr, int sbw, int sf) {
    if (lora_init() == 0) {
        ESP_LOGE(TAG, "Does not recognize the module");
        return;
    }
    ESP_LOGI(TAG, "Frequency is 915MHz");
    lora_set_frequency(915e6); // 915MHz
    lora_enable_crc();
    lora_set_coding_rate(cr);
    ESP_LOGI(TAG, "coding_rate=%d", cr);
    lora_set_bandwidth(sbw);
    ESP_LOGI(TAG, "signal_bandwidth=%d", sbw);
    lora_set_spreading_factor(sf);
    ESP_LOGI(TAG, "spreading_factor=%d", sf);

    lora_initialized_flag = true;
}

void task_lora_sender(void *arg) {
    (void)arg;
    ESP_LOGW(TAG, "Start lora sender task");

    const int send_rate_ms = (int)(1000.0 / (float)(TASK_TELEMETRY_SEND_RATE_Hz));
    uint32_t timer_send_ms = esp_log_timestamp();
    simplified_system_t simplified;
    TickType_t start_tick, end_tick;
    int lost;

    print_task_remaining_space();

    if (lora_initialized_flag == false) {
        ESP_LOGE(TAG, "LoRa not initialized");
        vTaskDelete(NULL);
    } 

    for (;;) {
        if ((esp_log_timestamp() - timer_send_ms) >= send_rate_ms){
            timer_send_ms += send_rate_ms;

            // LÊ OS DADOS COM SEGURANÇA
            xSemaphoreTake(sh_global_vars, portMAX_DELAY);
            
            ESP_LOGI(TAG, "DEBUG LORA SEND -> SPD: %.1f | RPM: %.1f | BAT: %.1f | 4x4: %d", 
                     system_global.speed, system_global.rpm, system_global.battery, (int)system_global.fourxfour);

            simplified = system_to_simplified(&system_global);
            xSemaphoreGive(sh_global_vars);

            start_tick = xTaskGetTickCount();

            lora_send_packet((uint8_t*)&simplified, sizeof(simplified));
            
            end_tick = xTaskGetTickCount();
            uint32_t time_taken_ms = pdTICKS_TO_MS(end_tick - start_tick);
            ESP_LOGI(TAG, "Sent %d bytes in %" PRIu32 " ms", sizeof(simplified), time_taken_ms);

            lost = lora_packet_lost();
            if (lost != 0) {
                ESP_LOGW(TAG, "%d packets lost", lost);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void task_lora_receiver(void *arg) {
    (void)arg;
    ESP_LOGW(TAG, "Start lora receiver task");

    system_t converted_system;
    simplified_system_t received_system;
    uint8_t buf[256];
    char payload[256];
    int rxLen;

    print_task_remaining_space();

    if (lora_initialized_flag == false) {
        ESP_LOGE(TAG, "LoRa not initialized");
        vTaskDelete(NULL);
    } 

    for (;;) {
        lora_receive(); 
        
        if (lora_received()) {
            rxLen = lora_receive_packet(buf, sizeof(buf));

            if (rxLen == sizeof(simplified_system_t)) {
                memcpy(&received_system, buf, sizeof(simplified_system_t));

                if (received_system.key == TELEMETRY_KEY) {
                    converted_system = simplified_to_system(&received_system);

                    int len = snprintf(
                        payload, sizeof(payload),
                        "DATA:%d,%d,%d,%.2f,%.2f,%d,%.2f,%.2f,%.2f,%d",
                        (uint16_t)converted_system.rpm,
                        (uint8_t)converted_system.speed,
                        (uint8_t)converted_system.fuel_em,
                        (float)converted_system.battery,
                        (float)converted_system.temp,
                        (uint8_t)converted_system.rollover,
                        (float)converted_system.tilt_x,
                        (float)converted_system.tilt_y,
                        (float)converted_system.tilt_z,
                        (uint8_t)converted_system.fourxfour
                    );

                    if (len > 0 && len < sizeof(payload)) {
                        printf("%s\n", payload); 
                    } else {
                        ESP_LOGE(TAG, "Error formatting the payload");
                    }
                }
                memset(&received_system, 0, sizeof(received_system));
                memset(&converted_system, 0, sizeof(converted_system));
                memset(&payload, 0, sizeof(payload));
                memset(&buf, 0, sizeof(buf));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}