// power/power_manager.h
#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include "config.h"

class PowerManager {
public:
    void enterLowPowerMode();
    void wakeUp();
    float getBatteryLevel();
    bool isLowBattery();

private:
    static constexpr uint32_t STARTUP_DELAY_MS = 100;
    static constexpr float LOW_BATTERY_THRESHOLD = 20.0;
    static constexpr int BATTERY_SAMPLES = 5;
    static constexpr uint32_t ADC_DELAY_MS = 5;
    static constexpr uint32_t VOLTAGE_SETTLE_MS = 10;
};
#endif // POWER_MANAGER_H