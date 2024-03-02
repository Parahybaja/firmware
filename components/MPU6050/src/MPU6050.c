#include "driver/MPU6050.h"

static const char *TAG = "MPU";

/*-----MPU configs-----*/
uint8_t upside_down_mounting = 0;
float gyro_lsb_to_degsec, acc_lsb_to_g;
float gyroXoffset, gyroYoffset, gyroZoffset;
float accXoffset, accYoffset, accZoffset;

/*-----result variables-----*/
float temp, acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z;
float angle_acc_x, angle_acc_y;
float angle_x, angle_y, angle_z;
int64_t pre_interval;
float filter_gyro_coef;

/**
 * @brief Wrap an angle in the range [-limit,+limit]
 * 
 * @param angle 
 * @param limit 
 * @return float 
 */
static float wrap(float angle, float limit){
    while (angle >  limit) angle -= 2*limit;
    while (angle < -limit) angle += 2*limit;
    return angle;
}

void i2c_init(void) {
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

esp_err_t i2c_write_data(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, uint8_t value) {
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

esp_err_t mpu_init(void) {
    i2c_init();

    esp_err_t ret = mpu_write_data(MPU6050_PWR_MGMT_1_REGISTER, 0x01);

    if (ret != ESP_OK) {
        return ret;
    }

    mpu_write_data(MPU6050_SMPLRT_DIV_REGISTER, 0x00);
    mpu_write_data(MPU6050_CONFIG_REGISTER, 0x00);

    mpu_write_data(MPU6050_GYRO_CONFIG_REGISTER, 0x00);
    mpu_write_data(MPU6050_ACCEL_CONFIG_REGISTER, 0x00);

    vTaskDelay(100 / portTICK_PERIOD_MS);

    mpu_set_acc_config(0);
    mpu_set_gyro_config(0);

    return ret;
}

esp_err_t mpu_write_data(uint8_t reg, uint8_t value) {
    return i2c_write_data(I2C_MASTER_PORT_NUM, MPU6050_ADDR, reg, value);
}

esp_err_t mpu_set_acc_config(uint8_t config_num){
    esp_err_t status;
    switch(config_num){
        case 0: // range = +- 2 g
            acc_lsb_to_g = 16384.0;
            status = mpu_write_data(MPU6050_ACCEL_CONFIG_REGISTER, 0x00);
            break;
        case 1: // range = +- 4 g
            acc_lsb_to_g = 8192.0;
            status = mpu_write_data(MPU6050_ACCEL_CONFIG_REGISTER, 0x08);
            break;
        case 2: // range = +- 8 g
            acc_lsb_to_g = 4096.0;
            status = mpu_write_data(MPU6050_ACCEL_CONFIG_REGISTER, 0x10);
            break;
        case 3: // range = +- 16 g
            acc_lsb_to_g = 2048.0;
            status = mpu_write_data(MPU6050_ACCEL_CONFIG_REGISTER, 0x18);
            break;
        default: // error
            status = ESP_FAIL;
            break;
    }
    return status;
}

esp_err_t mpu_set_gyro_config(uint8_t config_num){
    esp_err_t status;
    switch(config_num){
        case 0: // range = +- 250 deg/s
            gyro_lsb_to_degsec = 131.0;
            status = mpu_write_data(MPU6050_GYRO_CONFIG_REGISTER, 0x00);
            break;
        case 1: // range = +- 500 deg/s
            gyro_lsb_to_degsec = 65.5;
            status = mpu_write_data(MPU6050_GYRO_CONFIG_REGISTER, 0x08);
            break;
        case 2: // range = +- 1000 deg/s
            gyro_lsb_to_degsec = 32.8;
            status = mpu_write_data(MPU6050_GYRO_CONFIG_REGISTER, 0x10);
            break;
        case 3: // range = +- 2000 deg/s
            gyro_lsb_to_degsec = 16.4;
            status = mpu_write_data(MPU6050_GYRO_CONFIG_REGISTER, 0x18);
            break;
        default: // error
            status = ESP_FAIL;
            break;
    }
    
    return status;
}

esp_err_t mpu_fetch(void) {
    uint8_t raw_data[14] = {};

    esp_err_t ret = i2c_read_data(I2C_MASTER_PORT_NUM, MPU6050_ADDR, MPU6050_ACCEL_OUT_REGISTER, raw_data, sizeof(raw_data));

    /*--------Bytes of acc--------*/
    int16_t raw_acc_x = (raw_data[0] << 8) + raw_data[1];
    int16_t raw_acc_y = (raw_data[2] << 8) + raw_data[3];
    int16_t raw_acc_z = (raw_data[4] << 8) + raw_data[5];

    acc_x = ((float)raw_acc_x) / acc_lsb_to_g;
    acc_y = ((float)raw_acc_y) / acc_lsb_to_g;
    acc_z = (!upside_down_mounting - upside_down_mounting) * ((float)raw_acc_z) / acc_lsb_to_g;

    /*--------Bytes of temp--------*/
    int16_t raw_temp = (raw_data[6] << 8) + raw_data[7];

    temp = ((float)raw_temp + TEMP_LSB_OFFSET) / TEMP_LSB_2_DEGREE;

    /*--------Bytes of gyro--------*/
    int16_t raw_gyro_x = (raw_data[8] << 8) + raw_data[9];
    int16_t raw_gyro_y = (raw_data[10] << 8) + raw_data[11];
    int16_t raw_gyro_z = (raw_data[12] << 8) + raw_data[13];

    gyro_x = ((float)raw_gyro_x) / gyro_lsb_to_degsec;
    gyro_y = ((float)raw_gyro_y) / gyro_lsb_to_degsec;
    gyro_z = ((float)raw_gyro_z) / gyro_lsb_to_degsec;

    return ret;
}

esp_err_t mpu_update(void) {
    // retrieve raw data
    mpu_fetch();
    
    // estimate tilt angles: this is an approximation for small angles!
    float sg_z = acc_z<0 ? -1 : 1; // allow one angle to go from -180 to +180 degrees
    angle_acc_x =   atan2(acc_y, sg_z*sqrt(acc_z*acc_z + acc_x*acc_x)) * RAD_2_DEG; // [-180,+180] deg
    angle_acc_y = - atan2(acc_x,      sqrt(acc_z*acc_z + acc_y*acc_y)) * RAD_2_DEG; // [- 90,+ 90] deg

    int64_t new_time = esp_timer_get_time();
    float dt = (new_time - pre_interval) / US_TO_S;
    pre_interval = new_time;

    angle_x = wrap(filter_gyro_coef*(angle_acc_x + wrap(angle_x +      gyro_x*dt - angle_acc_x,180)) + (1.0-filter_gyro_coef)*angle_acc_x,180);
    angle_y = wrap(filter_gyro_coef*(angle_acc_y + wrap(angle_y + sg_z*gyro_y*dt - angle_acc_y, 90)) + (1.0-filter_gyro_coef)*angle_acc_y, 90);
    angle_z += gyro_z * dt; // not wrapped

    return ESP_OK;
}

float mpu_get_angle_x(void) { return angle_x;}

float mpu_get_angle_y(void) { return angle_y;}

float mpu_get_angle_z(void) { return angle_z;}

float mpu_get_temp(void) {return temp;}
