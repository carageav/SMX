// sensors/temperature.h
//#ifndef TEMPERATURE_H
//#define TEMPERATURE_H

#include "main.h"

class TemperatureSensor {
public:
    bool initialize();
    int getTemperature();
    void sleep();
    void wakeup();

private:
    static constexpr float TEMP_OFFSET = 0.5;
};