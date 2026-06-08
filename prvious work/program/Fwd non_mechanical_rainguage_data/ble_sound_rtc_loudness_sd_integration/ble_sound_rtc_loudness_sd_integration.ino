#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
int s=A0;
int sound = A0;
int loudness=A3;
File myFile;
RTC_DS1307 rtc;
void setup() {
// Open serial communications and wait for port to open:
Serial.begin(9600);
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
  if (!SD.begin(10)) {
 Serial.println("initialization failed!");
 while (1);
}
 Serial.println("initialization done.");
}

//myFile = SD.open("test3.txt", FILE_WRITE);

int readsound()
{
  for(int i=0; i<32; i++)
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
    myFile = SD.open("test1.txt", FILE_WRITE);
  if (myFile) {
   int p;
  int l;
  p=readsound();
  l=readloudness();
 // myFile = SD.open("test3.txt", FILE_WRITE);
  Serial.print("sound=\t");
  Serial.println(p);
  myFile.print("sound\t");
  myFile.println(p);
  Serial.print("loudness=\t");
  myFile.print("loudness\t");
  Serial.println(l);
  myFile.println(l);
  delay(60000);
  
  tmElements_t tm;

 if (RTC.read(tm)) {
    
    Serial.print("Time = ");
   // print2digits(tm.Hour);
     Serial.print(":");
   // print2digits(tm.Minute);
    Serial.print(":");
    //print2digits(tm.Second);
    myFile.print("Time= :\t");
    myFile.print(tm.Hour);
    myFile.print(":");
    myFile.print(tm.Minute);
    myFile.print(":");
    myFile.print(tm.Second);
    Serial.print(", Date (D/M/Y) = \t");
    Serial.print(tm.Day);
    Serial.print('/');
    Serial.print(tm.Month);
    Serial.print('/');
    Serial.println(tmYearToCalendar(tm.Year));
    myFile.print(", Date (D/M/Y) =\t ");
    myFile.print(tm.Day);
    myFile.print("/");
    myFile.print(tm.Month);
    myFile.print("/");
    myFile.println(tmYearToCalendar(tm.Year));
    myFile.close();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(60000);} 
}
myFile = SD.open("test1.txt");
if (myFile) {
  
Serial.println("test1.txt:");
// read from the file until there's nothing else in it:
while (myFile.available()) {
Serial.write(myFile.read());
}
myFile.close();
}}
 
 
