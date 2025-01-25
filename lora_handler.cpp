// src/communication/lora_handler.cpp
#include "lora_handler.h"

// Initialize static members
/*//60002
uint8_t LoRaWANHandler::deviceEUI[8] = {0xAC, 0x1F, 0x09, 0xFF, 0xFE, 0x18, 0x44, 0xE5};
uint8_t LoRaWANHandler::appEUI[8] = {0xB8, 0x27, 0xEB, 0xFF, 0xFE, 0x39, 0x00, 0x00};
uint8_t LoRaWANHandler::appKey[16] = {0x29, 0xE5, 0xF2, 0xB1, 0x9E, 0x41, 0x87, 0x60, 0xFC, 0x1F, 0x4A, 0xC7, 0x30, 0x07, 0xD9, 0x61};
*/

//60003
uint8_t LoRaWANHandler::deviceEUI[8] = {0xAC, 0x1F, 0x09, 0xFF, 0xFE, 0x18, 0x22, 0x57};
uint8_t LoRaWANHandler::appEUI[8] = {0xB8, 0x27, 0xEB, 0xFF, 0xFE, 0x39, 0x00, 0x03};
uint8_t LoRaWANHandler::appKey[16] = {0x7D, 0x5D, 0xC9, 0x6A, 0xCB, 0x38, 0xB3, 0xBE, 0x89, 0xC8, 0x3D, 0x9B, 0x88, 0x24, 0x0B, 0x7A};




bool LoRaWANHandler::doOTAA = true;  // Add this definition

LoRaWANHandler::LoRaWANHandler() : 
    measurementCallback(nullptr),
    intervalCallback(nullptr) {
    m_lora_app_data.buffer = m_lora_app_data_buffer;
    m_lora_app_data.buffsize = 0;
    m_lora_app_data.port = 0;
    m_lora_app_data.rssi = 0;
    m_lora_app_data.snr = 0;
}

bool LoRaWANHandler::initialize() {
    if (lora_rak4630_init() != 0) {
        Serial.println("SX126x init failed");
        return false;
    }

    // Set the keys
    lmh_setDevEui(deviceEUI);
    lmh_setAppEui(appEUI);
    lmh_setAppKey(appKey);

    // Setup callbacks
    setupCallbacks();

    // Set sub band
    if (!lmh_setSubBandChannels(1)) {
        Serial.println("Subband init error");
        return false;
    }

    // Start Join procedure
    Serial.println("Starting join procedure");
    lmh_join();

    return true;
}

uint8_t LoRaWANHandler::getBatteryLevel() {
    return static_cast<uint8_t>(Batt);
}

void LoRaWANHandler::getUniqueId(uint8_t* id) {
    memcpy(id, deviceEUI, 8);  // Now using static deviceEUI
}

uint32_t LoRaWANHandler::getRandomSeed() {
    return millis();
}

void LoRaWANHandler::handleRxData(lmh_app_data_t* app_data) {
    if (app_data && app_data->buffsize > 0 && loraHandler) {
        loraHandler->handleDownlink(app_data->buffer, app_data->buffsize);
    }
}

void LoRaWANHandler::handleJoinSuccess() {
    Serial.println("OTAA join successful");
    digitalWrite(LED_CONN, LOW);
}

void LoRaWANHandler::handleClassConfirmation(DeviceClass_t Class) {
    Serial.printf("Switch to class %c done\n", "ABC"[Class]);
}

void LoRaWANHandler::handleJoinFailure() {
    Serial.println("OTAA join failed!");
}

void LoRaWANHandler::setupCallbacks() {
    static lmh_callback_t callbacks = {
        getBatteryLevel,
        getUniqueId,
        getRandomSeed,
        handleRxData,
        handleJoinSuccess,
        handleClassConfirmation,
        handleJoinFailure
    };
    
    // Initialize LoRaWAN with callbacks
    lmh_param_t lora_param_init = {
        LORAWAN_ADR_OFF,
        DR_3,
        LORAWAN_PUBLIC_NETWORK,
        JOINREQ_NBTRIALS,
        LORAWAN_DEFAULT_TX_POWER,
        LORAWAN_DUTYCYCLE_OFF
    };

    lmh_init(&callbacks, lora_param_init, true, CLASS_A, LORAMAC_REGION_EU868);
}

/*
bool LoRaWANHandler::sendData(const uint8_t* data, uint8_t length) {
    ///Serial.println("send data"); 
    if (!lmh_join_status_get()) {
        Serial.println("Not joined to network");
        return false;
    }

    if (length > LORAWAN_APP_DATA_BUFF_SIZE) {
        Serial.println("Data too long");
        return false;
    }

    m_lora_app_data.port = LORAWAN_APP_PORT;
    ///Serial.println("data.port"); 
    memcpy(m_lora_app_data_buffer, data, length);
    ///Serial.println("buffer"); 
    m_lora_app_data.buffsize = length;
    ///Serial.println("data.buffsize"); 

    return (lmh_send(&m_lora_app_data, LMH_UNCONFIRMED_MSG) == 0);
}*/

bool LoRaWANHandler::sendData(const uint8_t* data, uint8_t length) {
    Serial.println("\nLoRaWAN Send Data:");
    
    if (!lmh_join_status_get()) {
        Serial.println("ERROR: Not joined to network!");
        return false;
    }

    Serial.println("Network Status: Connected");
    Serial.printf("Sending payload: [");
    for(uint8_t i = 0; i < length; i++) {
        Serial.printf("%02X ", data[i]);
        if (i < length - 1) Serial.print(" ");
    }
    Serial.println("]");

    m_lora_app_data.port = LORAWAN_APP_PORT;
    memcpy(m_lora_app_data_buffer, data, length);
    m_lora_app_data.buffsize = length;

    lmh_error_status error = lmh_send(&m_lora_app_data, LMH_UNCONFIRMED_MSG);
    
    if (error == 0) {
        Serial.println("LoRa send request successful");
        return true;
    } else {
        Serial.printf("LoRa send failed with error: %d\n", error);
        return false;
    }
}

// Also add debug messages to join callback
static void lorawan_has_joined_handler(void) {
    Serial.println("\n*** LoRaWAN Network Joined! ***");
    if (LoRaWANHandler::doOTAA) {  // Use the class scope
        uint32_t otaaDevAddr = lmh_getDevAddr();
        Serial.printf("OTAA joined with device address: %08X\n", otaaDevAddr);
    } else {
        Serial.println("ABP connection established");
    }
    digitalWrite(LED_CONN, LOW);
}


static void lorawan_join_failed_handler(void) {
    Serial.println("\n*** LoRaWAN Join Failed! ***");
    Serial.println("Please check:");
    Serial.println("1. LoRaWAN keys configuration");
    Serial.println("2. Gateway availability");
    Serial.println("3. Antenna connection");
    Serial.println("4. Radio frequency settings");
}


// Add handleDownlink implementation
void LoRaWANHandler::handleDownlink(const uint8_t* data, uint8_t size) {
    if (!data || size == 0) return;

    switch(data[0]) {
        case 0x01: // Update interval
	    //Serial.println("case 01");
            if (size >= 2 && intervalCallback != nullptr) {
                intervalCallback(data[1]);
            }
            break;

        case 0x02: // Request immediate measurement
	    Serial.println("case 02");
            if (measurementCallback) {
                measurementCallback();
            }
            break;

        case 0x03: // System reset
	    Serial.println("case 03");
            NVIC_SystemReset();
            break;

        default:
            Serial.printf("Unknown command: 0x%02X\n", data[0]);
            break;
    }
}