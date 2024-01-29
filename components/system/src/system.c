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
    .tilt_z = 0.0,
    .blind_spot_l = 0.0,
    .blind_spot_r = 0.0
};

// -----FreeRTOS objects-----
TaskHandle_t th_example;
TaskHandle_t th_lora;
TaskHandle_t th_alive;
TaskHandle_t th_rpm;
TaskHandle_t th_fuel_em;
TaskHandle_t th_speed;
TaskHandle_t th_rollover;
TaskHandle_t th_battery;
TaskHandle_t th_blind_spot;
TaskHandle_t th_display_nextion;
TaskHandle_t th_display_LCD;
TaskHandle_t th_telemetry;
SemaphoreHandle_t sh_global_vars;

// -----esp-now addresses-----
const uint8_t mac_address_TCU[]       = {0xC8, 0xF0, 0x9E, 0x31, 0x8C, 0xA0};
const uint8_t mac_address_ECU_front[] = {0x94, 0xB5, 0x55, 0x2D, 0x1A, 0x61};
const uint8_t mac_address_ECU_rear[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8C, 0x11};
const uint8_t mac_address_module_1[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8D, 0x38};
const uint8_t mac_address_module_2[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8A, 0xD8};
const uint8_t mac_address_module_3[]  = {0xC8, 0xF0, 0x9E, 0x31, 0x8B, 0xA0};

bool lora_initialized_flag;

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

void system_espnow_init(void) {
    // -----init wifi-----
    ESP_ERROR_CHECK(nvs_flash_init()); // bug fix: https://github.com/espressif/arduino-esp32/issues/761
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE));

#if CONFIG_ESPNOW_ENABLE_LONG_RANGE
    ESP_ERROR_CHECK(
        esp_wifi_set_protocol(ESPNOW_WIFI_IF, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR));
#endif

    // -----init espnow-----
    /* Initialize ESPNOW and register sending and receiving callback function. */
    ESP_ERROR_CHECK(esp_now_init());
#if CONFIG_ESPNOW_ENABLE_POWER_SAVE
    ESP_ERROR_CHECK(esp_now_set_wake_window(CONFIG_ESPNOW_WAKE_WINDOW));
    ESP_ERROR_CHECK(esp_wifi_connectionless_module_set_wake_interval(CONFIG_ESPNOW_WAKE_INTERVAL));
#endif
    /* Set primary master key. */
    // ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)CONFIG_ESPNOW_PMK) );

    /* Add broadcast peer information to peer list. */
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

void system_lora_init(void) {
    ESP_LOGW(TAG, "CS: %i", CONFIG_CS_GPIO);
    ESP_LOGW(TAG, "RST: %i", CONFIG_RST_GPIO);

    if (lora_init() == 0) {
		ESP_LOGE(TAG, "Does not recognize the module");

        return;
	}

    ESP_LOGI(TAG, "Frequency is 915MHz");
    lora_set_frequency(915e6); // 915MHz

    lora_enable_crc();

    int cr = 1; // coding rate
	int bw = 7; // bandwidth
	int sf = 7; // spreading factor rate
    
    lora_set_coding_rate(cr);
	ESP_LOGI(TAG, "coding_rate=%d", cr);

	lora_set_bandwidth(bw);
	ESP_LOGI(TAG, "bandwidth=%d", bw);

	lora_set_spreading_factor(sf);
	ESP_LOGI(TAG, "spreading_factor=%d", sf);

    lora_initialized_flag = true;
}

void task_lora_sender(void *arg) {
    (void)arg;

	ESP_LOGI(TAG, "Start lora sender task");
	uint8_t buf[256]; // Maximum Payload size of SX1276/77/78/79 is 255
    TickType_t nowTick;
    int send_len;
    int lost;

    // show remaining task space
    print_task_remaining_space();

	for (;;) {
		nowTick = xTaskGetTickCount();
		
        send_len = sprintf((char *)buf,"Hello World!! %"PRIu32, nowTick);
		lora_send_packet(buf, send_len);
		ESP_LOGI(TAG, "%d byte packet sent...", send_len);
		
        lost = lora_packet_lost();
		if (lost != 0) {
			ESP_LOGW(TAG, "%d packets lost", lost);
		}

		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}

void task_lora_receiver(void *arg) {
    (void)arg;

	ESP_LOGI(TAG, "Start lora receiver task");
	uint8_t buf[256]; // Maximum Payload size of SX1276/77/78/79 is 255
	int rxLen;

    // show remaining task space
    print_task_remaining_space();

    for (;;) {
		lora_receive(); // put into receive mode
		
        if (lora_received()) {
			rxLen = lora_receive_packet(buf, sizeof(buf));
			ESP_LOGI(TAG, "%d byte packet received:[%.*s]", rxLen, rxLen, buf);
		}
		
        vTaskDelay(pdMS_TO_TICKS(10)); // Avoid WatchDog alerts
	}
}