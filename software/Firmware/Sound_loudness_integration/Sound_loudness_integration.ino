int loudness;
const int pinAdc = A0;
  int sound();
  int loud();
  
void setup()
{
    Serial.begin(9600);
    //Serial.begin(115200);
    Serial.print("loudness");
    Serial.print("\t");
    Serial.println("sound");
}

int loud()
  {
  
   int loudness = analogRead(A3);
   // Serial.print("loudness= ");
    Serial.print(loudness);
    //Serial.print("\t");
    Serial.print("\t");
    
    delay(200);
  
  }

int sound()
  {
  long sum = 0;
    for(int i=0; i<32; i++)
    {
        sum += analogRead(pinAdc);
    }

    sum >>= 5;
    
    //Serial.print("sound= ");
    Serial.println(sum);
    
    delay(200);
  
  
  
  }




void loop()
{
   sound();
   loud();
}
