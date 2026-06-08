#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

RTC_DS1307 rtc;
File dataFile;
// Define the interrupt pin for the rain gauge
const int rainGaugePin = 2;
const int ledPin=13;
const float pulseToRainFactor = 0.28;
const unsigned long minuteInMillis = 60000;

void countRainPulse();
volatile unsigned long totalRainPulses = 0; // Total pulse count
unsigned long lastMinuteMillis = 0;          // Timestamp for the last minute reset
float totalRain ;

void countRainPulse() {
  totalRainPulses++;
  Serial.println(totalRainPulses);
}

void setup()
{
  Serial.begin(9600);  //start Serial in case we need to print debugging info
  Serial.println("welcome rain gauge");
  pinMode(ledPin, OUTPUT);
  
  pinMode(rainGaugePin, INPUT_PULLUP);
  SD.begin();

  while(!Serial);
    if(! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      while (1);
    }
    else {
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
   if(! rtc.isrunning()) {
      Serial.println("RTC is NOT running!");
    }
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
 Serial.println("initialization failed!");
 while (1);
}
   Serial.println("Date,Time,totalRainPulses,totalRain");
   dataFile.print("Date,Time,totalRainPulses,totalRain");
   attachInterrupt(digitalPinToInterrupt(rainGaugePin), countRainPulse, RISING);
}

void loop()
{
  
  //digitalWrite(ledPin, state);
  
  unsigned long currentMillis = millis();
  unsigned long elapsedMillis = currentMillis - lastMinuteMillis;
 
  
  if (elapsedMillis >= minuteInMillis) {
    totalRain = totalRainPulses * pulseToRainFactor;
    logData(totalRain, totalRainPulses);

  
    totalRainPulses = 0;// Reset the total pulse count and update the last minute timestamp
    lastMinuteMillis = currentMillis;
  }


}

void logData(float rain, unsigned long pulseCount)
{
dataFile = SD.open("rama.txt", FILE_WRITE);
  if (dataFile) {  
  tmElements_t tm;

 if (RTC.read(tm)) {
    
    Serial.print(tm.Day);
    Serial.print('/');
    Serial.print(tm.Month);
    Serial.print('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.print(",");
    Serial.print(tm.Hour);
    Serial.print(":");
    Serial.print(tm.Minute);
    Serial.print(":");
    Serial.print(tm.Second);
    Serial.print(',');
    Serial.print(pulseCount);
    Serial.print(',');
    Serial.print(rain);
    Serial.print(" mm");
    Serial.println();
    dataFile.print(tm.Day);
    dataFile.print('/');
    dataFile.print(tm.Month);
    dataFile.print('/');
    dataFile.print(tmYearToCalendar(tm.Year));
    dataFile.print(",");
    dataFile.print(tm.Hour);
    dataFile.print(":");
    dataFile.print(tm.Minute);
    dataFile.print(":");
    dataFile.print(tm.Second);
    dataFile.print(',');
    dataFile.print(pulseCount);
    dataFile.print(',');
    dataFile.print(rain);
    dataFile.print(" ");
    dataFile.println(" mm");
    Serial.println();
    dataFile.close();
    
  } 
    else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    } 
}
dataFile = SD.open("rama.txt");
if (dataFile) {
  
Serial.println("rama.txt:");
// read from the file until there's nothing else in it:
while (dataFile.available()) {
Serial.write(dataFile.read());
}
dataFile.close();
}
}
