// power/power_manager.cpp
#include "power_manager.h"
#include <Wire.h>

void PowerManager::enterLowPowerMode() {
    // Disable I2C
    Wire.end();
    
    // Configure pins for low power
    pinMode(Pins::EN, INPUT);
    pinMode(Pins::LOW_DIV, INPUT);
    
    digitalWrite(Pins::EN, LOW);
    digitalWrite(Pins::LOW_DIV, HIGH);
    
    // Enter low power mode
    sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
}

void PowerManager::wakeUp() {
    // Configure pins for normal operation
    pinMode(Pins::EN, OUTPUT);
    pinMode(Pins::LOW_DIV, OUTPUT);
    
    digitalWrite(Pins::EN, HIGH);
    digitalWrite(Pins::LOW_DIV, LOW);
    
    // Restart I2C
    Wire.begin();
    
    // Wait for system stabilization
    delay(STARTUP_DELAY_MS);
}

float PowerManager::getBatteryLevel() {
    float voltage = 0;
    
    // Configure ADC
    analogReference(AR_INTERNAL_3_0);
    analogReadResolution(12);
    
    // Prepare for measurement
    digitalWrite(Pins::LOW_DIV, LOW);
    delay(VOLTAGE_SETTLE_MS);
    
    // Take multiple samples
    for (int i = 0; i < BATTERY_SAMPLES; i++) {
        voltage += analogRead(Pins::BATT) * BatteryConfig::REAL_MV_PER_LSB;
        delay(ADC_DELAY_MS);
    }
    voltage /= BATTERY_SAMPLES;
    
    // Restore pin state
    digitalWrite(Pins::LOW_DIV, HIGH);
    
    // Convert to percentage
    float percentage = ((voltage - BatteryConfig::VMIN) / 
                       (BatteryConfig::VMAX - BatteryConfig::VMIN)) * 100;
    return constrain(percentage, 0.0f, 100.0f);
}

bool PowerManager::isLowBattery() {
    return getBatteryLevel() < LOW_BATTERY_THRESHOLD;
}
