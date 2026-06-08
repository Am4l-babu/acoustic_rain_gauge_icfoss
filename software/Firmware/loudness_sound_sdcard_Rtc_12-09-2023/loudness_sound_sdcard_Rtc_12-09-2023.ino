#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
int addition, sum, p, l;
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 60000;
int s = A0;
int sound = A0;
int loudness = A3;
File myFile;
RTC_DS1307 rtc;

void setup() {
  Serial.begin(9600);
  SD.begin();

  while (!Serial);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  else {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    digitalWrite(7,HIGH);
    delay(1000);
    while (1);
  }
  Serial.println("initialization done.");
  Serial.println("Date,Time,Sound,loudness");
  myFile.print("Date,Time,Sound, loudness");



}
int readsound()
{
  for (int i = 0; i < 32; i++)
  {
    s += analogRead(0);
  }
  s >>= 5;

  //loudness = analogRead(A3);
  return s;
}
int readloudness()
{
  loudness = analogRead(A3);

  return loudness;
}

void loop() {
  int sound, loudness;
  myFile = SD.open("manu.txt", FILE_WRITE);
  if (myFile) {
    while (analogRead(A0) > 0)
    {
      currentMillis = millis();

      p = readsound();
      Serial.print("sound p = ");
      Serial.println(p);
      delay(1000);
      l = readloudness();
      Serial.print("loudness = ");
      Serial.println(l);
      delay(1000);
      sum += p;
      Serial.print("sound sum = ");
      Serial.println(sum);
      addition += l;
      Serial.print("loudness sum = ");
      Serial.println(addition);
      if (currentMillis - startMillis >= period)
      {
        sound = sum / 60;
        loudness = addition / 60;
        startMillis = currentMillis;
        delay(100);
        break;
      }
    }
    addition = 0;
    sum = 0;


    p = 0;
    l = 0;
    Serial.println("debug");
    Serial.println(p);
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
      Serial.print(sound);
      Serial.print(',');
      Serial.println(loudness);
      myFile.print(tmYearToCalendar(tm.Year));
      myFile.print('/');
      myFile.print(tm.Month);
      myFile.print('/');
      myFile.print(tm.Day);
      myFile.print(",");
      myFile.print(" ");
      myFile.print(tm.Hour);
      myFile.print(":");
      myFile.print(tm.Minute);
      myFile.print(":");
      myFile.print(tm.Second);
      myFile.print(',');
      myFile.print(' ');
      myFile.print(sound);
      myFile.print(',');
      myFile.println(loudness);
      myFile.close();
      digitalWrite(7,HIGH);
      delay(1000);
       digitalWrite(7,LOW);
       delay(10);
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
  myFile = SD.open("manu.txt");
  if (myFile) {

    Serial.println("manu.txt:");
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  }
}
