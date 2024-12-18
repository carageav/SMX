// src/communication/lora_handler.h
#ifndef LORA_HANDLER_H
#define LORA_HANDLER_H

#include "main.h"
#include "eeprom_manager.h"  // Include the full definition


// LoRaWAN constants
#define JOINREQ_NBTRIALS 8
#define LORAWAN_APP_DATA_BUFF_SIZE 64

class LoRaWANHandler {

public:
    static bool doOTAA;  // Add this static member
    // Define callback types for downlink handling
    typedef void (*MeasurementRequestCallback)();
    typedef void (*IntervalUpdateCallback)(uint8_t newInterval);        

    LoRaWANHandler();
    bool initialize();
    bool sendData(const uint8_t* data, uint8_t length);
    void handleDownlink(const uint8_t* data, uint8_t size);

    // Set callbacks
    void setCallbacks(MeasurementRequestCallback measurementCb,
                     IntervalUpdateCallback intervalCb) {
        measurementCallback = measurementCb;
        intervalCallback = intervalCb;
    }

private:
    // Declare these first
    uint8_t m_lora_app_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE];
    lmh_app_data_t m_lora_app_data;

    // Callback pointers
    MeasurementRequestCallback measurementCallback;
    IntervalUpdateCallback intervalCallback;

    
    // Static members for LoRaWAN configuration
    static uint8_t deviceEUI[8];
    static uint8_t appEUI[8];
    static uint8_t appKey[16];

    void setupCallbacks();
    
    // Static callback methods
    static uint8_t getBatteryLevel();
    static void getUniqueId(uint8_t* id);
    static uint32_t getRandomSeed();
    static void handleRxData(lmh_app_data_t* app_data);
    static void handleJoinSuccess();
    static void handleClassConfirmation(DeviceClass_t Class);
    static void handleJoinFailure();



};

#endif // LORA_HANDLER_H