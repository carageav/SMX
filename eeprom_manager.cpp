// src/storage/eeprom_manager.cpp
#include "eeprom_manager.h"

EEPROMManager::EEPROMManager(ExternalEEPROM& eeprom) : eeprom(eeprom) {}

// src/storage/eeprom_manager.cpp
bool EEPROMManager::readConfig(SensorConfig& config) {
    Serial.println("Starting EEPROM read process...");
    
    Serial.println("Reading gainL...");
    eeprom.get(EEPROMConfig::GAIN_L_ADDR, config.gainL);
    Serial.print("gainL: ");
    Serial.println(config.gainL, 15);
    
    Serial.println("Reading gainH...");
    eeprom.get(EEPROMConfig::GAIN_H_ADDR, config.gainH);
    Serial.print("gainH: ");
    Serial.println(config.gainH, 15);
    
    Serial.println("Reading CmaxL...");
    eeprom.get(EEPROMConfig::CMAX_L_ADDR, config.CmaxL);
    Serial.printf("CmaxL: %d\n", config.CmaxL);
    
    Serial.println("Reading CmaxH...");
    eeprom.get(EEPROMConfig::CMAX_H_ADDR, config.CmaxH);
    Serial.printf("CmaxH: %d\n", config.CmaxH);
    
    Serial.println("Reading CminL...");
    eeprom.get(EEPROMConfig::CMIN_L_ADDR, config.CminL);
    Serial.printf("CminL: %d\n", config.CminL);
    
    Serial.println("Reading CminH...");
    eeprom.get(EEPROMConfig::CMIN_H_ADDR, config.CminH);
    Serial.printf("CminH: %d\n", config.CminH);
    
    Serial.println("Reading SNr...");
    eeprom.get(EEPROMConfig::SNR_ADDR, config.SNr);
    Serial.printf("SNr: %d\n", config.SNr);
    
    Serial.println("Reading DS_min...");
    config.DS_min = eeprom.read(EEPROMConfig::SLEEP_TIME_ADDR);
    Serial.printf("DS_min: %d\n", config.DS_min);
    
    Serial.println("EEPROM read complete");
    return true;
}


  //Adafruit EEPROM
bool EEPROMManager::initialize() {
    Wire.begin();
    Serial.println("Initializing EEPROM...");
    
    eeprom.setMemoryType(2);  // As in your calibration code

    if (!eeprom.begin()) {
        Serial.println("Failed to initialize EEPROM");
        return false;
    }
    
    Serial.print("Memory size: ");
    Serial.println(eeprom.length());
    delay(300);  // Same delay as your calibration
    
    return true;
}


/*
bool EEPROMManager::initialize() {
    // Enable EEPROM
    //digitalWrite(Pins::MEM_EN, HIGH);
    //delay(10);
    eeprom.setMemoryType(2);  // 
    Serial.print("Set memory type for your specific EEPROM");
    if (!eeprom.begin()) Serial.println("failed to initialise EEPROM");
    Serial.println("nnnnnnnnnnnnn");
    //if (success) {
      //  delay(INIT_DELAY_MS);
        //Serial.print("Memory size: ");
        //Serial.println(eeprom.length());
    //}   
    
    
    // Disable EEPROM if initialization failed
    //if (!success) {
      //  digitalWrite(Pins::MEM_EN, LOW);
    //}
    
    return true;
}
*/


/*
bool EEPROMManager::writeConfig(const SensorConfig& config) {
    uint16_t newDS = config.DS_min;Serial.print("new DStime: ");Serial.println(newDS);
    eeprom.put(EEPROMConfig::SLEEP_TIME_ADDR, config.DS_min);
    return true;
}
*/


/* MERGEEEE PARTIAL
bool EEPROMManager::writeConfig(const SensorConfig& config) {
    uint16_t newDS = config.DS_min;
    Serial.print("new DStime: ");
    Serial.println(newDS);
    
    Wire.begin();
    Serial.println("I2C initialized");
    
    // Use the correct namespace
    eeprom.setMemoryType(EEPROMConfig::EEPROM_SIZE);
    bool beginResult = eeprom.begin();
    Serial.print("EEPROM begin result: ");
    Serial.println(beginResult ? "OK" : "Failed");
    
    if (!beginResult) {
        return false;
    }
    
    Serial.print("Memory size: ");
    Serial.println(eeprom.length());
    delay(300);  // Same delay as calibration
    
    Serial.println("Attempting write...");
    bool writeResult = eeprom.write(EEPROMConfig::SLEEP_TIME_ADDR, (uint8_t)newDS);
    Serial.print("Write result: ");
    Serial.println(writeResult ? "Success" : "Failed");
    
    if (writeResult) {
        delay(10);  // Wait for write to complete
        uint8_t readBack = eeprom.read(EEPROMConfig::SLEEP_TIME_ADDR);
        Serial.print("Verification - Read back: ");
        Serial.println(readBack);
    }
    
    return writeResult;
}
*/

bool EEPROMManager::writeConfig(const SensorConfig& config) {
    uint16_t newDS = config.DS_min;
    Serial.print("new DStime: ");
    Serial.println(newDS);
    
    Wire.begin();
    Serial.println("I2C initialized");
    
    eeprom.setMemoryType(EEPROMConfig::EEPROM_SIZE);
    bool beginResult = eeprom.begin();
    Serial.print("EEPROM begin result: ");
    Serial.println(beginResult ? "OK" : "Failed");
    
    Serial.print("Memory size: ");
    Serial.println(eeprom.length());
    delay(300);
    
    Serial.println("Writing to EEPROM...");
    eeprom.write(EEPROMConfig::SLEEP_TIME_ADDR, (uint8_t)newDS);
    delay(10);  // Wait for write to complete
    
    // Read back verification
    uint8_t readBack = eeprom.read(EEPROMConfig::SLEEP_TIME_ADDR);
    Serial.print("Verification - Read back value: ");
    Serial.println(readBack);
    
    // Consider write successful if readback matches
    bool success = (readBack == (uint8_t)newDS);
    Serial.print("Write ");
    Serial.println(success ? "successful!" : "failed!");
    
    return success;
}

/*
bool EEPROMManager::writeConfig(const SensorConfig& config) {
   Serial.println("\nWriting configuration to EEPROM:");

   // Enable EEPROM by setting MEM_EN HIGH
   digitalWrite(WB_IO2, HIGH);
   delay(10);  // Give EEPROM time to wake up
   
   Serial.print("Writing gainL ("); 
   Serial.print(config.gainL);
   Serial.print(") to address 0x");
   Serial.println(EEPROMConfig::GAIN_L_ADDR, HEX);
   LoraMem.put(EEPROMConfig::GAIN_L_ADDR, config.gainL);

   Serial.print("Writing gainH (");
   Serial.print(config.gainH);
   Serial.print(") to address 0x");
   Serial.println(EEPROMConfig::GAIN_H_ADDR, HEX);
   LoraMem.put(EEPROMConfig::GAIN_H_ADDR, config.gainH);

   Serial.print("Writing CmaxL (");
   Serial.print(config.CmaxL);
   Serial.print(") to address 0x");
   Serial.println(EEPROMConfig::CMAX_L_ADDR, HEX);
   LoraMem.put(EEPROMConfig::CMAX_L_ADDR, config.CmaxL);

   Serial.print("Writing CmaxH (");
   Serial.print(config.CmaxH);
   Serial.print(") to address 0x");
   Serial.println(EEPROMConfig::CMAX_H_ADDR, HEX);
   LoraMem.put(EEPROMConfig::CMAX_H_ADDR, config.CmaxH);

   Serial.print("Writing CminL (");
   Serial.print(config.CminL);
   Serial.print(") to address 0x");
   Serial.println(EEPROMConfig::CMIN_L_ADDR, HEX);
   LoraMem.put(EEPROMConfig::CMIN_L_ADDR, config.CminL);

   Serial.print("Writing CminH (");
   Serial.print(config.CminH);
   Serial.print(") to address 0x");
   Serial.println(EEPROMConfig::CMIN_H_ADDR, HEX);
   LoraMem.put(EEPROMConfig::CMIN_H_ADDR, config.CminH);

   Serial.print("Writing SNr (");
   Serial.print(config.SNr);
   Serial.print(") to address 0x");
   Serial.println(EEPROMConfig::SNR_ADDR, HEX);
   LoraMem.put(EEPROMConfig::SNR_ADDR, config.SNr);

   Serial.print("Writing Sleep Time (");
   Serial.print(config.DS_min);
   Serial.print(") to address 0x");
   Serial.println(EEPROMConfig::SLEEP_TIME_ADDR, HEX);
   eeprom.write(EEPROMConfig::SLEEP_TIME_ADDR, config.DS_min);

   Serial.println("EEPROM write complete");

   // Disable EEPROM by setting MEM_EN LOW
   digitalWrite(WB_IO2, LOW);

   return true;
}
*/

