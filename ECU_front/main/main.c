/**
 * @file main.h
 * @authors
 *      Jefferson Lopes (jefferson.lopes@ufcg.edu.br)
 * @brief Parahybaja's embedded system: ECU Front
 * @version 5.0
 * @date 2024-01-15
 *
 * @copyright Copyright (c) 2024
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  5.0.0    Jefferson L.  15/01/2024  convertion to esp-idf
 *
 */

#include <stdio.h>

#include "string.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"

#include "system.h"
#include "task/alive.h"
#include "task/task_example.h"

static const char* TAG = "ECU_front";

#define ESPNOW_CHANNEL 1

enum {
    EXAMPLE_ESPNOW_DATA_BROADCAST,
    EXAMPLE_ESPNOW_DATA_UNICAST,
    EXAMPLE_ESPNOW_DATA_MAX,
};

static uint8_t s_example_broadcast_mac[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint16_t s_example_espnow_seq[EXAMPLE_ESPNOW_DATA_MAX] = {0, 0};

static void example_espnow_send_cb(const uint8_t*, esp_now_send_status_t);
static void example_espnow_recv_cb(const esp_now_recv_info_t*, const uint8_t*, int);

void app_main(void) {
    ESP_LOGD(TAG, "ECU front v5");

    uint8_t mac[6];
    esp_err_t ret = esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "MAC Address: " MACSTR "", MAC2STR(mac));
    } else {
        ESP_LOGE(TAG, "Failed to get MAC address");
    }

    // -----init wifi-----
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
    ESP_ERROR_CHECK(esp_now_register_send_cb(example_espnow_send_cb));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(example_espnow_recv_cb));
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
    memcpy(peer->peer_addr, s_example_broadcast_mac, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(peer));
    free(peer);

    
    // -----fire up tasks-----
    xTaskCreatePinnedToCore(task_example,  // task function
                            "example",     // task name
                            2048,          // stack size
                            NULL,          // parameters
                            10,            // priority
                            &th_example,   // handler
                            APP_CPU_NUM    // core number
    );

    gpio_num_t gpio_pin = GPIO_NUM_12;
    xTaskCreatePinnedToCore(task_alive_LED,   // task function
                            "alive LED",      // task name
                            2048,             // stack size
                            (void*)gpio_pin,  // parameters
                            8,                // priority
                            &th_alive,        // handler
                            APP_CPU_NUM       // core number
    );
}

/* ESPNOW sending or receiving callback function is called in WiFi task.
 * Users should not do lengthy operations from this task. Instead, post
 * necessary data to a queue and handle it from a lower priority task. */
static void example_espnow_send_cb(const uint8_t* mac_addr, esp_now_send_status_t status) {
    // example_espnow_event_t evt;
    // example_espnow_event_send_cb_t *send_cb = &evt.info.send_cb;

    // if (mac_addr == NULL) {
    //     ESP_LOGE(TAG, "Send cb arg error");
    //     return;
    // }

    // evt.id = EXAMPLE_ESPNOW_SEND_CB;
    // memcpy(send_cb->mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
    // send_cb->status = status;
    // if (xQueueSend(s_example_espnow_queue, &evt, ESPNOW_MAXDELAY) != pdTRUE) {
    //     ESP_LOGW(TAG, "Send send queue fail");
    // }
}

static void example_espnow_recv_cb(const esp_now_recv_info_t* recv_info, const uint8_t* data, int len) {
    // example_espnow_event_t evt;
    // example_espnow_event_recv_cb_t *recv_cb = &evt.info.recv_cb;
    // uint8_t * mac_addr = recv_info->src_addr;
    // uint8_t * des_addr = recv_info->des_addr;

    // if (mac_addr == NULL || data == NULL || len <= 0) {
    //     ESP_LOGE(TAG, "Receive cb arg error");
    //     return;
    // }

    // if (IS_BROADCAST_ADDR(des_addr)) {
    //     /* If added a peer with encryption before, the receive packets may be
    //      * encrypted as peer-to-peer message or unencrypted over the broadcast channel.
    //      * Users can check the destination address to distinguish it.
    //      */
    //     ESP_LOGD(TAG, "Receive broadcast ESPNOW data");
    // } else {
    //     ESP_LOGD(TAG, "Receive unicast ESPNOW data");
    // }

    // evt.id = EXAMPLE_ESPNOW_RECV_CB;
    // memcpy(recv_cb->mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
    // recv_cb->data = malloc(len);
    // if (recv_cb->data == NULL) {
    //     ESP_LOGE(TAG, "Malloc receive data fail");
    //     return;
    // }
    // memcpy(recv_cb->data, data, len);
    // recv_cb->data_len = len;
    // if (xQueueSend(s_example_espnow_queue, &evt, ESPNOW_MAXDELAY) != pdTRUE) {
    //     ESP_LOGW(TAG, "Send receive queue fail");
    //     free(recv_cb->data);
    // }
}