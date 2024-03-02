/**
 * @file MPU6050.h
 * @authors 
 *      Jefferson Lopes (jefferson.lopes@ee.ufcg.edu.br)
 *      Flaviano Medeiros (flaviano.medeiros@estudante.ufcg.edu.br)
 * @brief general task example
 * @version 2.2
 * @date 2024-03-02
 *
 * @copyright Copyright (c) 2024
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Jefferson L.  02/03/2024  first version (based on https://github.com/rfetick/MPU6050_light)
 */

#pragma once

#include <stdio.h>
#include <math.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----- i2c config-----*/
#define I2C_MASTER_SCL_IO 22               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 21               /*!< gpio number for I2C master data  */
#define I2C_MASTER_PORT_NUM I2C_NUM_0      /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 400e3           /*!< 400kHz - 2C master clock frequency */
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
#define US_TO_S (float)1e6

/**
 * @brief Initialize I2C hardware with 400kHz clock speed
 * 
 */
void i2c_init(void);

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
esp_err_t i2c_write_data(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, uint8_t value);

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
esp_err_t i2c_read_data(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, uint8_t *data_rd, size_t size);

/**
 * @brief Initialiaze MPU6050 and set working mode
 * 
 * @return esp_err_t 
 */
esp_err_t mpu_init(void);

/**
 * @brief Write value to MPU6050 register through I2C
 * 
 * @param reg MPU6050 register address
 * @param value MPU6050 register value
 * @return esp_err_t 
 */
esp_err_t mpu_write_data(uint8_t reg, uint8_t value);

/**
 * @brief 
 * 
 * @param config_num 
 * @return esp_err_t 
 */
esp_err_t mpu_set_acc_config(uint8_t config_num);

/**
 * @brief 
 * 
 * @param config_num 
 * @return esp_err_t 
 */
esp_err_t mpu_set_gyro_config(uint8_t config_num);

/**
 * @brief Fetch data(read raw data) from the MPU6050
 * 
 * @return esp_err_t 
 */
esp_err_t mpu_fetch(void);

/**
 * @brief 
 * 
 * @return esp_err_t 
 */
esp_err_t mpu_update(void);

/**
 * @brief returns the angle X
 * 
 * @return float 
 */
float mpu_get_angle_x(void);

/**
 * @brief returns the angle Y
 * 
 * @return float 
 */
float mpu_get_angle_y(void);

/**
 * @brief returns the angle Z
 * 
 * @return float 
 */
float mpu_get_angle_z(void);

/**
 * @brief returns the temperature
 * 
 * @return float 
 */
float mpu_get_temp(void);

#ifdef __cplusplus
}
#endif