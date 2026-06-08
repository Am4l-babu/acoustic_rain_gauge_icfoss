#include <Arduino.h>

// Define hardware serial pins for ESP32
#define RXD2 16   // RX pin (connect to sensor TX)
#define TXD2 17   // TX pin (connect to sensor RX)

HardwareSerial mySerial(2); // Use UART2 (Serial2)

uint8_t writeData[6]; // Data to send
uint8_t readData[6];  // Data to read

void setup() {
  Serial.begin(115200);     
  mySerial.begin(115200, SERIAL_8N1, RXD2, TXD2);  // Init UART2
  Serial.println("HardwareSerial2 initialized for communication");
}

void loop() {
  // Prepare the data to send
  writeData[0] = 0x3A;
  writeData[1] = 0x01;
  writeData[2] = 0x00;
  writeData[3] = 0x00;
  writeData[4] = 0x0D;

  // Send data using HardwareSerial
  mySerial.write(writeData, 6);
  Serial.print("Data sent: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("0x%02X ", writeData[i]);
  }
  Serial.println();

  delay(50); // Allow sensor time to respond

  // Read data if available
  if (mySerial.available() >= 5) { // Assuming 5-byte response
    for (int i = 0; i < 5; i++) {
      if (mySerial.available()) {
        readData[i] = mySerial.read();
      }
    }

    Serial.print("Data received: ");
    for (int i = 0; i < 5; i++) {
      Serial.printf("0x%02X ", readData[i]);
    }
    Serial.println();
  } else {
    Serial.println("No response or incomplete data received");
  }

  delay(1000);
}
