

#include <SoftwareSerial.h>




#include <SPI.h>
#include <SoftwareSerial.h>
SoftwareSerial unoSerial = SoftwareSerial(7, 8);
int a = 0;
int flag;
unsigned long previous_time = 0;
unsigned long previous_day_time = 0;
unsigned long current_time = 0;
int previous_accumulation = 0;

struct
{
  int Accumulation ;
  int Event_Accumulation ;
  int Total_Accumulation ;
  int Rain_Intensity ;
} mydata;




void setup()
{

  while (!Serial);
  Serial.begin(9600);
  unoSerial.begin(9600);
 
  delay(100);     // per sample code on RF_95 test
  Serial.println(F("Starting"));

}
  




//unoSerial.write('n');
void rg()
{
  
  unoSerial.write('r');
  unoSerial.write('\n');
  String response = unoSerial.readStringUntil('\n');
  Serial.println(response);
  delay(1000);
  
  if (response.startsWith("Acc"))  {
//    //slicing string
    Serial.println("slicing string");
    char Acc[7], EventAcc[7], TotalAcc[7], RInt[7], unit[4];
    sscanf (response.c_str(), "%*s %s %[^,] , %*s %s %*s %*s %s %*s %*s %s", &Acc, &unit, &EventAcc, &TotalAcc, &RInt);

    //converting char array to float
    float float_Acc = atof(Acc);
    float float_EventAcc = atof(EventAcc);
    float float_TotalAcc = atof(TotalAcc);
    float float_RInt = atof(RInt);
    delay(1000);
    Serial.println();
    int Accumulation = int(float_Acc * 100);
    int Event_Accumulation = int(float_EventAcc * 100);
    int Total_Accumulation = int(float_TotalAcc * 100);
    int Rain_Intensity = int(float_RInt * 100);
//    Serial.print("Accumulation");
//    Serial.println(Accumulation);
//    Serial.print(" Event_Accumulation=");
//    Serial.println(Event_Accumulation);
//    Serial.print("Total_Accumulation = ");
//    Serial.println(Total_Accumulation);
//    Serial.print("Rain_Intensity=");
//    Serial.println(Rain_Intensity);
    delay(1000);
    if (previous_accumulation != Event_Accumulation)
    {
      //Serial.println("Send Data to GATEWAY");
      mydata.Accumulation = int(float_Acc * 100);
      mydata.Event_Accumulation = int(float_EventAcc * 100);
      mydata.Total_Accumulation = int(float_TotalAcc * 100);
      mydata.Rain_Intensity = int(float_RInt * 100);
      delay(1000);
    }
    else
    {
     // Serial.println("NO FOOL AVAILABLE NOW ");

      mydata.Accumulation = int(float_Acc * 0);
      mydata.Event_Accumulation = int(float_EventAcc * 0);
      mydata.Total_Accumulation = int(float_TotalAcc * 0);
      mydata.Rain_Intensity = int(float_RInt * 0);
      delay(1000);
    }

    previous_accumulation = Event_Accumulation;
    Serial.println("previous_accumulation");
    Serial.println(previous_accumulation);

  
  
  } 
}

void loop() {

current_time = millis();
  while (current_time - previous_day_time >= 30000)
  {
     rg();
    Serial.print("Accumulation");
    Serial.println( mydata.Accumulation );
    Serial.print(" Event_Accumulation=");
    Serial.println( mydata.Event_Accumulation);
    Serial.print("Total_Accumulation = ");
    Serial.println(mydata.Total_Accumulation);
    Serial.print("Rain_Intensity=");
    Serial.println(mydata.Rain_Intensity);
     previous_day_time = current_time;
  }

   
     

  


}
