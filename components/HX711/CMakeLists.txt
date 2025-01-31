set(req driver freertos esp_timer esp_rom)
idf_component_register(
    SRCS "src/HX711.cpp"
    INCLUDE_DIRS "include"
    REQUIRES ${req}
)