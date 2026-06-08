#include <Wire.h>
#include <RTClib.h>
#include <DS1307RTC.h>
RTC_DS1307 rtc;

// Define the interrupt pin for the rain gauge
const int rainGaugePin = 2;

// Constants for conversion
const float pulseToRainFactor = 0.28;
const unsigned long minuteInMillis = 60000; // 60,000 milliseconds = 1 minute

// Variables
volatile unsigned long totalRainPulses = 0; // Total pulse count
unsigned long lastMinuteMillis = 0;          // Timestamp for the last minute reset

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("welcome rain gauge");
   pinMode(rainGaugePin, INPUT_PULLUP);
  // Initialize the RTC
  rtc.begin();

  // Check if the RTC is running, and set the time if not
  if (!rtc.isrunning()) {
    // Set the RTC with the current date and time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Attach an interrupt to the rain gauge pin
  attachInterrupt(digitalPinToInterrupt(rainGaugePin), countRainPulse, RISING);
}

void loop() {
  
  // Get the current time from the RTC
  DateTime now = rtc.now();

  // Calculate elapsed time since the last minute reset
  unsigned long currentMillis = millis();
  unsigned long elapsedMillis = currentMillis - lastMinuteMillis;

  // Check if 1 minute has passed
  if (elapsedMillis >= minuteInMillis) {
    // Calculate and print the total rain for the past minute
    float totalRain = totalRainPulses * pulseToRainFactor;
    Serial.print("Total Rain in the last minute (");
    Serial.print(now.hour(), DEC);
    Serial.print(":");
    Serial.print(now.minute(), DEC);
    Serial.print("): ");
    Serial.print(totalRain);
    Serial.println(" mm");

    // Reset the total pulse count and update the last minute timestamp
    totalRainPulses = 0;
    lastMinuteMillis = currentMillis;
  }

  // Your other loop code can go here
}

// Interrupt service routine to count rain pulses
void countRainPulse() {
  totalRainPulses++;
  Serial.println(totalRainPulses);
}
