idf_component_register(
    SRCS "src/fuel_level.c"
    INCLUDE_DIRS "include"
    REQUIRES system esp_timer driver
)