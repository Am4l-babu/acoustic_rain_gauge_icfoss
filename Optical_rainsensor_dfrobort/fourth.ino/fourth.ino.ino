#include <Arduino.h>
#include <HardwareSerial.h>

class OpticalRainSensor {
private:
    // ESP32/ESP8266 UART Configuration
    HardwareSerial& sensorSerial;
    
    // Calibration Constants (PROGMEM for memory efficiency)
    static const PROGMEM byte CALIBRATION_CMD[5];
    static const PROGMEM byte READ_CALIBRATION_CMD[5];

    // Configuration Parameters
    volatile uint16_t calibrationValue = 0;
    volatile bool calibrationStatus = false;

    // Error Tracking
    uint8_t errorCount = 0;
    const uint8_t MAX_ERROR_THRESHOLD = 3;

public:
    // Constructor with Serial Reference
    OpticalRainSensor(HardwareSerial& serial) 
        : sensorSerial(serial) {
        // Configure UART
        sensorSerial.begin(9600, SERIAL_8N1);
    }

    // Optimized Command Transmission
    bool sendCommand(const byte* command, size_t length) {
        // Clear input buffer before sending
        while(sensorSerial.available()) sensorSerial.read();

        // Send command with atomic write
        sensorSerial.write(command, length);
        sensorSerial.flush();  // Ensure complete transmission

        return waitForResponse();
    }

    // Non-Blocking Response Wait
    bool waitForResponse(uint16_t timeout = 500) {
        unsigned long startTime = millis();
        
        while (millis() - startTime < timeout) {
            if (sensorSerial.available()) return true;
            yield();  // ESP non-blocking delay
        }
        
        return false;
    }

    // Advanced Calibration with Error Handling
    bool performOpticalCalibration() {
        if (errorCount >= MAX_ERROR_THRESHOLD) {
            // Soft reset mechanism
            resetCalibration();
            return false;
        }

        bool result = sendCommand(CALIBRATION_CMD, sizeof(CALIBRATION_CMD));
        
        if (!result) {
            errorCount++;
            calibrationStatus = false;
            return false;
        }

        // Read response with timeout
        byte response[10];
        size_t bytesRead = sensorSerial.readBytes(response, sizeof(response));

        if (bytesRead > 0 && response[0] == 0x3A) {
            calibrationStatus = true;
            errorCount = 0;  // Reset error tracking
            return true;
        }

        return false;
    }

    // Enhanced Calibration Value Reading
    uint16_t readCalibrationValue() {
        if (!sendCommand(READ_CALIBRATION_CMD, sizeof(READ_CALIBRATION_CMD))) {
            return 0;
        }

        byte response[6];
        size_t bytesRead = sensorSerial.readBytes(response, sizeof(response));

        if (bytesRead == 6 && response[0] == 0x3A) {
            // Bitwise value extraction
            calibrationValue = (response[2] << 8) | response[3];
            return calibrationValue;
        }

        return 0;
    }

    // Soft Reset Mechanism
    void resetCalibration() {
        calibrationStatus = false;
        calibrationValue = 0;
        errorCount = 0;
    }

    // Diagnostic Reporting with WiFi/MQTT Potential
    void diagnosticReport() {
        String reportData = String("Calibration Status: ") + 
                            (calibrationStatus ? "Success" : "Failed") +
                            ", Value: " + calibrationValue;
        
        // Optional: Send via MQTT/WebSocket
        Serial.println(reportData);
    }

    // Getter Methods
    bool isCalibrated() const { return calibrationStatus; }
    uint16_t getCalibrationValue() const { return calibrationValue; }
};

// Static Constant Definitions
const PROGMEM byte OpticalRainSensor::CALIBRATION_CMD[5] = {0x3A, 0x83, 0x00, 0x00, 0x54};
const PROGMEM byte OpticalRainSensor::READ_CALIBRATION_CMD[5] = {0x3A, 0x03, 0x00, 0x00, 0x81};

// Global Sensor Instance (Using Serial2 for ESP32)
OpticalRainSensor rainSensor(Serial2);

void setup() {
    // High-speed debug serial
    Serial.begin(115200);
    
    // Configure Serial2 for sensor communication
    Serial2.begin(9600, SERIAL_8N1, 16, 17);  // RX, TX pins for ESP32
    
    // Calibration with Retry Logic
    for (int attempt = 0; attempt < 3; attempt++) {
        if (rainSensor.performOpticalCalibration()) {
            rainSensor.readCalibrationValue();
            rainSensor.diagnosticReport();
            break;
        }
        delay(500);
    }
}

void loop() {
    static unsigned long lastCalibrationTime = 0;
    
    // Non-blocking periodic calibration
    if (millis() - lastCalibrationTime > 60000) {
        rainSensor.performOpticalCalibration();
        lastCalibrationTime = millis();
    }

    // Optional: Add other sensor monitoring tasks
    yield();  // Prevent watchdog timeout
}
