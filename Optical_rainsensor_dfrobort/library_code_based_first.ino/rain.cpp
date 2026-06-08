#include "MySensorLibrary.h"

// Constructor
MySensorLibrary::MySensorLibrary(HardwareSerial &serial, uint32_t baudRate, int intPin)
    : serial(serial), baudRate(baudRate), interruptPin(intPin), isDataAvailable(false) {}

// Initialize UART and optional interrupt
void MySensorLibrary::begin() {
    serial.begin(baudRate);

    // Setup interrupt if a valid pin is provided
    if (interruptPin >= 0) {
        pinMode(interruptPin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(interruptPin), [this]() { this->handleInterrupt(); }, FALLING);
    }
}

// Send the status command to the sensor
void MySensorLibrary::sendCommand() {
    serial.write(command, sizeof(command));
}

// Handle hardware interrupt
void MySensorLibrary::handleInterrupt() {
    isDataAvailable = true;
}

// Read the sensor's status
bool MySensorLibrary::readStatus(uint8_t *buffer, size_t length, uint16_t timeout) {
    // Send the command to the sensor
    sendCommand();

    // Wait for the response or interrupt
    size_t index = 0;
    unsigned long startTime = millis();

    while (millis() - startTime < timeout) {
        // Check for data availability via hardware interrupt or polling
        if (isDataAvailable || serial.available()) {
            isDataAvailable = false; // Reset flag
            buffer[index++] = serial.read();
            if (index == length) {
                return true; // Successfully read all bytes
            }
        }
    }

    return false; // Timeout occurred
}