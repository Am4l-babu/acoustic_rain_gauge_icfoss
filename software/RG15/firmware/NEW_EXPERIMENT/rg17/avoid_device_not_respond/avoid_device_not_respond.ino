#include <SoftwareSerial.h>

SoftwareSerial unoSerial = SoftwareSerial(7,8);

void setup() {
  Serial.begin(9600);
  unoSerial.begin(9600);
}

void loop() {
 
  String response = unoSerial.readString();
  Serial.println(response);
  
 
  delay(1000);
}
