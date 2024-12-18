// sensors/impedance_meter.cpp
#include "impedance_meter.h"

bool ImpedanceMeter::initialize() {
    return (AD5933::reset() &&
            AD5933::setInternalClock(true) &&
            AD5933::setStartFrequency(START_FREQ) &&
            AD5933::setIncrementFrequency(FREQ_INCR) &&
            AD5933::setNumberIncrements(NUM_INCR) &&
            AD5933::setPGAGain(PGA_GAIN_X1));
}

double ImpedanceMeter::measureImpedance(double gain) {
    float sumMagnitude = 0;
    uint8_t validSamples = 0;
    int real, imag;

    if (!(AD5933::setPowerMode(POWER_STANDBY) &&
          AD5933::setControlMode(CTRL_INIT_START_FREQ) &&
          AD5933::setControlMode(CTRL_START_FREQ_SWEEP))) {
        return -1;
    }

    for (uint8_t sample = 0; sample < NUM_SAMPLES; sample++) {
        double magnitude = 0;
        int i = 0;

        while ((AD5933::readStatusRegister() & STATUS_SWEEP_DONE) != STATUS_SWEEP_DONE) {
            if (!AD5933::getComplexData(&real, &imag)) {
                continue;
            }

            if (i == 1) {
                magnitude = sqrt(pow(real, 2) + pow(imag, 2));
            }
            if (i >= 2) {
                double magnread = sqrt(pow(real, 2) + pow(imag, 2));
                magnitude = (magnitude + magnread) / 2;
            }
            i++;
            AD5933::setControlMode(CTRL_INCREMENT_FREQ);
        }

        if (magnitude > 0) {
            sumMagnitude += magnitude;
            validSamples++;
        }
        delay(10);
    }

    AD5933::setPowerMode(POWER_DOWN);
    //return (validSamples > 0) ? (sumMagnitude / validSamples) : -1;
    return (validSamples > 0) ? 1/((sumMagnitude / validSamples)*gain)-204 : -1;
}

int ImpedanceMeter::getMoisture(double gain, int Cmin, int Cmax, float temp) {
    double impedance = measureImpedance(gain);
    Serial.print("imped: "); Serial.println(impedance);
    if (impedance < 0) return -1;
    //float Cin=1E+12/2/M_PI/(START_FREQ+FREQ_INCR*NUM_INCR/2)/impedance;Serial.print(" Cin_flt = ");Serial.println(Cin);
    double Cin = 1E+12 / (2 * M_PI * (START_FREQ + FREQ_INCR * NUM_INCR / 2) * impedance); Serial.print("Cin_dbl: "); Serial.println(Cin);
    Cin = tempCompensation(Cin, temp);

    return constrain(abs((Cin - Cmin) * 100 / (Cmax - Cmin)), 0, 100);
}

float ImpedanceMeter::tempCompensation(float capacitance, float temp) {
    return capacitance * (1 + TEMP_COEFF * (temp - REF_TEMP));
}
