#pragma once

#include "system.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"

#define MOSI_PIN 23 
#define MISO_PIN 19
#define SCK_PIN 18
#define CS_PIN 5

void task_sdlogger(void*);

