#include <Arduino.h>
#include <SoftwareSerial.h>
SoftwareSerial unoSerial = SoftwareSerial(7,8);

//#include "rg15arduino.h"
unsigned long previous_time = 0;
unsigned long previous_day_time = 0;
unsigned long current_time = 0;
uint8_t Accumulation = 0;
uint16_t Event_accumulation = 0;
uint16_t Total_accumulation = 0;
uint16_t IPH = 0;




void setup() {
  Serial.begin(9600);
  Serial.println("setup");
  unoSerial.begin(9600);
  

}

void loop() {
  current_time = millis();
  while (current_time - previous_day_time >= 30000)
  {
    polling() ;
    //data_mine();
    previous_day_time = current_time;
  }

}

bool polling() 
{
    unoSerial.write('r');
    unoSerial.write('\n');
    String response = unoSerial.readStringUntil('\n');
    if(response.startsWith(F("Acc"))) {

        char accB[7], eventAccB[7], totalAccB[7], rIntB[7],unit[7];

        sscanf (response.c_str(),"%*s %s %[^,] , %*s %s %*s %*s %s %*s %*s %s", &accB, &unit, &eventAccB, &totalAccB, &rIntB);

      float  acc = atof(accB);
      float  eventAcc = atof(eventAccB);
      float  totalAcc = atof(totalAccB);
       float rInt = atof(rIntB);
      float  storage(acc,eventAcc,totalAcc,rInt);
        //metric = !(unit[0] == 'i' && unit[1] == 'n');
        //void storage(acc,eventAcc,totalAcc,rInt);
        return true;
    }
    return false;
}

float storage(float A,float B,float C,float D)
{

Serial.println("Accumulation = ");
Serial.println(A);
Serial.println("Event_Accumulation = ");
Serial.println(B);
Serial.println("Total_Accumulation = ");
Serial.println(C);
Serial.println("Itensity = ");
Serial.println(D);
}
