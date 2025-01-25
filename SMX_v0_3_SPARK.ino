// SMX_v0.1.ino
#include "main.h"
#include "config.h"
#include "impedance_meter.h"
#include "temperature.h"
#include "lora_handler.h"
#include "eeprom_manager.h"
#include "power_manager.h"

uint8_t retryCount = 0;

// Global instances
ImpedanceMeter* impedanceMeter = nullptr;
PowerManager* powerManager = nullptr;
LoRaWANHandler* loraHandler = nullptr;
//EEPROMManager* eepromManager = nullptr;
TemperatureSensor* tempSensor = nullptr;
ExternalEEPROM eeprom;
EEPROMManager eepromManager(eeprom);

// Hardware instances
PCA9536 io;
TMP102 STemp;
//Adafruit_EEPROM_I2C LoraMem;

// Global state variables
SystemState currentState = SystemState::INIT;
SensorConfig config;
bool measurementRequested = false;

uint32_t startupTime = 0;
uint32_t cycleCount = 0;
const uint32_t TEST_DURATION_MS = 10 * 60 * 1000; // 10 minutes test duration


bool initializeSensors();

// Measurement data
int8_t HL = 0;
int8_t HH = 0;
int Temp = 0;
int8_t Batt = 0;
uint16_t SNr = 0;
uint32_t Time = 0;
uint32_t lastWakeupTime = 0;

// Task management
SemaphoreHandle_t taskEvent = nullptr;
SoftwareTimer taskWakeupTimer;
int8_t eventType = -1;

void setup() {
  startupTime = millis();

  // Disable BLE first thing
    Serial.println("Disabling BLE...");
    Bluefruit.begin(0, 0);  // Initialize with 0 peripherals and 0 centrals
    Bluefruit.Advertising.stop();  // Stop advertising
    Bluefruit.Scanner.stop();      // Stop scanning



    // Add power monitoring point
    uint32_t initialCurrent = analogRead(WB_A0);  // Battery monitoring pin
    Serial.print("Initial current draw: ");
    Serial.println(initialCurrent);
    Serial.begin(115200);
    time_t timeout = millis();
    while (!Serial && (millis() - timeout) < 5000);

    Serial.println("\n===================================");
    Serial.printf("SMX Soil Moisture Sensor v%s\n", VERSION_STRING);
    Serial.printf("Build: %s %s\n", BUILD_DATE, BUILD_TIME);
    Serial.println("===================================\n");

    initializeSystem();
    // Create EEPROM manager instance
    //eepromManager = new EEPROMManager(LoraMem);
     // Initialize EEPROM
    //if (!eepromManager->initialize()) {
      //  Serial.println("Failed to initialize EEPROM");
    //} else (Serial.println("xxxxxxxxxxxxxxxxxxxxxxxEEPROM"));
    
    
}

bool initializeSensors() {
    bool success = true;
    
    if (!io.begin()) {
        Serial.println("PCA9536 not detected");
        success = false;
    } else {
        io.pinMode(Pins::EN_SEL, OUTPUT);  // Use Pins namespace
        io.pinMode(Pins::C_SEL, OUTPUT);   // Use Pins namespace
        io.write(Pins::EN_SEL, LOW);
        io.write(Pins::C_SEL, HIGH);
    }
    
    // Initialize temperature sensor
    if (!STemp.begin()) {
        Serial.println("TMP102 not detected");
        success = false;
    }
    
    if (!impedanceMeter->initialize()) {
        Serial.println("AD5933 not detected");
        success = false;
    }
    
    return success;
}

// Callback implementations
void handleMeasurementRequest() {
    measurementRequested = true;
    if (taskEvent) {
        xSemaphoreGive(taskEvent);
    }
}

/*
void handleIntervalUpdate(uint8_t newInterval) {
    
    if (config.DS_min != newInterval) {
        config.DS_min = newInterval;
        if (eepromManager) {
            eepromManager->writeConfig(config);
            Serial.print("Am scris in eeprom: ");
        }
        Time = SystemConstants::MIN_TO_MS(config.DS_min);
        taskWakeupTimer.stop();
        taskWakeupTimer.begin(Time, periodicWakeup);
        taskWakeupTimer.start();
    }
    
}*/

void handleIntervalUpdate(uint8_t newInterval) {
    if (config.DS_min != newInterval) {
        uint8_t oldInterval = config.DS_min;  // Save old value
        config.DS_min = newInterval;
        
        if (eepromManager.writeConfig(config)) {
            Serial.print("Am scris in eeprom: ");
            Serial.print(oldInterval);
            Serial.print(" -> ");
            Serial.println(newInterval);
            
            // Only update timer if EEPROM write was successful
            Time = SystemConstants::MIN_TO_MS(config.DS_min);
            taskWakeupTimer.stop();
            taskWakeupTimer.begin(Time, periodicWakeup);
            taskWakeupTimer.start();
        } else {
            config.DS_min = oldInterval;  // Revert to old value if write failed
            Serial.println("Eroare la scriere in EEPROM!");
        }
    }
}

/*
void handleIntervalUpdate(uint8_t newInterval) {
    if (config.DS_min != newInterval) {
        Serial.printf("Updating interval from %d to %d minutes\n", config.DS_min, newInterval);
        
        // Update config
        config.DS_min = newInterval;
        
        // Write to EEPROM first
        if (eepromManager) {
            // Write just the interval to EEPROM
            eepromManager->eeprom.write(EEPROMConfig::SLEEP_TIME_ADDR, newInterval);
            delay(10); // Give EEPROM time to write
            
            // Verify the write
            uint8_t readBack = eepromManager->eeprom.read(EEPROMConfig::SLEEP_TIME_ADDR);
            if (readBack == newInterval) {
                Serial.println("New interval saved to EEPROM");
            } else {
                Serial.println("ERROR: Failed to save interval to EEPROM");
            }
        }

        // Update timer after successful EEPROM write
        Time = SystemConstants::MIN_TO_MS(config.DS_min);
        taskWakeupTimer.stop();
        taskWakeupTimer.begin(Time, periodicWakeup);
        taskWakeupTimer.start();
        
        Serial.println("Timer updated with new interval");
    }
}*/

void initializeSystem() {
    Serial.println("Starting initialization...");
    PowerMonitor::init();
    PowerMonitor::printPowerStatus("System start");

    // Create instances
    Serial.println("Creating component instances...");
    impedanceMeter = new ImpedanceMeter();
    powerManager = new PowerManager();
    loraHandler = new LoRaWANHandler();
    //eepromManager = new EEPROMManager(LoraMem);
    if (!eepromManager.initialize()) {
        Serial.println("Failed to initialize EEPROM");
    } else (Serial.println("xxxxxxxxxxxxxxxxxxxxxxxEEPROM"));
    Serial.println("Component instances created");

    // Set up LoRaWAN callbacks
    loraHandler->setCallbacks(handleMeasurementRequest, handleIntervalUpdate);
    Serial.println("LoRaWAN callbacks configured");

    // Initialize hardware
    Serial.println("Initializing hardware...");
    pinMode(Pins::EN, OUTPUT);
    pinMode(Pins::LOW_DIV, OUTPUT);
    //pinMode(WB_IO2, OUTPUT);
    //digitalWrite(WB_IO2, HIGH);   // power on for AT24C02 device
    Wire.begin();
    Serial.println("I2C initialized");
    
    // Initialize task management
    Serial.println("Initializing task management...");
    taskEvent = xSemaphoreCreateBinary();
    if (taskEvent == NULL) {
        Serial.println("ERROR: Failed to create semaphore!");
    } else {
        Serial.println("Task semaphore created");
    }
    
    // In initializeSystem()
    Serial.println("Reading EEPROM configuration...");
    if (!eepromManager.initialize()) {
        Serial.println("ERROR: EEPROM initialization failed!");
        while(1) {
            delay(1000);
            Serial.println("System halted due to EEPROM error");
        }
    }
    /*
    if (!eepromManager->readConfig(config)) {
        Serial.println("ERROR: Failed to read configuration!");
    } else {
        Serial.println("Configuration loaded successfully");
        Serial.printf("- Sleep time: %d minutes\n", config.DS_min);
        Serial.printf("- Serial number: %d\n", config.SNr);
    }
    */
    // Read configuration
    Serial.println("Reading EEPROM configuration...");
    if (!eepromManager.readConfig(config)) {
        Serial.println("ERROR: Failed to read configuration!");
    } else {
        Serial.println("Configuration loaded:");
        Serial.printf("- Sleep time: %d minutes\n", config.DS_min);
        Serial.printf("- Serial number: %d\n", config.SNr);
    }
    
    // Initialize time
    Time = 15000; // Initial 20s interval
    Serial.printf("Initial interval set to: %d ms\n", Time);
    
    // Initialize components
    Serial.println("Initializing power management...");
    powerManager->wakeUp();
    
    Serial.println("Initializing impedance meter...");
    if (!impedanceMeter->initialize()) {
        Serial.println("ERROR: Failed to initialize impedance meter!");
    } else {
        Serial.println("Impedance meter initialized");
    }
    
    Serial.println("Initializing LoRaWAN...");
    if (!loraHandler->initialize()) {
        Serial.println("ERROR: Failed to initialize LoRaWAN!");
    } else {
        Serial.println("LoRaWAN initialized");
    }
    
    Serial.println("Starting wake timer...");
    taskWakeupTimer.begin(Time, periodicWakeup);
    taskWakeupTimer.start();

    Serial.println("Initialization complete!");
    Serial.println("===========================\n");
    PowerMonitor::printPowerStatus("After initialization");
}


void loop() {
    handleState();
}

void handleState() {
    switch(currentState) {
        case SystemState::INIT:
            handleInitState();
            break;
        case SystemState::MEASUREMENT:
            handleMeasurementState();
            break;
        case SystemState::TRANSMIT:
            handleTransmitState();
            break;
        case SystemState::SLEEP:
            handleSleepState();
            break;
    }
}

void handleInitState() {
    if (initializeSensors()) {
        currentState = SystemState::MEASUREMENT;
        Serial.println("Moving to measurement state");
    }
}

void handleMeasurementState() {
    if (xSemaphoreTake(taskEvent, portMAX_DELAY) == pdTRUE && measurementRequested) {
        Serial.println("\nStarting measurement cycle...");
        lastWakeupTime = millis();
        
        powerManager->wakeUp();
        delay(100);
        
        // Read battery level
        Batt = powerManager->getBatteryLevel();
        Serial.printf("Battery level: %d%%\n", Batt);
        
        // Read temperature
        Temp = tempSensor->getTemperature();
        Serial.printf("Temperature: %d?C\n", Temp);
        
        // Get moisture readings
        io.write(Pins::C_SEL, HIGH);
        delay(10);
        HL = impedanceMeter->getMoisture(config.gainL, config.CminL, config.CmaxL, Temp);
        Serial.printf("Low-gain moisture: %d%%\n", HL);
        
        io.write(Pins::C_SEL, LOW);
        delay(10);
        
        HH = impedanceMeter->getMoisture(config.gainH, config.CminH, config.CmaxH, Temp);
        Serial.printf("High-gain moisture: %d%%\n", HH);
        
        if (HL >= 0 && HH >= 0) {
            Serial.println("Measurements successful, moving to transmission");
            currentState = SystemState::TRANSMIT;
        } else {
            Serial.println("ERROR: Invalid measurements!");
        }
        
        measurementRequested = false;
    }
}

void handleTransmitState() {
    Serial.println("Preparing LoRaWAN transmission...");
    
    uint8_t payload[9];
    payload[0] = HL & 0xFF;
    payload[1] = (HL >> 8) & 0xFF;
    payload[2] = HH & 0xFF;
    payload[3] = (HH >> 8) & 0xFF;
    payload[4] = Temp;
    payload[5] = Batt;
    payload[6] = config.SNr & 0xFF;
    //Serial.println(Batt);
    payload[7] = (config.SNr >> 8) & 0xFF;
    //Serial.println(Batt);
    payload[8] = config.DS_min;
    //Serial.println(Batt);

    // Print interpreted data
    Serial.println("Payload contents:");
    Serial.printf("Low-gain moisture: %d%%\n", HL);
    Serial.printf("High-gain moisture: %d%%\n", HH);
    Serial.printf("Temperature: %d°C\n", Temp);
    Serial.printf("Battery: %d%%\n", Batt);
    Serial.printf("Serial Number: %d\n", config.SNr);
    Serial.printf("Sleep interval: %d minutes\n", config.DS_min);

    if (loraHandler->sendData(payload, sizeof(payload))) {
        Serial.println("LoRa transmission successful");
        currentState = SystemState::SLEEP;
    } else {
        Serial.println("LoRa transmission failed");
        if (++retryCount >= SystemConstants::RETRY_COUNT_MAX) {
            Serial.println("Max retries reached, going to sleep");
            retryCount = 0;
            currentState = SystemState::SLEEP;
        } else {
            Serial.printf("Retry %d of %d\n", retryCount, SystemConstants::RETRY_COUNT_MAX);
        }
    }
}


void handleSleepState() {
    PowerMonitor::printPowerStatus("Before sleep");
    cycleCount++;
    uint32_t runTime = (millis() - startupTime) / 1000; // seconds
    Serial.printf("\nCycle #%lu, Runtime: %lu seconds\n", cycleCount, runTime);
    
    // Check if we're close to the 55-hour mark where it failed before
    if (runTime >= 86400) {
        Serial.println("WARNING: Approaching previous failure time!  Reset!");
        NVIC_SystemReset();
    }
    
    
    Serial.printf("\nEntering sleep mode for %d minutes\n", config.DS_min);
    Serial.println("==================================");
    
    powerManager->enterLowPowerMode();
    
    taskWakeupTimer.stop();
    Time = SystemConstants::MIN_TO_MS(config.DS_min);
    taskWakeupTimer.begin(Time, periodicWakeup);
    taskWakeupTimer.start();
    
    PowerMonitor::printPowerStatus("After sleep setup");
    
    currentState = SystemState::MEASUREMENT;
}



/*void handleSleepState() {
    cycleCount++;
    uint32_t runTime = (millis() - startupTime) / 1000; // seconds
    Serial.printf("\nCycle #%lu, Runtime: %lu seconds\n", cycleCount, runTime);
    
    // Check if we're close to the 55-hour mark where it failed before
    if (runTime >= TEST_DURATION_MS/1000) {
        Serial.println("WARNING: Approaching previous failure time!");
    }
    
    Serial.printf("Entering sleep mode for %d minutes\n", config.DS_min);
    Serial.println("==================================");
    
    Serial.println("Step 1: Before powerManager");
    powerManager->enterLowPowerMode();
    Serial.println("Step 2: After powerManager");
    
    // Temporarily reduce sleep time for testing
    uint32_t debugTime = 30000; // 30 seconds instead of normal minutes
    
    Serial.println("Step 3: Before timer stop");
    taskWakeupTimer.stop();
    Serial.println("Step 4: After timer stop");
    
    Time = debugTime; // Use shorter time for testing
    Serial.println("Step 5: Time calculated");
    
    Serial.println("Step 6: Before timer begin");
    taskWakeupTimer.begin(Time, periodicWakeup);
    Serial.println("Step 7: After timer begin");
    
    Serial.println("Step 8: Before timer start");
    taskWakeupTimer.start();
    Serial.println("Step 9: After timer start");
    
    Serial.println("Step 10: Before state change");
    currentState = SystemState::MEASUREMENT;
    Serial.println("Step 11: State changed to MEASUREMENT");
    
    Serial.flush();
}*/



void periodicWakeup(TimerHandle_t unused) {
    eventType = 1;
    measurementRequested = true;
    xSemaphoreGiveFromISR(taskEvent, pdFALSE);
}