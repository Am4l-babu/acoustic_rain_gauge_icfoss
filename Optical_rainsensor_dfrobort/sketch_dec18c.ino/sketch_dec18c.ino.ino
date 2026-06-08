#include <stdint.h>

// Function: CRC-8 Calculation
// Polynomial: 0x31; Data Reverse: MSB First; Initial Value: 0xFF; XOR Value: 0x00
uint8_t xCal_crc(uint8_t *ptr, uint32_t len) {
  uint8_t crc = 0xFF;  // Initial CRC value
  uint8_t i;

  while (len--) {           // Process each byte of data
    crc ^= *ptr++;          // XOR byte with current CRC
    for (i = 0; i < 8; i++) {  // Process each bit
      if (crc & 0x80) {      // If MSB is 1, XOR with polynomial 0x31
        crc = (crc << 1) ^ 0x31;
      } else {               // If MSB is 0, just shift left
        crc = (crc << 1);
      }
    }
  }
  return crc;  // Return final CRC value
}

void setup() {
  Serial.begin(115200);  // Start Serial Communication
  Serial.println("CRC-8 Calculation Test");

  // Test data for CRC calculation
  uint8_t data[] = {0x3A, 0x01, 0x00};  // Example data
  uint32_t length = sizeof(data);

  // Calculate CRC-8 value
  uint8_t crc_result = xCal_crc(data, length);

  // Print CRC-8 result to the Serial Monitor
  Serial.print("CRC-8 Checksum: 0x");
  Serial.println(crc_result, HEX);
}

void loop() {
  // Do nothing in the loop
}
