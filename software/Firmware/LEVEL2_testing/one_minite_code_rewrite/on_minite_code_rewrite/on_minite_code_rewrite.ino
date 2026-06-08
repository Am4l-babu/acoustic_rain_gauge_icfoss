#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

const int soundSensorPin = A0;     // Analog pin for sound sensor
const int loudnessSensorPin = A1;  // Analog pin for loudness sensor
const int chipSelect = 4;          // Pin for SD card module's chip select

RTC_DS3231 rtc;
File dataFile;

unsigned long startTime = 0;
int soundSum = 0;
int loudnessSum = 0;
int readingsCount = 0;

void setup() {
  Serial.begin(9600);

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  if (!rtc.begin()) {
    Serial.println("RTC initialization failed!");
    while (1);
  }

  dataFile = SD.open("hello.txt", FILE_WRITE);

  if (dataFile) {
    Serial.println("welcome to mazhamavini...");
  } else {
    Serial.println("Error opening hello.txt!");
  }
}

void loop() {
  DateTime now = rtc.now();

  int soundValue = analogRead(soundSensorPin);
  
  int loudnessValue = analogRead(loudnessSensorPin);
    
  if (now.second() == 0) {
    if (readingsCount > 0) {
      int avgSound = soundSum / readingsCount;
      int avgLoudness = loudnessSum / readingsCount;

      // Print to Serial Monitor
      Serial.print(now.day());
      Serial.print("/");
      Serial.print(now.month());
      Serial.print("/");
      Serial.print(now.year());
      Serial.print(" ");
      if (now.hour() < 10) Serial.print("0");
      Serial.print(now.hour());
      Serial.print(":");
      if (now.minute() < 10) Serial.print("0");
      Serial.print(now.minute());
      Serial.print(":");
      if (now.second() < 10) Serial.print("0");
      Serial.print(now.second());
      Serial.print(",");
      Serial.print(avgSound);
      Serial.print(",");
      Serial.println(avgLoudness);

      // Write data to the SD card with the same format
      dataFile.print(now.day());
      dataFile.print("/");
      dataFile.print(now.month());
      dataFile.print("/");
      dataFile.print(now.year());
      dataFile.print(" ");
      if (now.hour() < 10) dataFile.print("0");
      dataFile.print(now.hour());
      dataFile.print(":");
      if (now.minute() < 10) dataFile.print("0");
      dataFile.print(now.minute());
      dataFile.print(":");
      if (now.second() < 10) dataFile.print("0");
      dataFile.print(now.second());
      dataFile.print(",");
      dataFile.print(avgSound);
      dataFile.print(",");
      dataFile.println(avgLoudness);
      
     
      soundSum = 0;
      loudnessSum = 0;
      readingsCount = 0;
    }

    startTime = millis();
  }

  soundSum += soundValue;
  loudnessSum += loudnessValue;
  readingsCount++;

  if (now.second() == 0 && now.minute() % 1 == 0) {
    dataFile.close();
    dataFile = SD.open("hello.txt", FILE_WRITE);
    
  }

  delay(1000);

  
}



 
