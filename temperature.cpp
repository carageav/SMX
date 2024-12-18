// sensors/temperature.cpp
#include "temperature.h"

bool TemperatureSensor::initialize() {
    return STemp.begin();
}

int TemperatureSensor::getTemperature() {
    wakeup();
    int temp = int(STemp.readTempC() + TEMP_OFFSET);
    sleep();
    return temp;
}

void TemperatureSensor::sleep() {
    STemp.sleep();
}

void TemperatureSensor::wakeup() {
    STemp.wakeup();
}