#include <HardwareSerial.h>

#define RXD2 16
#define TXD2 17
#define BAUD_RATE 115200

HardwareSerial RainSensor(1); // Use UART2

// CRC-8 Checksum Calculation Function
uint8_t calculateCRC8(uint8_t *data, uint32_t length) {
    uint8_t crc = 0xFF;
    while(length--) {
        crc ^= *data++;
        for(int i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}

// Query Rainfall Status Command
void queryRainfallStatus() {
    uint8_t command[] = {0x3A, 0x00, 0x00, 0x00, 0x4B};
    RainSensor.write(command, 5);
}

void setup() {
    Serial.begin(115200);
    RainSensor.begin(BAUD_RATE, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
    queryRainfallStatus();
    
    if (RainSensor.available()) {
        uint8_t response[5];
        RainSensor.readBytes(response, 5);
        
        // Verify CRC
        uint8_t calculatedCRC = calculateCRC8(response, 5);
        
        if (calculatedCRC == response[5]) {
            switch(response[2]) {
                case 0x00: Serial.println("No Rain"); break;
                case 0x01: Serial.println("Light Rain"); break;
                case 0x02: Serial.println("Moderate Rain"); break;
                case 0x03: Serial.println("Heavy Rain"); break;
            }
        }
    }
    
    delay(100); // Check every second
}
