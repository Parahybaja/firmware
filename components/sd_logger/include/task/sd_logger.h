#pragma once

#include "system.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "esp_err.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "driver/spi_master.h"

#define MISO_PIN 19
#define MOSI_PIN 23
#define SCK_PIN 18
#define CS_PIN 5

void task_sdlogger(void*);

