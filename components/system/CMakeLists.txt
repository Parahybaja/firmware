idf_component_register(
    SRCS "src/system.c"
    INCLUDE_DIRS "include"
    REQUIRES freertos esp_wifi esp_netif
)