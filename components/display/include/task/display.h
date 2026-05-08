#pragma once

#include "system.h"
#include "driver/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NEX_VAR_SPEED    "page0.speed"
#define NEX_VAR_RPM      "page0.rpm"
#define NEX_VAR_TEMP     "page0.va_temp"
#define NEX_VAR_BATTERY  "page0.va_bat"
#define NEX_VAR_ROLL     "page0.va_roll"
#define NEX_VAR_PITCH    "page0.va_pitch"
#define NEX_VAR_FUEL     "page0.va_fuel"
#define NEX_VAR_4X4      "page0.va_4x4"

void task_display(void* arg);
void sendNextionInt(const char* obj, int val);

#ifdef __cplusplus
}
#endif