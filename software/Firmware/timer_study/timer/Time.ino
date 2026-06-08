#include <Wire.h>
#include <RTClib.h>
#include <DS1307RTC.h>
#include <SD.h>
RTC_DS1307 rtc;
File dataFile;
// Define the interrupt pin for the rain gauge
const int rainGaugePin = 2;

// Constants for conversion
const float pulseToRainFactor = 0.28;
const unsigned long minuteInMillis = 60000; // 60,000 milliseconds = 1 minute
void logData(DateTime timestamp, float rain, unsigned long pulseCount);
void countRainPulse();
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
  SD.begin();
  
   Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
  Serial.println("initialization failed!");
  while (1);
  }
   Serial.println("initialization done.");
   Serial.println("Date,Time,totalRainPulses,totalRain");
   dataFile.print("Date,Time,totalRainPulses,totalRain");
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
    DateTime timestamp;// Calculate and print the total rain for the past minute
    float totalRain = totalRainPulses * pulseToRainFactor;
    Serial.print("Total Rain in the last minute (");
    Serial.print(now.hour(), DEC);
    Serial.print(":");
    Serial.print(now.minute(), DEC);
    Serial.print("): ");
    Serial.print(timestamp.year(), DEC);
    Serial.print("-");
    Serial.print(timestamp.month(), DEC);
    Serial.print("-");
    Serial.print(timestamp.day(), DEC);
    Serial.print(", ");
  

    Serial.print(totalRain);
    Serial.println(" mm");
    logData(now, totalRain, totalRainPulses);
    totalRainPulses = 0;//// Reset the total pulse count and update the last minute timestamp
    lastMinuteMillis = currentMillis;
  }

}
 


// Interrupt service routine to count rain pulses
void countRainPulse() {
  totalRainPulses++;
  Serial.println(totalRainPulses);
}

void logData(DateTime timestamp, float rain, unsigned long pulseCount) {
  
   dataFile = SD.open("ram.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(timestamp.year(), DEC);
    dataFile.print("-");
    dataFile.print(timestamp.month(), DEC);
    dataFile.print("-");
    dataFile.print(timestamp.day(), DEC);
    dataFile.print(", ");
    dataFile.print(timestamp.hour(), DEC);
    dataFile.print(":");
    dataFile.print(timestamp.minute(), DEC);
    dataFile.print(", ");
    dataFile.print(pulseCount);
    dataFile.print(", ");
    dataFile.print(rain);
    dataFile.println(" mm");
    dataFile.close();
  } else {
    Serial.println("Error opening data file.");
  }
 dataFile = SD.open("ram.txt");
  if (dataFile) {

    Serial.println("ram.txt:");
    // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
}
}
