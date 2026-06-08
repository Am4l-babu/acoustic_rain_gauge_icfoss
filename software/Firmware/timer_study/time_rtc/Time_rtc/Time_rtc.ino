#include <Wire.h>
#include <RTClib.h>
#include <DS1307RTC.h>
#include <SD.h>
File dataFile;
RTC_DS1307 rtc;
unsigned long startMillis;
unsigned long currentMillis;
unsigned long previousMillis = 0;

const unsigned long period = 60000;
const byte ledPin = 13;
const byte interruptPin = 2;
volatile byte state = LOW;
int count = 0;
void counting();
int totalcount;
int flag;
  int total_rain;
void blink() {
  state = !state;
  int rain_count = +flag;
  Serial.println(rain_count);
  flag++;

}
void setup()
{

  Serial.begin(9600);  //start Serial in case we need to print debugging info
  pinMode(ledPin, OUTPUT);
  startMillis = millis();
  pinMode(interruptPin, INPUT_PULLUP);
  rtc.begin();

  // Check if the RTC is running, and set the time if not
  if (!rtc.isrunning()) {
    // Set the RTC with the current date and time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
//  SD.begin();
//
//  Serial.print("Initializing SD card...");
//  if (!SD.begin(4)) {
//    Serial.println("initialization failed!");
//    while (1);
//  }
  Serial.println("initialization done.");
  Serial.println("Date,Time,totalRainPulses,totalRain");
  dataFile.print("Date,Time,totalRainPulses,totalRain");
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, RISING);

}
void loop()
{
  DateTime now = rtc.now();
  DateTime timestamp;
  currentMillis = millis();

  if (currentMillis - startMillis >= period)
  {
    total_rain = flag;
    float rain = total_rain * 0.28;
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

    Serial.print("Rain count");
    Serial.println(total_rain);
    Serial.print("Rain");
    Serial.print(rain);
    Serial.println("mm");

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
    dataFile.print(total_rain);
    dataFile.print(", ");
    dataFile.print(rain);
    dataFile.println(" mm");
    dataFile.close();

    flag = 0;
    startMillis = currentMillis;
  }
  else {
    Serial.println(total_rain);
    delay(10000);
  }
// dataFile = SD.open("ram.txt");
//  if (dataFile) {
//
//    Serial.println("ram.txt:");
//    // read from the file until there's nothing else in it:
//    while (dataFile.available()) {
//      Serial.write(dataFile.read());
//    }
//    dataFile.close();

//}
}
