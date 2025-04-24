/**
 * @file task_blind_spot.h
 * @author Flaviano Medeiros da Silva Júnior (flaviano.medeiros@estudante.ufcg.edu.br)
 * @brief task blind spot
 * @version 0.1
 * @date 2024-01-22
 *
 * @copyright Copyright (c) 2024
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Flaviano Jr.  22/01/2024  first version
 * 
 */

#pragma once

#include "system.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_BLIND_SPOT_SEND_RATE_Hz 1 // RPM task send rate in hertz

#define ACTIVE_LOW false
#define BLIND_SPOT_AVERAGE_POINTS 5
#define THRESHOLD 1 // to be calibrated

void task_blind_spot(void*);

#ifdef __cplusplus
}
#endif