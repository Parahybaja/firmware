idf_component_register(
    SRCS "src/sd_logger.c"
    INCLUDE_DIRS "include"
    REQUIRES system fatfs vfs sdmmc esp_timer
)