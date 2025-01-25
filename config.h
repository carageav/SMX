// src/config/config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Pin Definitions
namespace Pins {
    constexpr uint8_t EN = WB_SW1;
    constexpr uint8_t LOW_DIV = WB_IO2;
    //constexpr uint8_t MEM_EN = WB_IO3;
    constexpr uint8_t BATT = WB_A0;
    constexpr uint8_t C_SEL = 0;
    constexpr uint8_t EN_SEL = 1;
}

// Battery Configuration
namespace BatteryConfig {
    constexpr uint16_t VMAX = 4600;        // Maximum battery voltage in mV
    constexpr uint16_t VMIN = 3300;        // Minimum battery voltage in mV
    constexpr float MV_PER_LSB = 0.73242188F;
    constexpr float DIVIDER_COMP = 1.73F;
    constexpr float REAL_MV_PER_LSB = DIVIDER_COMP * MV_PER_LSB;
}


// System Constants
namespace SystemConstants {
    constexpr uint8_t RETRY_COUNT_MAX = 3;
    constexpr uint32_t MIN_TO_MS(uint32_t minutes) { return minutes * 30 * 1000; }
}


// EEPROM Configuration - Remove duplicate definitions
namespace EEPROMConfig {
    //constexpr uint8_t ADDR = 0x50;           // Add this back
    //constexpr uint32_t MAX_ADDR = 262143;
    constexpr uint8_t EEPROM_SIZE = 2;
    constexpr uint16_t GAIN_L_ADDR = 0;
    constexpr uint16_t GAIN_H_ADDR = 10;
    constexpr uint16_t CMAX_L_ADDR = 20;
    constexpr uint16_t CMAX_H_ADDR = 30;
    constexpr uint16_t CMIN_L_ADDR = 40;
    constexpr uint16_t CMIN_H_ADDR = 50;
    constexpr uint16_t SNR_ADDR = 60;
    constexpr uint16_t SLEEP_TIME_ADDR = 70;
}

// System States
enum class SystemState {
    INIT,
    MEASUREMENT,
    TRANSMIT,
    SLEEP
};

// Sensor Configuration
struct SensorConfig {
    double gainL;
    double gainH;
    uint16_t CminL;
    uint16_t CmaxL;
    uint16_t CminH;
    uint16_t CmaxH;
    uint16_t SNr;
    uint8_t DS_min;
};

#endif // CONFIG_H