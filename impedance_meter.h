// sensors/impedance_meter.h
//#ifndef IMPEDANCE_METER_H
//#define IMPEDANCE_METER_H

#include "main.h"
#include <AD5933.h>

class ImpedanceMeter {
public:
    bool initialize();
    int getMoisture(double gain, int Cmin, int Cmax, float temp);

private:
    double measureImpedance(double gain);
    float tempCompensation(float capacitance, float temp);
    
    static constexpr uint32_t START_FREQ = 99930;
    static constexpr uint16_t FREQ_INCR = 10;
    static constexpr uint8_t NUM_INCR = 12;
    static constexpr uint8_t NUM_SAMPLES = 5;
    static constexpr float TEMP_COEFF = 0.02;
    static constexpr float REF_TEMP = 25.0;
};