
#include <HardwareSerial.h>

// Define the UART port to use
#define RX_PIN 16  // Pin for receiving data
#define TX_PIN 17  // Pin for transmitting data
HardwareSerial sensorSerial(1); // Use UART1

void setup() {
  Serial.begin(115200);  // Debug serial monitor
  sensorSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Configure UART1
  
  // Optional: Confirm setup
  Serial.println("UART initialized for sensor communication");
}

void loop() 
{
  uint8_t writeData[5] = {0x3A, 0x00, 0x00, 0x00, 0x4B}; // Data to send
  uint8_t readData[5]; // Array to store received data

  // Write the 5-byte data to the sensor
  sensorSerial.write(writeData, 5);
  String response =sensorSerial.readStringUntil('80');
 //String response = unoSerial.readStringUntil('\n');
  Serial.println(response);
  delay(50);
}
