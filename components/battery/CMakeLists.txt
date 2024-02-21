idf_component_register(
    SRCS "src/battery.c"
    INCLUDE_DIRS "include"
    REQUIRES system driver esp_adc
)