idf_component_register(
    SRCS "src/display.c"
    INCLUDE_DIRS "include"
    REQUIRES system esp32_driver_nextion
)