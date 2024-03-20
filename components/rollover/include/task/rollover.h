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
 *  1.2.0    Jefferson L.  26/10/2023  add MPU calibration
 *  2.2.0    Jefferson L.  02/03/2024  idf convertion
 */

#pragma once

#include "system.h"
#include "driver/MPU6050.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_ROLLOVER_RATE_Hz 1 // reading rate in hertz (must be greater then send rate)

/*-----MPU configs-----*/
#define MPU_GYRO_CONFIG 3
#define MPU_ACC_CONFIG  3

/*-----MPU offset-----*/
#define CALIB_ACC_X 0.126168 // usada para definir o zero do eixo x
#define CALIB_ACC_Y -0.016679 // usada para definir o zero do eixo y
#define CALIB_ACC_Z -0.272904 // usada para definir o zero do eixo z
#define CALIB_GYRO_X -2.928174 // usada para definir o zero do eixo x
#define CALIB_GYRO_Y -0.410976 // usada para definir o zero do eixo y
#define CALIB_GYRO_Z -0.131342 // usada para definir o zero do eixo z

/**
 * @brief MPU6050 rollover sensor task
 * 
 * @param arg void arg
 */
void task_rollover(void*);

#ifdef __cplusplus
}
#endif