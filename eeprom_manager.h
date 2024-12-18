// src/storage/eeprom_manager.h
#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include "main.h"
#include "config.h"

class EEPROMManager {
public:
    explicit EEPROMManager(ExternalEEPROM& eeprom);
    bool initialize();
    bool readConfig(SensorConfig& config);
    bool writeConfig(const SensorConfig& config);

private:
    ExternalEEPROM& eeprom;
    static constexpr uint32_t INIT_DELAY_MS = 300;
    
    // Helper methods for reading and writing different data types
    //void writeFloat(uint16_t addr, float value);
    //void writeUInt16(uint16_t addr, uint16_t value);
    //float readFloat(uint16_t addr);
    //uint16_t readUInt16(uint16_t addr);
};

#endif // EEPROM_MANAGER_H