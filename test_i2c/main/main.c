#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

static const char *TAG = "test-i2c";

/*----- i2c config-----*/
#define I2C_MASTER_SCL_IO 22               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 21               /*!< gpio number for I2C master data  */
#define I2C_MASTER_PORT_NUM I2C_NUM_0      /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 400e3           /*!< 100kHz - 2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0        /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0        /*!< I2C master doesn't need buffer */
#define ACK_CHECK_EN 0x1                   /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                  /*!< I2C master will not check ack from slave */

/*-----MPU6050 configs-----*/
#define MPU6050_ADDR                  0x68
#define MPU6050_SMPLRT_DIV_REGISTER   0x19
#define MPU6050_CONFIG_REGISTER       0x1a
#define MPU6050_GYRO_CONFIG_REGISTER  0x1b
#define MPU6050_ACCEL_CONFIG_REGISTER 0x1c
#define MPU6050_PWR_MGMT_1_REGISTER   0x6b
#define MPU6050_GYRO_OUT_REGISTER     0x43
#define MPU6050_ACCEL_OUT_REGISTER    0x3B
#define RAD_2_DEG             57.29578 // [deg/rad]
#define CALIB_OFFSET_NB_MES   500
#define TEMP_LSB_2_DEGREE     340.0    // [bit/celsius]
#define TEMP_LSB_OFFSET       12412.0
#define DEFAULT_GYRO_COEFF    0.98

/*-----Define value of variables-----*/
#define ACC_GRAVITY_VALUE 16384.0
#define GYRO_LSB_TO_DEGSEC 131.0

/*-------Upside Down Mounting-------*/
uint8_t upsideDownMounting 1;

/**
 * @brief Initialize I2C hardware with 400kHz clock speed
 * 
 */
void i2c_init();

/**
 * @brief Write value to device's register through I2C
 * 
 * @param i2c_num I2C port num
 * @param addr I2C device address
 * @param reg register address
 * @param value register value
 * @return esp_err_t 
 * 
 * __________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write reg + ack | write value + ack | stop |
 * |_______|___________________________|_________________|___________________|______|
*/
esp_err_t i2c_write_command(i2c_port_t, uint8_t, uint8_t, uint8_t);

/**
 * @brief Read data from register through I2C
 * 
 * @param i2c_num I2C port num
 * @param addr I2C device address
 * @param reg register
 * @param data_rd data read array
 * @param size size of data read array
 * @return esp_err_t 
 * 
 * _______________________________________________________________________________________
 * | start | slave_addr + rd_bit +ack | read n-1 bytes + ack | read 1 byte + nack | stop |
 * |_______|__________________________|______________________|____________________|______|
 */
esp_err_t i2c_read_data(i2c_port_t, uint8_t, uint8_t, uint8_t*, size_t);

/**
 * @brief Initialiaze MPU6050 and set working mode
 * 
 * @return esp_err_t 
 */
esp_err_t mpu_init();

/**
 * @brief Write value to MPU6050 register through I2C
 * 
 * @param reg MPU6050 register address
 * @param value MPU6050 register value
 * @return esp_err_t 
 */
esp_err_t mpu_write_command(uint8_t, uint8_t);

/**
 * @brief Fetch data(read raw data) from the MPU6050
 * 
 * @return esp_err_t 
 */
esp_err_t mpu_fetch();

void i2c_init() {
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,         
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,        
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ, 
        .clk_flags = 0,                        
    };
    
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_PORT_NUM, &i2c_conf));

    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_PORT_NUM, i2c_conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
}

esp_err_t i2c_write_command(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    /**
     * MPU6050 I2C single write sequence 
     * 
     * _______________________________________________________
     * | Master | S | AD+W |     | RA |     | DATA |     | P |
     * |________|___|______|_____|____|_____|______|_____|___|
     * | Slave  |   |      | ACK |    | ACK |      | ACK |   |
     * |________|___|______|_____|____|_____|______|_____|___|
     * 
     * @param S Start Condition: SDA goes from high to low while SCL is high
     * @param AD Slave I2C address
     * @param W Write bit (0)
     * @param R Read bit (1)
     * @param ACK Acknowledge: SDA line is low while the SCL line is high at the 9th clock cycle
     * @param NACK Not-Acknowledge: SDA line stays high at the 9th clock cycle
     * @param RA MPU-60X0 internal register address
     * @param DATA Transmit or received data
     * @param P Stop condition: SDA going from low to high while SCL is high
     */
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ((addr << 1) | I2C_MASTER_WRITE), ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, value, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    
    return ret;
}

esp_err_t i2c_read_data(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, uint8_t *data_rd, size_t size) {
    
    if (size == 0) {
        return ESP_OK;
    }

    /**
     * MPU6050 I2C burts read sequence 
     * 
     * _______________________________________________________________________________________
     * | Master | S | AD+W |     | RA |     | S | AD+R |      |      | ACK |      | NACK | P |
     * |________|___|______|_____|____|_____|___|______|______|______|_____|______|______|___|
     * | Slave  |   |      | ACK |    | ACK |   |      | ACK  | DATA |     | DATA |      |   |
     * |________|___|______|_____|____|_____|___|______|______|______|_____|______|______|___|
     * 
     * @param S Start Condition: SDA goes from high to low while SCL is high
     * @param AD Slave I2C address
     * @param W Write bit (0)
     * @param R Read bit (1)
     * @param ACK Acknowledge: SDA line is low while the SCL line is high at the 9th clock cycle
     * @param NACK Not-Acknowledge: SDA line stays high at the 9th clock cycle
     * @param RA MPU-60X0 internal register address
     * @param DATA Transmit or received data
     * @param P Stop condition: SDA going from low to high while SCL is high
     */
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, I2C_MASTER_ACK); // read (size - 1) bytes
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 3000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    
    return ret;
}

esp_err_t mpu_init() {
    i2c_init();

    esp_err_t ret = mpu_write_command(MPU6050_PWR_MGMT_1_REGISTER, 0x00);
    mpu_write_command(MPU6050_SMPLRT_DIV_REGISTER, 0x00);
    mpu_write_command(MPU6050_CONFIG_REGISTER, 0x00);

    mpu_write_command(MPU6050_GYRO_CONFIG_REGISTER, 0x00);
    mpu_write_command(MPU6050_ACCEL_CONFIG_REGISTER, 0x00);

    ESP_LOGW(TAG, "i2c ret=0x%x", ret);
    ESP_LOGW(TAG, "i2c ret=%i", ret);

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    return ret;
}

esp_err_t mpu_write_command(uint8_t reg, uint8_t value) {
    return i2c_write_command(I2C_MASTER_PORT_NUM, MPU6050_ADDR, reg, value);
}

esp_err_t mpu_fetch() {
    uint8_t raw_data[14] = {};

    esp_err_t ret = i2c_read_data(I2C_MASTER_PORT_NUM, MPU6050_ADDR, MPU6050_ACCEL_OUT_REGISTER, raw_data, sizeof(raw_data));

        /*--------Bytes of acc--------*/
    int16_t raw_acc_x = (raw_data[0] << 8) + raw_data[1];
    int16_t raw_acc_y = (raw_data[2] << 8) + raw_data[3];
    int16_t raw_acc_z = (raw_data[4] << 8) + raw_data[5];

    float acc_x = ((float)raw_acc_x) / ACC_GRAVITY_VALUE;
    float acc_y = ((float)raw_acc_y) / ACC_GRAVITY_VALUE;
    float acc_z = (!upsideDownMounting - upsideDownMounting) * ((float)raw_acc_z) / ACC_GRAVITY_VALUE;

    ESP_LOGI(TAG, "acc_x=%f", acc_x);
    ESP_LOGI(TAG, "acc_y=%f", acc_y);
    ESP_LOGI(TAG, "acc_z=%f", acc_z);

        /*--------Bytes of temp--------*/
    int16_t raw_temp = (raw_data[6] << 8) + raw_data[7];

    float temp = ((float)raw_temp + TEMP_LSB_OFFSET) / TEMP_LSB_2_DEGREE;

    ESP_LOGE(TAG, "temp:%f", temp);
    ESP_LOGW(TAG, "[6]:%x, [7]%x, raw:%i", raw_data[6], raw_data[7], raw_temp);
    
        /*--------Bytes of gyro--------*/
    int16_t raw_gyro_x = (raw_data[8] << 8) + raw_data[9];
    int16_t raw_gyro_y = (raw_data[10] << 8) + raw_data[11];
    int16_t raw_gyro_z = (raw_data[12] << 8) + raw_data[13];

    float gyroX = ((float)raw_gyro_x) / GYRO_LSB_TO_DEGSEC;
    float gyroY = ((float)raw_gyro_y) / GYRO_LSB_TO_DEGSEC;
    float gyroZ = ((float)raw_gyro_z) / GYRO_LSB_TO_DEGSEC;

    ESP_LOGW(TAG, "[0]:%x, [1]%x, raw:%i", raw_data[0], raw_data[1], raw_acc_x);

    return ret;
}

void app_main(void) {
    esp_err_t ret1 = mpu_init();
    esp_err_t ret2;
    uint8_t raw_data;

    if (ret1 != ESP_OK) {
        esp_restart();
    }

    for (;;) {
        ret2 = mpu_fetch();

        ESP_LOGI(TAG, "fetch ret:%x", ret2);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}