
#include <SoftwareSerial.h>

SoftwareSerial unoSerial = SoftwareSerial(7,8);
void Poll();
void setup() {
  Serial.begin(9600);
  unoSerial.begin(9600);
}

void Poll()
{
  unoSerial.begin(9600);
  delay(1000);
  unoSerial.write('r');
  unoSerial.write('\n');
  //unoSerial.flush();
}
void loop() {
 
    Poll();
    String response = unoSerial.readStringUntil('\n');
    Serial.println(response);
    if (response.startsWith("Acc"))
    {
    //slicing string
    char Acc[7], EventAcc[7], TotalAcc[7], RInt[7], unit[4];
    sscanf (response.c_str(), "%*s %s %[^,] , %*s %s %*s %*s %s %*s %*s %s", &Acc, &unit, &EventAcc, &TotalAcc, &RInt);

    // converting char array to float
    float float_Acc = atof(Acc);
    float float_EventAcc = atof(EventAcc);
    float float_TotalAcc = atof(TotalAcc);
    float float_RInt = atof(RInt);

     Serial.print("accumulation = ");
     Serial.println(atof(Acc));
     Serial.print("eventAccumulation = ");
     Serial.println(atof(EventAcc));
     Serial.print("Totalcommunication = ");
     Serial.println(atof(TotalAcc));
     Serial.print("Rint =");
     Serial.println(atof(RInt));
     delay(1000);
    delay(60000);
    
    }
  }
