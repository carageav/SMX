// main.h
#ifndef MAIN_H
#define MAIN_H

#include "VERSION.h"
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <LoRaWan-RAK4630.h>
#include <PCA9536D.h>
#include <SparkFunTMP102.h>
//#include "Adafruit_EEPROM_I2C.h"
#include <SparkFun_External_EEPROM.h>
#include "config.h"

// Forward declarations
class ImpedanceMeter;
class PowerManager;
class LoRaWANHandler;
class EEPROMManager;

// Global variables declarations
extern PCA9536 io;
extern TMP102 STemp;
//extern ExternalEEPROM LoraMem;
extern uint8_t retryCount;


extern ImpedanceMeter* impedanceMeter;
extern PowerManager* powerManager;
extern LoRaWANHandler* loraHandler;
//extern EEPROMManager* eepromManager;

extern SensorConfig config;
extern SystemState currentState;
extern int8_t HL;
extern int8_t HH;
extern int Temp;
extern int8_t Batt;
extern uint16_t SNr;
extern uint32_t Time;
extern SemaphoreHandle_t taskEvent;
extern SoftwareTimer taskWakeupTimer;

// Function declarations
void initializeSystem();
bool initializeSensors();
void periodicWakeup(TimerHandle_t unused);

#endif // MAIN_H