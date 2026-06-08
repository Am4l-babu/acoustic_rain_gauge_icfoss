#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
File myFile;
RTC_DS1307 rtc;

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 60000;  //the value is a number of milliseconds
const byte ledPin = 13;    //using the built in LED
const byte interruptPin = 2;
volatile byte state = LOW;
int count=0;
void counting();
int totalcount;
int flag;
int total_rain;
int avg_rain_count;
int rain;
int avg_rain;


void blink() {
  state = !state;
  flag++;
}





void setup()
{
  Serial.begin(9600);  //start Serial in case we need to print debugging info
  pinMode(ledPin, OUTPUT);
  startMillis = millis();
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, RISING);
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
 Serial.println("initialization done.");
Serial.println("Date,Time,avg_rain_count,rain,avg_rain");
 //myFile.print("Date","Time","Sound","loudness");
}

void loop()
{
digitalWrite(ledPin, state);
  myFile = SD.open("manu.txt", FILE_WRITE);
  if (myFile) {
  int rain_count=+flag;
  Serial.println(rain_count);
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    total_rain=flag;
    avg_rain_count=total_rain/60;
    rain=total_rain*0.28;
    avg_rain =avg_rain_count*0.28;
    
   //digitalWrite(ledPin, !digitalRead(ledPin));  //if so, change the state of the LED.  Uses a neat trick to change the state
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
    flag=0;
  }
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
    Serial.print(avg_rain_count);
    Serial.print(',');
    Serial.println(rain);
    Serial.print(',');
    Serial.println(avg_rain);
    myFile.print(tm.Day);
    myFile.print('/');
    myFile.print(tm.Month);
    myFile.print('/');
    myFile.print(tmYearToCalendar(tm.Year));
    myFile.print(",");
    myFile.print(tm.Hour);
    myFile.print(":");
    myFile.print(tm.Minute);
    myFile.print(":");
    myFile.print(tm.Second);
    myFile.print(',');
    myFile.print(avg_rain_count);
    myFile.print(',');
    myFile.println(rain);
    myFile.print(',');
    myFile.println(avg_rain);
    myFile.close();
    
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
