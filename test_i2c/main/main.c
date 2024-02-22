#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"


static const char *TAG = "i2c-simple-example";


#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define MPU6050_ADDR                0x68
#define MPU6050_SMPLRT_DIV_REGISTER   0x19
#define MPU6050_CONFIG_REGISTER       0x1a
#define MPU6050_GYRO_CONFIG_REGISTER  0x1b
#define MPU6050_ACCEL_CONFIG_REGISTER 0x1c
#define MPU6050_PWR_MGMT_1_REGISTER   0x6b
#define MPU6050_GYRO_OUT_REGISTER     0x43
#define MPU6050_ACCEL_OUT_REGISTER    0x3B

// #define ACK_CHECK_EN 0x1            /*!< I2C master will check ack from slave*/
// #define WRITE_BIT I2C_MASTER_WRITE  /*!< I2C master write */
// #define READ_BIT I2C_MASTER_READ    /*!< I2C master read */
// #define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
// #define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
// #define SCL_IO_PIN CONFIG_I2C_MASTER_SCL
// #define SDA_IO_PIN CONFIG_I2C_MASTER_SDA
// #define MASTER_FREQUENCY CONFIG_I2C_MASTER_FREQUENCY
// #define PORT_NUMBER -1
// #define LENGTH 48
// #define TAG "Test"
// #define BUFF_SIZE 16

static gpio_num_t i2c_gpio_sda = 18;
static gpio_num_t i2c_gpio_scl = 19;
static uint32_t i2c_frequency = 100000;

static i2c_port_t i2c_port = I2C_NUM_0;

uint8_t write_buf[2] = {};

float acc_lsb_to_g = 16384.0;

// uint8_t data[BUFF_SIZE];

// static esp_err_t i2c_get_port(int port, i2c_port_t *i2c_port)
// {
//     if (port >= I2C_NUM_MAX) {
//         ESP_LOGE(TAG, "Wrong port number: %d", port);
//         return ESP_FAIL;
//     }
//     *i2c_port = port;
//     return ESP_OK;
// }

// static esp_err_t i2c_master_driver_initialize(void)
// {
//     i2c_config_t conf = {
//         .mode = I2C_MODE_MASTER,
//         .sda_io_num = i2c_gpio_sda,
//         .sda_pullup_en = GPIO_PULLUP_ENABLE,
//         .scl_io_num = i2c_gpio_scl,
//         .scl_pullup_en = GPIO_PULLUP_ENABLE,
//         .master.clk_speed = i2c_frequency,
//         // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
//     };
//     return i2c_param_config(i2c_port, &conf);
// }

// static int do_i2cdetect_cmd()
// {
//     i2c_driver_install(i2c_port, I2C_MODE_MASTER, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
//     i2c_master_driver_initialize();
//     uint8_t address;
//     printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");

//     for (int i = 0; i < 128; i += 16) {
//         printf("%02x: ", i);
//         for (int j = 0; j < 16; j++) {
//             fflush(stdout);
//             address = i + j;
//             i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//             i2c_master_start(cmd);
//             i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, ACK_CHECK_EN);
//             i2c_master_stop(cmd);
//             esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 50 / portTICK_PERIOD_MS);
//             i2c_cmd_link_delete(cmd);

//             if (ret == ESP_OK) {
//                 printf("%02x ", address);
//                 i2c_master_read_byte(cmd,data, I2C_MASTER_LAST_NACK);
//                 //printf("Dados lidos: 0x%02x\n", data[0]);

//             } else if (ret == ESP_ERR_TIMEOUT) {
//                 printf("UU ");
//             } else {
//                 printf("-- ");
//             }
//         }
//         printf("\r\n");
//     }

//     i2c_driver_delete(i2c_port);
//     return 0;
// }

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void) {
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = i2c_gpio_sda,
        .scl_io_num = i2c_gpio_scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void app_main(void){
    // do_i2cdetect_cmd();
    
    uint8_t data[2] = {0x00, 0x01};
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");


    int ret;
    // begin ----------------------------------------
    write_buf[0] = MPU6050_PWR_MGMT_1_REGISTER;
    write_buf[1] = 0x01;
    ret = i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    ESP_LOGW(TAG, "i2c return code: %i", ret);

    write_buf[0] = MPU6050_SMPLRT_DIV_REGISTER;
    write_buf[1] = 0x00;
    ret = i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    ESP_LOGW(TAG, "i2c return code: %i", ret);

    write_buf[0] = MPU6050_CONFIG_REGISTER;
    write_buf[1] = 0x00;
    ret = i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    ESP_LOGW(TAG, "i2c return code: %i", ret);
    // ------------------------------------------------

    for (;;) {
        // fetchdata -------------------------------------
        write_buf[0] = MPU6050_ACCEL_OUT_REGISTER;
        write_buf[1] = 0x01;
        // ret = i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
        // ESP_LOGW(TAG, "i2c return code: %i", ret);
        
        uint8_t rawData[14];
        i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDR, write_buf, sizeof(write_buf), rawData, 14, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

        int16_t raw2;
        raw2 = rawData[0] << 8;
        raw2 |= rawData[1];
        float acc_x = (float)raw2;
        acc_x /= acc_lsb_to_g ;
        ESP_LOGI(TAG, "acc_x: %f", acc_x);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}