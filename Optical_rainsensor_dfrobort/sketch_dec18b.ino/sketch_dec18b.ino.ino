#include <SoftwareSerial.h>

#define rxPin 10
#define txPin 11

SoftwareSerial softSerial =  SoftwareSerial(rxPin, txPin);

void setup()  {
  
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  softSerial.begin(115200);

  Serial.begin(115200);
}

void loop() {
  
  int data = softSerial.read();
  Serial.println(data);
  
  delay (1000);
}
