// power_monitor.h
#ifndef POWER_MONITOR_H
#define POWER_MONITOR_H

#include "main.h"

class PowerMonitor {
public:
    static void init();
    static uint32_t getCurrentDraw();
    static void printPowerStatus(const char* label);
};

#endif