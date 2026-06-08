//#include <SoftwareSerial.h>

// Define SoftwareSerial pins
#define RX_PIN 10
#define TX_PIN 11

// Create a SoftwareSerial object
SoftwareSerial mySerial(RX_PIN, TX_PIN);

uint8_t writeData[6]; // Data to send
uint8_t readData[6];  // Data to read

void setup() {
  Serial.begin(115200);     // For debugging with the Serial Monitor
  mySerial.begin(115200);   // Initialize SoftwareSerial communication
  Serial.println("SoftwareSerial initialized for communication");
}

void loop() {
  // Prepare the data to send
  writeData[0] = 0x3A;
  writeData[1] = 0x00;
  writeData[2] = 0x00;
  writeData[3] = 0x00;
  writeData[4] = 0x00;
  writeData[5] = 0x4B;

  // Send data using SoftwareSerial
  mySerial.write(writeData, 6);
  Serial.print("Data sent: ");
  for (int i = 0; i < 6; i++) {
    Serial.print("0x");
    if (writeData[i] < 0x10) Serial.print("0"); // Ensure two-digit format
    Serial.print(writeData[i], HEX);
    if (i < 5) Serial.print(" ");              // Add space between bytes
  }
  Serial.println();

  delay(50); // Increase delay for sensor response

  // Read data if available
  if (mySerial.available() >= 5) { // Assuming response size is 5 bytes
    for (int i = 0; i < 5; i++) {
      if (mySerial.available()) {
        readData[i] = mySerial.read(); // Read each byte
      }
    }

    Serial.print("Data received: ");
    for (int i = 0; i < 5; i++) {
      Serial.print("0x");
      if (readData[i] < 0x10) Serial.print("0"); // Ensure two-digit format
      Serial.print(readData[i], HEX);
      if (i < 4) Serial.print(" ");             // Add space between bytes
    }
    Serial.println();
  } else {
    Serial.println("No response or incomplete data received");
  }

  delay(1000); // Loop delay
}
