#include "espnow_callback.h"

void register_callbacks(void) {
    ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_callback));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_callback));
}

void espnow_send_callback(const uint8_t* mac_addr, esp_now_send_status_t status) {
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

void espnow_recv_callback(const esp_now_recv_info_t* recv_info, const uint8_t* data, int len) {
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