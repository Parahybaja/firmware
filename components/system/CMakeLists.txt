idf_component_register(
    SRCS "src/system.c"
    INCLUDE_DIRS "include"
    REQUIRES lora freertos esp_wifi esp_netif nvs_flash esp_timer
)