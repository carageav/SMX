# SMX - Soil Moisture Sensor v0.1
## Description
SMX is a sophisticated soil moisture sensor system that combines impedance measurement with LoRaWAN communication capabilities. The system uses capacitive sensing technology with temperature compensation for accurate soil moisture measurements.

## Hardware Components
- RAK4631 (nRF52840-based) - Main processor and LoRaWAN communication
- AD5933 - Impedance measurement IC
- TMP102 - Temperature sensor
- PCA9536D - I/O expander for sensor selection
- EEPROM - Configuration storage
- Dual-range moisture sensing circuit

## Features
- Dual-range moisture measurement (High/Low gain)
- Temperature compensation
- LoRaWAN communication
- Configurable sleep intervals
- Low power operation
- Battery monitoring
- Remote configuration capability
- EEPROM-based calibration storage

## Project Structure
```
SMX_v0.1/
├── src/
│   ├── config/
│   │   └── config.h           // System configuration and constants
│   ├── sensors/
│   │   ├── impedance_meter.h  // Soil moisture measurement
│   │   ├── impedance_meter.cpp
│   │   ├── temperature.h      // Temperature sensing
│   │   └── temperature.cpp
│   ├── communication/
│   │   ├── lora_handler.h     // LoRaWAN communication
│   │   ├── lora_handler.cpp
│   │   └── lora_config.h      // LoRaWAN settings
│   ├── power/
│   │   ├── power_manager.h    // Power management
│   │   └── power_manager.cpp
│   └── storage/
│       ├── eeprom_manager.h   // Configuration storage
│       └── eeprom_manager.cpp
├── include/
│   └── main.h                 // Main header file
├── SMX_v0.1.ino              // Main Arduino file
└── README.md
```

## Dependencies
### Libraries
- LoRaWan-RAK4630
- Wire (I2C communication)
- SPI
- AD5933
- SparkFunTMP102
- PCA9536
- Adafruit_EEPROM_I2C

### Hardware Requirements
- RAK4631 WisDuo module
- Custom sensor board with:
  - AD5933 impedance analyzer
  - TMP102 temperature sensor
  - PCA9536 I/O expander
  - EEPROM

## Installation
1. Install Arduino IDE
2. Add RAK BSP to Arduino IDE
3. Install required libraries
4. Clone this repository
5. Open SMX_v0.1.ino in Arduino IDE

## Configuration
### LoRaWAN Settings
Update the following in `lora_config.h`:
```cpp
const uint8_t DEVICE_EUI[8] = {...};
const uint8_t APP_EUI[8] = {...};
const uint8_t APP_KEY[16] = {...};
```

### Hardware Pins
Configured in `config.h`:
```cpp
namespace Pins {
    constexpr uint8_t EN = WB_SW1;
    constexpr uint8_t LOW_DIV = WB_IO2;
    constexpr uint8_t BATT = WB_A0;
    constexpr uint8_t C_SEL = WB_IO2;
    constexpr uint8_t EN_SEL = 1;
}
```

## Usage
### Initial Setup
1. Configure LoRaWAN credentials
2. Perform sensor calibration
3. Upload code to device
4. Verify LoRaWAN connection

### Operation Modes
The device operates in four states:
1. INIT - System initialization
2. MEASUREMENT - Sensor reading
3. TRANSMIT - LoRaWAN communication
4. SLEEP - Power saving mode

### LoRaWAN Commands
- 0x01: Update measurement interval
- 0x02: Request immediate measurement
- 0x03: System reset

### Data Format
```
Payload (9 bytes):
[0-1]: Low gain moisture (%)
[2-3]: High gain moisture (%)
[4]: Temperature (°C)
[5]: Battery level (%)
[6-7]: Serial Number
[8]: Sleep interval (minutes)
```

## Power Management
- Sleep current: <50µA
- Active measurement: ~15mA
- Transmission: ~40mA
- Configurable sleep intervals

## Calibration
The system supports two-point calibration for each gain range:
1. Dry soil reference (0%)
2. Saturated soil reference (100%)
Values are stored in EEPROM for persistence.

## Error Handling
- Communication errors
- Sensor failures
- Battery monitoring
- LoRaWAN connection issues

## Contributing
1. Fork the repository
2. Create a feature branch
3. Commit changes
4. Push to the branch
5. Create Pull Request

## Version History
- v0.1 (Current)
  - Initial release
  - Basic functionality
  - Dual-range measurements
  - LoRaWAN communication

## Future Improvements
- [ ] Enhanced power optimization
- [ ] Advanced calibration routines
- [ ] Over-the-air firmware updates
- [ ] Enhanced error logging
- [ ] Web interface for configuration
- [ ] Data logging capabilities

## License
[Your License Here]

## Contact
[Your Contact Information]

## Acknowledgments
- RAKwireless for WisDuo module
- Analog Devices for AD5933
- SparkFun for TMP102
- Community contributors
