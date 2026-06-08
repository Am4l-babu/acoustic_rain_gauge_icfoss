#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

void setup () {
  Serial.begin(57600);
  Wire.begin();
  rtc.begin();

  pinMode(2, OUTPUT);
}

void loop () {
    DateTime now = rtc.now();
   
   if(now.hour() == 20 && now.minute() == 12){
     digitalWrite(2, HIGH);
   }
//DateTime now = rtc.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    Serial.println();
    delay(3000);
   
}
