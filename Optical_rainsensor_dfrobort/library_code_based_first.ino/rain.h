#ifndef RAIN-SENSORLIBRARY_H
#define RAIN-SENSORLIBRARY_H

#include <Arduino.h>

class Smart_inteligent_sensor {
private:
   public:
    // Constructor
    MySensorLibrary(HardwareSerial &serial, uint32_t baudRate, int intPin = -1);

    // Public Methods
    void begin();                             // Initialize UART and interrupt setup
    bool readStatus(uint8_t *buffer, size_t length, uint16_t timeout = 1000); // Read sensor status

    // Interrupt Service Routine (ISR) callback for hardware interrupt
    void handleInterrupt();

public:
    // UART settings
    HardwareSerial &serial;
    uint32_t baudRate;

    // Command to request status
    const uint8_t command[5] = {0x3A, 0x01, 0x00, 0x00, 0x0D};

    // Interrupt pin
    int interruptPin;

    // Private Methods
    void sendCommand();  // Send the status command
    bool isDataAvailable; // Flag for data availability
   
};

#endif 

