#include "task/sd_logger.h"

static const char *TAG = "sd_logger";

void task_sdlogger(void *arg) {
    //gpio_set_pull_mode(MOSI_PIN, GPIO_PULLUP_ONLY);
    //gpio_set_pull_mode(MISO_PIN, GPIO_PULLUP_ONLY);
    //gpio_set_pull_mode(SCK_PIN, GPIO_PULLUP_ONLY);
    //gpio_set_pull_mode(CS_PIN, GPIO_PULLUP_ONLY);

    sdmmc_host_t host_config = SDSPI_HOST_DEFAULT();
    host_config.max_freq_khz = 5000;
    
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = MOSI_PIN,
        .miso_io_num = MISO_PIN,
        .sclk_io_num = SCK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
        .disk_status_check_enable = true
    };

    spi_bus_initialize(host_config.slot, &bus_cfg, SDSPI_DEFAULT_DMA);

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = CS_PIN;
    slot_config.host_id = host_config.slot;

    vTaskDelay(pdMS_TO_TICKS(100));
    sdmmc_card_t* card;
    esp_err_t esp_err = esp_vfs_fat_sdspi_mount("/sdcard", &host_config, &slot_config, &mount_config, &card);

    if (esp_err != ESP_OK) {
        if (esp_err == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(esp_err));
        }
        return;
    }


    int file_number = 0;
    char file_name[30];
    bool file_exists = true;
    struct stat buf;
    while(file_exists == true) {
        sprintf(file_name, "/sdcard/log%d.txt", file_number);
        if(stat(file_name, &buf) == 0) {
            file_exists = true;
            file_number++;
        } else {
            file_exists = false;
        }
    }

    
    FILE* log = fopen(file_name, "a");
    if(log == NULL) {
        ESP_LOGE(TAG, "File was not able to be created.");
    }
    char log_line[256];
    int start_ms = 0;
    int end_ms = 0;
    for(;;) {
        if(start_ms == 0) {
            start_ms = esp_log_timestamp();
        }        
        end_ms = esp_log_timestamp();

        if((end_ms - start_ms) >= 3000) {
            fclose(log);
            log = fopen(file_name, "a");
            start_ms = 0;
            end_ms = 0;
            ESP_LOGI(TAG, "SD Card content flushed.");
        }
        sprintf(
            log_line,
            "DATA:%d,%d,%d,%.2f,%.2f,%d,%.2f,%.2f,%.2f,%d,%d, %lld",
            (uint16_t)system_global.rpm,
            (uint8_t)system_global.speed,
            (uint8_t)system_global.fuel_em,
            (float)system_global.battery,
            (float)system_global.temp,
            (uint8_t)system_global.rollover,
            (float)system_global.tilt_x,
            (float)system_global.tilt_y,
            (float)system_global.tilt_z,
            (uint8_t)system_global.infrared,
            (uint16_t)system_global.rotation,
            (uint64_t)esp_timer_get_time()
            );
        fprintf(log, "%s\n", log_line);
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}