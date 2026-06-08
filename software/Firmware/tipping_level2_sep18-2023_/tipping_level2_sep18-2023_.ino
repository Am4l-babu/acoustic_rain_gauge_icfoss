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

 
int calibaration();
int timer();


int timer()
{
 currentMillis = millis(); 
 //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    int a=5;
    
    startMillis = currentMillis;  
    return a;
  } 
  else{
    int b=0;
  return b;
  }
}
void blink() {
  state = !state;
  flag++;
}



void setup() {
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
Serial.println("Date,Time,k,total rain");
 //myFile.print("Date","Time","k","loudness");

}

void loop() {
  calibaration();

}
int calibaration(){

int alam=timer();
if(alam==0)
{
  flag=0;
}
else
{
  int total_rain_in_0neminite=+ flag;
  int c=total_rain_in_0neminite;
  calib(c); 
   }
}
int calib(int k){
  
  int total_rain=k*0.28;
  myFile = SD.open("manu.txt", FILE_WRITE);
  if (myFile) { 
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
    Serial.print(k);
    Serial.print(',');
    Serial.println(total_rain);
    
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
    myFile.print(k);
    myFile.print(',');
    myFile.println(total_rain);
    myFile.print(',');
   
    
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
  
  myFile = SD.open("manumohan.txt");
if (myFile) {
  
Serial.println("manu.txt:");
// read from the file until there's nothing else in it:
while (myFile.available()) {
Serial.write(myFile.read());
}
myFile.close();
}

}
  
  
  
  
  
  
