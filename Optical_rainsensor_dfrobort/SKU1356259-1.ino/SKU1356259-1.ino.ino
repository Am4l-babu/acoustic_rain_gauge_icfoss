// UART Communication for Rain Detection Module
#define FRAME_HEADER 0x3A  // Fixed frame header byte
#define CRC_POLY 0x31      // Polynomial for CRC-8
#define CRC_INIT 0xFF      // Initial CRC value

// Function to calculate CRC-8 checksum
uint8_t calculateCRC8(uint8_t *data, uint8_t length) {
    uint8_t crc = CRC_INIT;
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];  // XOR each byte
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ CRC_POLY;
            else
                crc <<= 1;
        }
    }
    return crc;
}

// Function to send a UART command
void sendCommand(uint8_t frameFlag, uint16_t frameData) {
    uint8_t frame[5];  // Frame: Header, Flag, Data (2 bytes), CRC
    frame[0] = FRAME_HEADER;  // Fixed header
    frame[1] = frameFlag;     // Frame flag
    frame[2] = (frameData >> 8) & 0xFF;  // Frame data MSB
    frame[3] = frameData & 0xFF;         // Frame data LSB
    frame[4] = calculateCRC8(&frame[1], 3);  // CRC-8 for Flag & Data

    // Send frame via UART
    for (uint8_t i = 0; i < 5; i++) {
        Serial.write(frame[i]);
    }
}

// Function to receive and verify data
bool receiveData(uint8_t *buffer) {
    if (Serial.available() >= 5) {
        for (int i = 0; i < 5; i++) {
            buffer[i] = Serial.read();
        }
        // Verify header
        if (buffer[0] != FRAME_HEADER) return false;

        // Verify CRC-8 checksum
        uint8_t crc = calculateCRC8(&buffer[1], 3);
        if (crc == buffer[4]) {
            return true;  // Data is correct
        }
    }
    return false;  // Data is incorrect
}

void setup() {
    Serial.begin(115200);  // UART configuration
    Serial.println("Rain Detection Module UART Test");
}

void loop() {
    uint8_t receivedData[5];

    // Example: Send a command to read rainfall status
    sendCommand(0x01, 0x0000);  // FrameFlag = 0x01 (Read Rainfall Status), Data = 0x0000
    delay(100);  // Wait for response

    // Receive and validate response
    if (receiveData(receivedData)) {
        Serial.print("Received Valid Data: ");
        for (int i = 0; i < 5; i++) {
            Serial.print(receivedData[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    } else {
        Serial.println("Invalid Data or No Response");
    }

    delay(1000);  // Wait before next command
}
