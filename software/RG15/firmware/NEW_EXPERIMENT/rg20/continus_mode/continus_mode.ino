#include <Arduino.h>
#include <SoftwareSerial.h>
//#include "rg15arduino.h"
#include "Stream.h"

String response;
SoftwareSerial mySerial(7, 8); // RX, TX

//RG15Arduino rg15;
String getAvailable()
{
    
  }

//bool RG15Arduino::setBaud(int baudCode) {
//    stream->write("b ");
//    stream->write(baudCode);
//    stream->write('\n');
//    String response = stream->readStringUntil('\n');
//    return response.charAt(2) == baudCode;
//}


//bool RG15Arduino::sendCommandLetter(char a) {
//    stream->write(a);
//    stream->write('\n');
//    String response = stream->readStringUntil('\n');
//    return response.charAt(0) == a;
//}


void setup() {
    Serial.begin(9600);
    Stream* stream;

  
  
    Serial.println("setup"); 
    mySerial.begin(9600);
    
}

void loop() {
  
    mySerial.write("/0");
    String response=mySerial.readString();
    
    Serial.println(response);
    delay(1000);
    
   // else{
      // Serial.println("not available");
      // delay(1000);
     // }
}
