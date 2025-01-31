idf_component_register(
    SRCS "src/MPU6050.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_timer
)