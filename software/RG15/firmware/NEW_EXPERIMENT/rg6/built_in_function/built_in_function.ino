#include <Arduino.h>
#include <SoftwareSerial.h>
//#include "rg15arduino.h"
unsigned long previous_time = 0;
unsigned long previous_day_time = 0;
unsigned long current_time = 0;
uint8_t Accumulation = 0;
uint16_t Event_accumulation = 0;
uint16_t Total_accumulation = 0;
uint16_t IPH = 0;
uint8_t A;
uint16_t E;
uint16_t T;
uint16_t I;
SoftwareSerial mySerial(7, 8); // RX, TX




RG15Arduino rg15;

void setup() {
  Serial.begin(9600);
  Serial.println("setup");
  mySerial.begin(9600);
  rg15.setStream(&mySerial);
}

void loop() {
  current_time = millis();
  while (current_time - previous_day_time >= 60000)
  {
    rain();
    //data_mine();
    previous_day_time = current_time;
  }

}

void rain()
{

  if (rg15.poll()) {
    Serial.print("Accumulation: ");
    Serial.print(rg15.acc);
    Serial.print(rg15.unit);
    Serial.write(", Event Accumulation: ");
    Serial.print(rg15.eventAcc);
    Serial.print(rg15.unit);
    Serial.write(", Total Accumulation: ");
    Serial.print(rg15.totalAcc);
    Serial.print(rg15.unit);
    Serial.write(", IPH: ");
    Serial.println(rg15.rInt);

    A = uint16_t(rg15.acc * 100);
    E = uint16_t(rg15.acc * 100);
    T = uint16_t(rg15.acc * 100);
    I = uint16_t(rg15.acc * 100);

  } else {
    Serial.println("Timeout!");
  }
  delay(1000);
  
}


void data_mine()
{

uint8_t Acc = A;
uint16_t Event_acc = E;
uint16_t Total_acc = T;
uint16_t Iten = I;
Serial.println("Accumulation = ");
Serial.println(Acc);
Serial.println("Event_Accumulation = ");
Serial.println(Event_acc);
Serial.println("Total_Accumulation = ");
Serial.println(Total_acc);
Serial.println("Itensity = ");
Serial.println(Iten);



  
}


bool poll() 
{
    stream->write('r');
    stream->write('\n');
    String response = stream->readStringUntil('\n');
    if(response.startsWith(F("Acc"))) {

        char accB[7], eventAccB[7], totalAccB[7], rIntB[7];

        sscanf (response.c_str(),"%*s %s %[^,] , %*s %s %*s %*s %s %*s %*s %s", &accB, &unit, &eventAccB, &totalAccB, &rIntB);

        acc = atof(accB);
        eventAcc = atof(eventAccB);
        totalAcc = atof(totalAccB);
        rInt = atof(rIntB);

        metric = !(unit[0] == 'i' && unit[1] == 'n');

        return true;
    }
    return false;
}


  
}
