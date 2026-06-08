#include <SoftwareSerial.h>

SoftwareSerial unoSerial = SoftwareSerial(7,8);

void setup() {
  Serial.begin(9600);
  unoSerial.begin(9600);
}

void loop() {
  unoSerial.write('r');
  unoSerial.write('n');
  String response = unoSerial.readStringUntil('n');
  Serial.println(response);
//  Serial.println("length of response");
 // Serial.println(response.length());
  delay(1000);
}
