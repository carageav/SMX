# SMX - Soil Moisture Sensor v0.1
[Previous general description remains the same...]

## Software Architecture

### Core Components

#### 1. Impedance Measurement System
```cpp
class ImpedanceMeter {
    static constexpr uint32_t START_FREQ = 99930;  // ~100kHz operating frequency
    static constexpr uint16_t FREQ_INCR = 10;      // 10Hz increments
    static constexpr uint8_t NUM_INCR = 12;        // 12 measurement points
    static constexpr uint8_t NUM_SAMPLES = 5;      // Samples for averaging
```
- Utilizes AD5933 for impedance spectroscopy
- Multiple frequency sweep for noise reduction
- Temperature compensation using empirical coefficient
- Averaging algorithm for measurement stability
- Capacitance calculation formula:
  ```
  Cin = 1E+12 / (2 * π * f * |Z|)
  where:
  - f = START_FREQ + (FREQ_INCR * NUM_INCR / 2)
  - |Z| = measured impedance magnitude
  ```

#### 2. Power Management System
```cpp
class PowerManager {
    static constexpr uint32_t STARTUP_DELAY_MS = 100;
    static constexpr float LOW_BATTERY_THRESHOLD = 20.0;
    static constexpr int BATTERY_SAMPLES = 5;
```
- State-based power control
- Configurable sleep modes
- Battery monitoring with voltage divider
- ADC averaging for stable readings
- Power consumption profiles:
  - Sleep: <50µA
  - Measurement: ~15mA
  - Transmission: ~40mA
  - Wake-up time: 100ms

#### 3. LoRaWAN Communication
```cpp
namespace LoRaConfig {
    constexpr uint8_t JOIN_TRIALS = 8;
    constexpr uint8_t APP_PORT = LORAWAN_APP_PORT;
    constexpr uint8_t DATA_BUFF_SIZE = 64;
```
- OTAA authentication
- EU868 frequency band
- ADR disabled for stability
- Data Rate: DR_3
- Retry mechanism with backoff
- Payload structure:
  ```
  struct PayloadFormat {
      int16_t moistureL;    // 2 bytes
      int16_t moistureH;    // 2 bytes
      int8_t temperature;   // 1 byte
      uint8_t battery;      // 1 byte
      uint16_t serialNum;   // 2 bytes
      uint8_t interval;     // 1 byte
  } __attribute__((packed)); // Total: 9 bytes
  ```

#### 4. Storage Management
```cpp
class EEPROMManager {
    static constexpr uint32_t WRITE_DELAY_MS = 5;
    static constexpr uint32_t INIT_DELAY_MS = 300;
```
- EEPROM layout:
  ```
  Address | Size | Description
  0x00    | 4    | Low Gain Calibration
  0x0A    | 4    | High Gain Calibration
  0x14    | 2    | Low Range Max Cap
  0x1E    | 2    | High Range Max Cap
  0x28    | 2    | Low Range Min Cap
  0x32    | 2    | High Range Min Cap
  0x3C    | 2    | Serial Number
  0x46    | 1    | Sleep Time
  ```
- Write verification
- Wear leveling considerations
- Data integrity checks

### State Machine Implementation

#### 1. System States
```cpp
enum class SystemState {
    INIT,
    MEASUREMENT,
    TRANSMIT,
    SLEEP
};
```

State transitions and conditions:
```
INIT → MEASUREMENT:
- Condition: Successful sensor initialization
- Actions: Configure peripherals, load calibration

MEASUREMENT → TRANSMIT:
- Condition: Valid measurements obtained
- Actions: Process readings, prepare payload

TRANSMIT → SLEEP:
- Condition: Successful transmission or max retries
- Actions: Update timing, enter low power

SLEEP → MEASUREMENT:
- Condition: Timer expiry or external trigger
- Actions: Wake-up sequence, sensor power-up
```

#### 2. Task Management
```cpp
SemaphoreHandle_t taskEvent;
SoftwareTimer taskWakeupTimer;
```
- FreeRTOS task synchronization
- Timer-based wake-up mechanism
- Event-driven state transitions
- Critical section handling

### Software Optimization

#### 1. Memory Management
- Static allocation for critical components
- Stack optimization in interrupt handlers
- Heap fragmentation prevention
- Buffer size optimization

#### 2. Processing Efficiency
- Interrupt-based timing
- Efficient numeric calculations
- Optimized I2C communications
- Power-aware state transitions

### Error Handling

#### 1. Error Categories
```cpp
enum class ErrorCode {
    NONE = 0,
    SENSOR_INIT_FAILED = 1,
    CALIBRATION_INVALID = 2,
    MEASUREMENT_ERROR = 3,
    LORA_JOIN_FAILED = 4,
    TRANSMISSION_FAILED = 5,
    LOW_BATTERY = 6
};
```

#### 2. Recovery Mechanisms
- Automatic retry logic
- Graceful degradation
- Watchdog implementation
- Error logging in EEPROM

### Calibration System

#### 1. Two-Point Calibration
```cpp
struct CalibrationPoint {
    float capacitance;
    float moisture;
    float temperature;
};
```

#### 2. Temperature Compensation
```cpp
float tempCompensation(float capacitance, float temp) {
    return capacitance * (1 + TEMP_COEFF * (temp - REF_TEMP));
}
```

### Remote Configuration

#### 1. LoRaWAN Downlink Commands
```cpp
enum class LoRaCommand : uint8_t {
    UPDATE_INTERVAL = 0x01,
    REQUEST_MEASUREMENT = 0x02,
    SYSTEM_RESET = 0x03
};
```

#### 2. Configuration Parameters
- Measurement interval
- Transmission power
- Gain settings
- Calibration values

### Debugging Features

#### 1. Serial Output Format
```cpp
Debug output format:
[timestamp][state][function] message
Example:
[1234567][MEAS][ImpedanceMeter] Reading: 234.5 pF
```

#### 2. Debug Levels
```cpp
enum class DebugLevel {
    NONE,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    VERBOSE
};
```

### Performance Metrics

#### 1. Timing
- Measurement cycle: ~500ms
- LoRaWAN join: <10s
- Wake-up time: <100ms
- Sleep transition: <10ms

#### 2. Accuracy
- Moisture: ±2% absolute
- Temperature: ±0.5°C
- Battery: ±2%

[Previous sections about Contributing, Version History, etc. remain the same...]

Would you like me to:
1. Add more implementation details for specific components?
2. Include code examples for common operations?
3. Add debugging procedures?
4. Expand on any specific section?