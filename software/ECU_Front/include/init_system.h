#ifndef INIT_SYSTEM_H
#define INIT_SYSTEM_H

#include "communication.h"

/**
 * @brief init system handlers such as queues and semaphores
 * 
 */
void init_handlers(void){
    // -----create semaphores-----
    sh_SD = xSemaphoreCreateMutex();
    if(sh_SD == NULL)
        ERROR("ERROR_1: SD Semaphore init failed", false);

    sh_global_vars = xSemaphoreCreateMutex();
    if(sh_global_vars == NULL)
        ERROR("ERROR_1: global vars Semaphore init failed", false);

    // -----create queues-----
    qh_rpm        = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_speed      = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_fuel_level = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_fuel_emer  = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_battery    = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_rollover   = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_tilt_x     = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_tilt_y     = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
    qh_tilt_z     = xQueueCreate(QUEUE_BUFFER_SIZE, sizeof(sensor_t));
}


#endif // INIT_SYSTEM_H