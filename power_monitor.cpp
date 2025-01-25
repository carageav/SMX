// power_monitor.cpp
#include "power_monitor.h"

void PowerMonitor::init() {
    pinMode(WB_A0, INPUT);
}

uint32_t PowerMonitor::getCurrentDraw() {
    uint32_t sum = 0;
    for(int i = 0; i < 10; i++) {
        sum += analogRead(WB_A0);
        delay(1);
    }
    return sum / 10;
}

void PowerMonitor::printPowerStatus(const char* label) {
    uint32_t current = getCurrentDraw();
    Serial.print(label);
    Serial.print(" Current: ");
    Serial.println(current);
}