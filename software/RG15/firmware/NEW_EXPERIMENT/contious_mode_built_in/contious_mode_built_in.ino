#include <SoftwareSerial.h>

SoftwareSerial unoSerial = SoftwareSerial(7,8);



void setup() {
  Serial.begin(9600);
  unoSerial.begin(9600);
  unoSerial.write('c');
  unoSerial.write('\n');
}

void loop() {

  String response = unoSerial.readStringUntil('\n');
  Serial.println(response);
  if(response.startsWith("Acc")) {
    char acc[7], eventAcc[7], totalAcc[7], rInt[7], unit[4];
    sscanf (response.c_str(),"%*s %s %[^,] , %*s %s %*s %*s %s %*s %*s %s", &acc, &unit, &eventAcc, &totalAcc, &rInt);
    
    Serial.print("Accumulation: ");
    Serial.print(atof (acc),3);  
    Serial.println(unit);
    Serial.print("Event Accumulation: ");
    Serial.print(atof (eventAcc),3);  
    Serial.println(unit);
    Serial.print("Total Accumulation: ");
    Serial.print(atof (totalAcc),3);  
    Serial.println(unit);
    Serial.print("IPH: ");
    Serial.print(atof (rInt), 3);
    Serial.println(" IPH\n");
  }
     
    

  delay(3000);
}
