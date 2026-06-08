#include "rg15arduino.h"
#include <SoftwareSerial.h>
SoftwareSerial mySerial(7, 8); // RX, TX

#ifndef _RG15ARDUINO_H_
#define _RG15ARDUINO_H_
#include "Stream.h"


void setup() {
    Serial.begin(9600);
    Serial.println("setup"); 
    mySerial.begin(9600);
    rg15.setStream(&mySerial);
}
RG15Arduino#poll()

void loop() {
    if(rg15.poll()) {
        Serial.print("Accumulation: ");
        Serial.print(rg15.acc,3);
        Serial.print(rg15.unit);
        Serial.write(", Event Accumulation: ");
        Serial.print(rg15.eventAcc,3);
        Serial.print(rg15.unit);
        Serial.write(", Total Accumulation: ");
        Serial.print(rg15.totalAcc,3);          
        Serial.print(rg15.unit);
        Serial.write(", IPH: ");
        Serial.println(rg15.rInt,3);
    } else {
       Serial.println("Timeout!");
    }
    delay(1000);
}
