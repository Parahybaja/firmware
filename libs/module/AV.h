/**
 * @file module_AFV.cpp
 * @authors
 *      Jefferson Lopes (jefferson.lopes@ee.ufcg.edu.br)
 * @brief AFV (Acceleration and Final Velocity) integration with embedded system
 * @version 0.2
 * @date 2023-08-02
 *
 * @copyright Copyright (c) 2023
 *
 * Version   Modified By   Date        Comments
 * -------  -------------  ----------  -----------
 *  0.1.0    Jefferson L.  25/04/2023  first version based on https://github.com/Parahybaja/Validation_AV/commit/673b40ee1af3f15fa4ccb54fb75f7474e30bd8f5
 *  0.2.0    Jefferson L.  02/08/2023  create reset_time_counter function
 */

#ifndef __AFV_H__
#define __AFV_H__

#include "system.h"

#define BOARD_01 1
#define BOARD_02 2
#define BOARD_03 3
#define CMD_NEWLINE 'Z'

#define DEFAULT_DIST 50 // default distance between sensors in cm

extern const uint8_t board_id;

typedef enum {
    AFV_START,  /*empty command*/
    AFV_FINISH, /*start command*/
    AFV_SPEED,  /*stop command*/
} afv_mode_t;

typedef struct setup_t {
    float distance = DEFAULT_DIST;
    bool active = false;
} setup_t;

typedef struct board_t {
    uint8_t id = board_id;
    afv_mode_t mode = afv_mode_t(board_id - 1);
    float speed;
} board_t;

volatile uint32_t initial_time = false;
volatile uint32_t final_time = false;

char str_buffer[64];

void reset_time_counter(void);

void reset_time_counter(void) {
    initial_time = false;
    final_time = false;
}

#endif // __AFV_H__