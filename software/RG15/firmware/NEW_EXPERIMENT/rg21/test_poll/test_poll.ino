#include <SoftwareSerial.h>
#include "Stream.h"
unsigned long previous_time = 0;
unsigned long previous_day_time = 0;
unsigned long current_time = 0;
SoftwareSerial unoSerial = SoftwareSerial(7, 8);
void setup() {
  Serial.begin(9600);
  Stream* stream;

  Serial.println("setup");
  unoSerial.begin(9600);
}

void loop() {
bool hai;
  current_time = millis();
  if (current_time - previous_day_time >= 30000)
  {
    for(int i=0;i<5;i++)
    {
     hai=call();
    }
    if(hai==true)
     {
      Serial.println("yes");
      }
      else
      {
        Serial.println("No");
        }
    previous_day_time = current_time;
  }
  
  
}


bool call()
{
  unoSerial.write('r');
  unoSerial.write('\n');
  String response =  unoSerial.readStringUntil('\n');
  delay(1000);
  if (response.startsWith(F("Acc"))) {

    char accB[7], eventAccB[7], totalAccB[7], rIntB[7], unit[7];

    sscanf (response.c_str(), "%*s %s %[^,] , %*s %s %*s %*s %s %*s %*s %s", &accB, &unit, &eventAccB, &totalAccB, &rIntB);

    float acc = atof(accB);
    float eventAcc = atof(eventAccB);
    float totalAcc = atof(totalAccB);
    float rInt = atof(rIntB);
    Serial.print("acc = ");
    Serial.println( acc);
    Serial.print("eventAcc = ");
    Serial.println( eventAcc);
    Serial.print("totalAcc = ");
    Serial.println(totalAcc);
    Serial.print("rInt = ");
    Serial.println(rInt);
    delay(1000);
    return true;
  }
  else{
    return false;
    }
  }
