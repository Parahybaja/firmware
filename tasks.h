#ifndef TASKS_H
#define TASKS_H

#include "definitions.h"

void init_tasks(void);
void task_alive(void*);

/**
 * @brief init run time tasks
 * 
 */
void init_tasks(void){
//     xTaskCreate(task_alive,   // task function
//                 "task_alive", // task name
//                 2048,         // stack size
//                 NULL,         // parameters
//                 10,           // priority
//                 &th_alive);   // handler 

//     xTaskCreate(task_display_control, // task function
//                 "display control",    // task name
//                 2048,                 // stack size
//                 NULL,                 // parameters
//                 10,                   // priority
//                 &th_display_control); // handler 

//     // xTaskCreate(task_battery,   // task function
//     //             "task_battery", // task name
//     //             2048,           // stack size
//     //             NULL,           // parameters
//     //             10,             // priority
//     //             &th_battery);   // handler  

//     // xTaskCreate(task_rollover,  // task function
//     //             "task_rollover",// task name
//     //             2048,           // stack size
//     //             NULL,           // parameters
//     //             10,             // priority
//     //             &th_rollover);  // handler 

//     xTaskCreate(task_send_pack, // task function
//                 "send pack",    // task name
//                 2048,           // stack size
//                 NULL,           // parameters
//                 10,             // priority
//                 &th_send_pack); // handler 

//     // xTaskCreate(task_write_SD, // task function
//     //             "write to SD", // task name
//     //             8048,          // stack size
//     //             NULL,          // parameters
//     //             10,            // priority
//     //             &th_SD);       // handler
}

/**
 * @brief alive signal
 * 
 * @param arg void arg
 */
void task_alive(void *arg){
    (void)arg;

    
    
    while (true){
        
    }
}

#endif // TASK