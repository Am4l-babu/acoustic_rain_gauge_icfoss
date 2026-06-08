#include <HardwareSerial.h>

// Define the UART port to use
#define RX_PIN 16  // Pin for receiving data
#define TX_PIN 17  // Pin for transmitting data
//HardwareSerial sensorSerial(1); // Use UART1
 //uint8_t writeData[5]; 
void setup() {
  Serial.begin(115200);  // Debug serial monitor
  sensorSerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); // Configure UART1

  // Optional: Confirm setup
  Serial.println("UART initialized for sensor communication");
}

void loop() {
  //writeData[0] = {0x3A};
  //writeData[1] = {0x00};
  //writeData[2] = {0x00};
 // writeData[3] = {0x00};
//  writeData[4] = {0x00};
 // writeData[5] = {0x4B}; 
//uint8_t writeData[6]={0x3A,0x00,0x00,0x00,0x4B};// Data to send
//uint8_t writeData[6]={0x3A,0x91,0x00,0x00,0x7C};
//uint8_t writeData[6]={0x3A,0x91,0x01,0x00,0x88};

uint8_t writeData[6]={0x3A,0x01,0x00,0x00,0x0D};// Data to send
//uint8_t writeData[6]={0x3A,0x02,0x00,0x00,0xC7};// Data to send
//uint8_t writeData[6]={0x3A,0x02,0x00,0x00,0xC7};// Data to send
  uint8_t readData[5]; // Array to store received data

  // Write the 5-byte data to the sensor
  sensorSerial.write(writeData, 5);
  Serial.println("Data sent to sensor");

  // Wait for response
  delay(10); // Wait as per sensor's response time

  // Check if data is available
  if (sensorSerial.available() >= 0) {
    sensorSerial.read(readData, 5); // Read 5 bytes
    Serial.println("Data received from sensor:");

    // Print the received data
    for (int i = 0; i < 5; i++) {
      Serial.printf("Byte %d: 0x%02X\n", i, readData[i]);
    }
  } else {
    Serial.println("No response received or incomplete data");
  }

  delay(1000); // Loop delay
}
