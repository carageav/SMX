// communication/lora_config.h
#ifndef LORA_CONFIG_H
#define LORA_CONFIG_H

#include "main.h"

namespace LoRaConfig {
    // Remove const from the arrays since lmh functions require non-const pointers
    extern uint8_t DEVICE_EUI[8];
    extern uint8_t APP_EUI[8];
    extern uint8_t APP_KEY[16];
        
        
    // LoRaWAN Parameters
    constexpr uint8_t JOIN_TRIALS = 8;
    constexpr uint8_t APP_PORT = LORAWAN_APP_PORT;
    constexpr uint8_t DATA_BUFF_SIZE = 64;
    
    // Default LoRaWAN settings
    const bool USE_OTAA = true;
    const DeviceClass_t DEFAULT_CLASS = CLASS_A;
    const LoRaMacRegion_t DEFAULT_REGION = LORAMAC_REGION_EU868;
}
#endif // LORA_CONFIG_H
    
/*
    // Device credentials (replace with your values)
    const uint8_t DEVICE_EUI[8] = {0xAC, 0x1F, 0x09, 0xFF, 0xFE, 0x18, 0x44, 0xE5};
    const uint8_t APP_EUI[8] = {0xB8, 0x27, 0xEB, 0xFF, 0xFE, 0x39, 0x00, 0x00};
    const uint8_t APP_KEY[16] = {0x29, 0xE5, 0xF2, 0xB1, 0x9E, 0x41, 0x87, 0x60, 
                                0xFC, 0x1F, 0x4A, 0xC7, 0x30, 0x07, 0xD9, 0x61};
}

// LoRaWAN command definitions
enum class LoRaCommand : uint8_t {
    UPDATE_INTERVAL = 0x01,
    REQUEST_MEASUREMENT = 0x02,
    SYSTEM_RESET = 0x03
};
*/
