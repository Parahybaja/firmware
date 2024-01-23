#include "system.h"

static const char *TAG = "system";

// -----system run time data-----
system_t system_global = {
    .rpm = 0.0,
    .speed = 0.0,
    .fuel_em = 0.0,
    .battery = 0.0,
    .temp = 0.0,
    .rollover = 0.0,
    .tilt_x = 0.0,
    .tilt_y = 0.0,
    .tilt_z = 0.0
};

// -----FreeRTOS objects-----
TaskHandle_t th_example;
TaskHandle_t th_alive;

// -----esp-now addresses-----
const uint8_t mac_address_ECU_BOX[]   = {0x94, 0xB5, 0x55, 0x2D, 0x1E, 0x0C};
const uint8_t mac_address_ECU_Front[] = {0x94, 0xB5, 0x55, 0x2D, 0x1A, 0x60};
const uint8_t mac_address_ECU_Rear[]  = {0x7C, 0x9E, 0xBD, 0xED, 0x52, 0x70};
const uint8_t mac_address_module_1[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8D, 0x38};
const uint8_t mac_address_module_2[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8A, 0xD8};
const uint8_t mac_address_module_3[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8B, 0xA0};

void task_remaining_space(void){
    TaskHandle_t handler = xTaskGetCurrentTaskHandle();

    uint32_t stackHighWaterMark = uxTaskGetStackHighWaterMark(handler);
    const char *task_name = pcTaskGetName(handler);

    ESP_LOGW(TAG, "task '%s' remaining stack space: %lu bytes", task_name, stackHighWaterMark);

    vTaskDelay(pdMS_TO_TICKS(50));
}

void print_mac_address(void) {
    uint8_t mac[MAC_SIZE];

    esp_err_t ret = esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
    
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "MAC Address: " MACSTR "", MAC2STR(mac));
    } else {
        ESP_LOGE(TAG, "Failed to get MAC address");
    }
}

void init_espnow(void) {
    // -----init wifi-----
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));
//     ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
//     ESP_ERROR_CHECK(esp_wifi_start());
//     ESP_ERROR_CHECK(esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE));

// #if CONFIG_ESPNOW_ENABLE_LONG_RANGE
//     ESP_ERROR_CHECK(
//         esp_wifi_set_protocol(ESPNOW_WIFI_IF, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR));
// #endif

//     // -----init espnow-----
//     /* Initialize ESPNOW and register sending and receiving callback function. */
//     ESP_ERROR_CHECK(esp_now_init());
// #if CONFIG_ESPNOW_ENABLE_POWER_SAVE
//     ESP_ERROR_CHECK(esp_now_set_wake_window(CONFIG_ESPNOW_WAKE_WINDOW));
//     ESP_ERROR_CHECK(esp_wifi_connectionless_module_set_wake_interval(CONFIG_ESPNOW_WAKE_INTERVAL));
// #endif
//     /* Set primary master key. */
//     // ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)CONFIG_ESPNOW_PMK) );

//     /* Add broadcast peer information to peer list. */
//     esp_now_peer_info_t* peer = malloc(sizeof(esp_now_peer_info_t));
//     if (peer == NULL) {
//         ESP_LOGE(TAG, "Malloc peer information fail");
//         esp_now_deinit();
//         esp_restart();
//     }
//     memset(peer, 0, sizeof(esp_now_peer_info_t));
//     peer->channel = ESPNOW_CHANNEL;
//     peer->ifidx = ESP_IF_WIFI_AP;
//     peer->encrypt = false;
//     memcpy(peer->peer_addr, s_example_broadcast_mac, ESP_NOW_ETH_ALEN);
//     ESP_ERROR_CHECK(esp_now_add_peer(peer));
//     free(peer);
}