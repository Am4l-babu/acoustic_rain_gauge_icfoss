#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
int addition,sum,p,l;
unsigned long startMillis;  
unsigned long currentMillis;
const unsigned long period = 60000;
int s=A0;
int sound = A0;
int loudness=A3;
File myFile;
RTC_DS1307 rtc;    
 
void setup() {
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
  if (!SD.begin(4)) {
 Serial.println("initialization failed!");
 while (1);
}
 Serial.println("initialization done.");
Serial.println("Date,Time,Sound,loudness");
 //myFile.print("Date","Time","Sound","loudness");


 
}
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


void loop() 
{
  int sound,loudness;
  myFile = SD.open("manu1.txt", FILE_WRITE);
  if (myFile) {
     
   
    p=readsound();
   delay(1000);
    l=readloudness();
    delay(1000);
   sum+=p;
   addition+=l;
   currentMillis = millis();
   while (currentMillis - startMillis >= period)
   {
    sound=sum/60;
    loudness=addition/60;
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
    myFile.print(sound);
    myFile.print(',');
    myFile.println(loudness);
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

myFile = SD.open("manu1.txt");
if (myFile) {
  
Serial.println("manu1.txt:");
// read from the file until there's nothing else in it:
while (myFile.available()) {
Serial.write(myFile.read());
}
myFile.close();
}
     
    startMillis = currentMillis;
    delay(100);
    break;
    
    
   }
   
}
}
